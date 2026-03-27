#include <stddef.h>
#include <thru-sdk/c/tn_sdk.h>
#include <thru-sdk/c/tn_sdk_syscall.h>
#include "tn_auction_program.h"

static void handle_create(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_auction_create_args_t const *args = (tn_auction_create_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    uchar const *proof_data = NULL;
    if (args->proof_size > 0) {
        proof_data = instruction_data + sizeof(tn_auction_create_args_t);
    }

    ulong result = tsys_account_create(account_idx, args->auction_seed, proof_data, args->proof_size);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_CREATE_FAILED);
    }

    result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    result = tsys_account_resize(account_idx, sizeof(tn_auction_account_t));
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_RESIZE_FAILED);
    }

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_auction_account_t *auction = (tn_auction_account_t *)account_data;
    auction->highest_bid = 0UL;
    auction->bid_count = 0UL;
    auction->status = TN_AUCTION_STATUS_ACTIVE;

    tsdk_return(TSDK_SUCCESS);
}

static void handle_bid(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_auction_bid_args_t const *args = (tn_auction_bid_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_auction_account_t *auction = (tn_auction_account_t *)account_data;

    if (auction->status != TN_AUCTION_STATUS_ACTIVE) {
        tsdk_revert(TN_AUCTION_ERR_AUCTION_ENDED);
    }

    if (args->bid_amount <= auction->highest_bid) {
        tsdk_revert(TN_AUCTION_ERR_BID_TOO_LOW);
    }

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    auction->highest_bid = args->bid_amount;
    auction->bid_count++;

    ulong event_data[2] = {auction->highest_bid, auction->bid_count};
    tsys_emit_event((uchar const *)event_data, sizeof(event_data));

    tsdk_return(TSDK_SUCCESS);
}

static void handle_end(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_auction_end_args_t const *args = (tn_auction_end_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_auction_account_t *auction = (tn_auction_account_t *)account_data;

    if (auction->status != TN_AUCTION_STATUS_ACTIVE) {
        tsdk_revert(TN_AUCTION_ERR_AUCTION_ENDED);
    }

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    auction->status = TN_AUCTION_STATUS_ENDED;

    ulong event_data[2] = {auction->highest_bid, auction->bid_count};
    tsys_emit_event((uchar const *)event_data, sizeof(event_data));

    tsdk_return(TSDK_SUCCESS);
}

static void handle_claim(uchar const *instruction_data, ulong instruction_data_sz TSDPK_PARAM_UNUSED) {
    tn_auction_claim_args_t const *args = (tn_auction_claim_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_auction_account_t *auction = (tn_auction_account_t *)account_data;

    if (auction->status == TN_AUCTION_STATUS_ACTIVE) {
        tsdk_revert(TN_AUCTION_ERR_AUCTION_NOT_ENDED);
    }

    if (auction->status == TN_AUCTION_STATUS_CLAIMED) {
        tsdk_revert(TN_AUCTION_ERR_ALREADY_CLAIMED);
    }

    if (auction->bid_count == 0UL) {
        tsdk_revert(TN_AUCTION_ERR_NO_BIDS);
    }

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_AUCTION_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    auction->status = TN_AUCTION_STATUS_CLAIMED;

    ulong event_data[2] = {auction->highest_bid, auction->bid_count};
    tsys_emit_event((uchar const *)event_data, sizeof(event_data));

    tsdk_return(TSDK_SUCCESS);
}

TSDK_ENTRYPOINT_FN void start(uchar const *instruction_data, ulong instruction_data_sz) {
    if (instruction_data_sz < sizeof(uint)) {
        tsdk_revert(TN_AUCTION_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    uint const *instruction_type = (uint const *)instruction_data;

    switch (*instruction_type) {
        case TN_AUCTION_INSTRUCTION_CREATE:
            if (instruction_data_sz < sizeof(tn_auction_create_args_t)) {
                tsdk_revert(TN_AUCTION_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            tn_auction_create_args_t const *create_args = (tn_auction_create_args_t const *)instruction_data;
            ulong expected_size = sizeof(tn_auction_create_args_t) + create_args->proof_size;
            if (instruction_data_sz != expected_size) {
                tsdk_revert(TN_AUCTION_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_create(instruction_data, instruction_data_sz);
            break;

        case TN_AUCTION_INSTRUCTION_BID:
            if (instruction_data_sz != sizeof(tn_auction_bid_args_t)) {
                tsdk_revert(TN_AUCTION_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_bid(instruction_data, instruction_data_sz);
            break;

        case TN_AUCTION_INSTRUCTION_END:
            if (instruction_data_sz != sizeof(tn_auction_end_args_t)) {
                tsdk_revert(TN_AUCTION_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_end(instruction_data, instruction_data_sz);
            break;

        case TN_AUCTION_INSTRUCTION_CLAIM:
            if (instruction_data_sz != sizeof(tn_auction_claim_args_t)) {
                tsdk_revert(TN_AUCTION_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_claim(instruction_data, instruction_data_sz);
            break;

        default:
            tsdk_revert(TN_AUCTION_ERR_INVALID_INSTRUCTION_TYPE);
    }

    tsdk_revert(TN_AUCTION_ERR_INVALID_INSTRUCTION_TYPE);
}
