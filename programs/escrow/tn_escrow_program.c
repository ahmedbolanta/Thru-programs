#include <stddef.h>
#include <thru-sdk/c/tn_sdk.h>
#include <thru-sdk/c/tn_sdk_syscall.h>
#include "tn_escrow_program.h"

static void handle_initialize(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_escrow_init_args_t const *args = (tn_escrow_init_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    uchar const *proof_data = NULL;
    if (args->proof_size > 0) {
        proof_data = instruction_data + sizeof(tn_escrow_init_args_t);
    }

    ulong result = tsys_account_create(account_idx, args->escrow_seed, proof_data, args->proof_size);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_CREATE_FAILED);
    }

    result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    result = tsys_account_resize(account_idx, sizeof(tn_escrow_account_t));
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_RESIZE_FAILED);
    }

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_escrow_account_t *escrow = (tn_escrow_account_t *)account_data;
    escrow->amount = 0UL;
    escrow->status = TN_ESCROW_STATUS_INITIALIZED;

    tsdk_return(TSDK_SUCCESS);
}

static void handle_fund(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_escrow_fund_args_t const *args = (tn_escrow_fund_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_escrow_account_t *escrow = (tn_escrow_account_t *)account_data;

    if (escrow->status != TN_ESCROW_STATUS_INITIALIZED) {
        tsdk_revert(TN_ESCROW_ERR_ALREADY_FUNDED);
    }

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    escrow->amount = args->amount;
    escrow->status = TN_ESCROW_STATUS_FUNDED;

    ulong event_data[2] = {escrow->amount, (ulong)escrow->status};
    tsys_emit_event((uchar const *)event_data, sizeof(event_data));

    tsdk_return(TSDK_SUCCESS);
}

static void handle_release(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_escrow_release_args_t const *args = (tn_escrow_release_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_escrow_account_t *escrow = (tn_escrow_account_t *)account_data;

    if (escrow->status != TN_ESCROW_STATUS_FUNDED) {
        tsdk_revert(TN_ESCROW_ERR_NOT_FUNDED);
    }

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    escrow->status = TN_ESCROW_STATUS_RELEASED;

    ulong event_data[2] = {escrow->amount, (ulong)escrow->status};
    tsys_emit_event((uchar const *)event_data, sizeof(event_data));

    tsdk_return(TSDK_SUCCESS);
}

static void handle_refund(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_escrow_refund_args_t const *args = (tn_escrow_refund_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_escrow_account_t *escrow = (tn_escrow_account_t *)account_data;

    if (escrow->status != TN_ESCROW_STATUS_FUNDED) {
        tsdk_revert(TN_ESCROW_ERR_NOT_FUNDED);
    }

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_ESCROW_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    escrow->status = TN_ESCROW_STATUS_REFUNDED;

    ulong event_data[2] = {escrow->amount, (ulong)escrow->status};
    tsys_emit_event((uchar const *)event_data, sizeof(event_data));

    tsdk_return(TSDK_SUCCESS);
}

TSDK_ENTRYPOINT_FN void start(uchar const *instruction_data, ulong instruction_data_sz) {
    if (instruction_data_sz < sizeof(uint)) {
        tsdk_revert(TN_ESCROW_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    uint const *instruction_type = (uint const *)instruction_data;

    switch (*instruction_type) {
        case TN_ESCROW_INSTRUCTION_INITIALIZE:
            if (instruction_data_sz < sizeof(tn_escrow_init_args_t)) {
                tsdk_revert(TN_ESCROW_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            tn_escrow_init_args_t const *init_args = (tn_escrow_init_args_t const *)instruction_data;
            ulong expected_size = sizeof(tn_escrow_init_args_t) + init_args->proof_size;
            if (instruction_data_sz != expected_size) {
                tsdk_revert(TN_ESCROW_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_initialize(instruction_data, instruction_data_sz);
            break;

        case TN_ESCROW_INSTRUCTION_FUND:
            if (instruction_data_sz != sizeof(tn_escrow_fund_args_t)) {
                tsdk_revert(TN_ESCROW_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_fund(instruction_data, instruction_data_sz);
            break;

        case TN_ESCROW_INSTRUCTION_RELEASE:
            if (instruction_data_sz != sizeof(tn_escrow_release_args_t)) {
                tsdk_revert(TN_ESCROW_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_release(instruction_data, instruction_data_sz);
            break;

        case TN_ESCROW_INSTRUCTION_REFUND:
            if (instruction_data_sz != sizeof(tn_escrow_refund_args_t)) {
                tsdk_revert(TN_ESCROW_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_refund(instruction_data, instruction_data_sz);
            break;

        default:
            tsdk_revert(TN_ESCROW_ERR_INVALID_INSTRUCTION_TYPE);
    }

    tsdk_revert(TN_ESCROW_ERR_INVALID_INSTRUCTION_TYPE);
}
