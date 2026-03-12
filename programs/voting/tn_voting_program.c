#include <stddef.h>
#include <thru-sdk/c/tn_sdk.h>
#include <thru-sdk/c/tn_sdk_syscall.h>
#include "tn_voting_program.h"

static void handle_create_proposal(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_voting_create_args_t const *args = (tn_voting_create_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    uchar const *proof_data = NULL;
    if (args->proof_size > 0) {
        proof_data = instruction_data + sizeof(tn_voting_create_args_t);
    }

    ulong result = tsys_account_create(account_idx, args->proposal_seed, proof_data, args->proof_size);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_CREATE_FAILED);
    }

    result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    result = tsys_account_resize(account_idx, sizeof(tn_voting_proposal_t));
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_RESIZE_FAILED);
    }

    void* account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_voting_proposal_t* proposal = (tn_voting_proposal_t*)account_data;
    proposal->yes_votes = 0UL;
    proposal->no_votes = 0UL;
    proposal->is_open = 1;  /* Proposal starts open */

    tsdk_return(TSDK_SUCCESS);
}

static void handle_vote_yes(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_voting_vote_args_t const *args = (tn_voting_vote_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    void* account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_voting_proposal_t* proposal = (tn_voting_proposal_t*)account_data;
    
    if (proposal->is_open == 0) {
        tsdk_revert(TN_VOTING_ERR_PROPOSAL_ALREADY_CLOSED);
    }

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    proposal->yes_votes++;

    /* Emit event with current vote counts */
    ulong votes[2] = {proposal->yes_votes, proposal->no_votes};
    tsys_emit_event((uchar const *)votes, sizeof(votes));

    tsdk_return(TSDK_SUCCESS);
}

static void handle_vote_no(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_voting_vote_args_t const *args = (tn_voting_vote_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    void* account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_voting_proposal_t* proposal = (tn_voting_proposal_t*)account_data;
    
    if (proposal->is_open == 0) {
        tsdk_revert(TN_VOTING_ERR_PROPOSAL_ALREADY_CLOSED);
    }

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    proposal->no_votes++;

    /* Emit event with current vote counts */
    ulong votes[2] = {proposal->yes_votes, proposal->no_votes};
    tsys_emit_event((uchar const *)votes, sizeof(votes));

    tsdk_return(TSDK_SUCCESS);
}

static void handle_close_proposal(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_voting_close_args_t const *args = (tn_voting_close_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    void* account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_voting_proposal_t* proposal = (tn_voting_proposal_t*)account_data;

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_VOTING_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    proposal->is_open = 0;  /* Close the proposal */

    /* Emit final results */
    ulong votes[2] = {proposal->yes_votes, proposal->no_votes};
    tsys_emit_event((uchar const *)votes, sizeof(votes));

    tsdk_return(TSDK_SUCCESS);
}

TSDK_ENTRYPOINT_FN void start(uchar const *instruction_data, ulong instruction_data_sz) {
    if (instruction_data_sz < sizeof(uint)) {
        tsdk_revert(TN_VOTING_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    uint const *instruction_type = (uint const *)instruction_data;

    switch (*instruction_type) {
        case TN_VOTING_INSTRUCTION_CREATE_PROPOSAL:
            if (instruction_data_sz < sizeof(tn_voting_create_args_t)) {
                tsdk_revert(TN_VOTING_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            tn_voting_create_args_t const *create_args = (tn_voting_create_args_t const *)instruction_data;
            ulong expected_size = sizeof(tn_voting_create_args_t) + create_args->proof_size;
            if (instruction_data_sz != expected_size) {
                tsdk_revert(TN_VOTING_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_create_proposal(instruction_data, instruction_data_sz);
            break;

        case TN_VOTING_INSTRUCTION_VOTE_YES:
            if (instruction_data_sz != sizeof(tn_voting_vote_args_t)) {
                tsdk_revert(TN_VOTING_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_vote_yes(instruction_data, instruction_data_sz);
            break;

        case TN_VOTING_INSTRUCTION_VOTE_NO:
            if (instruction_data_sz != sizeof(tn_voting_vote_args_t)) {
                tsdk_revert(TN_VOTING_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_vote_no(instruction_data, instruction_data_sz);
            break;

        case TN_VOTING_INSTRUCTION_CLOSE_PROPOSAL:
            if (instruction_data_sz != sizeof(tn_voting_close_args_t)) {
                tsdk_revert(TN_VOTING_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_close_proposal(instruction_data, instruction_data_sz);
            break;

        default:
            tsdk_revert(TN_VOTING_ERR_INVALID_INSTRUCTION_TYPE);
    }

    tsdk_revert(TN_VOTING_ERR_INVALID_INSTRUCTION_TYPE);
}
