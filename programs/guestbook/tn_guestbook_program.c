#include <stddef.h>
#include <thru-sdk/c/tn_sdk.h>
#include <thru-sdk/c/tn_sdk_syscall.h>
#include "tn_guestbook_program.h"

static void handle_create(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_guestbook_create_args_t const *args = (tn_guestbook_create_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    uchar const *proof_data = NULL;
    if (args->proof_size > 0) {
        proof_data = instruction_data + sizeof(tn_guestbook_create_args_t);
    }

    ulong result = tsys_account_create(account_idx, args->guestbook_seed, proof_data, args->proof_size);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_CREATE_FAILED);
    }

    result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    result = tsys_account_resize(account_idx, sizeof(tn_guestbook_account_t));
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_RESIZE_FAILED);
    }

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_guestbook_account_t *guestbook = (tn_guestbook_account_t *)account_data;
    guestbook->entry_count = 0;

    tsdk_return(TSDK_SUCCESS);
}

static void handle_sign(uchar const *instruction_data, ulong instruction_data_sz TSDK_PARAM_UNUSED) {
    tn_guestbook_sign_args_t const *args = (tn_guestbook_sign_args_t const *)instruction_data;
    ushort account_idx = args->account_index;

    if (args->message_len > TN_GUESTBOOK_MAX_MESSAGE_LEN) {
        tsdk_revert(TN_GUESTBOOK_ERR_MESSAGE_TOO_LONG);
    }

    void *account_data = tsdk_get_account_data_ptr(account_idx);
    if (account_data == NULL) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_DATA_ACCESS_FAILED);
    }

    tn_guestbook_account_t *guestbook = (tn_guestbook_account_t *)account_data;

    if (guestbook->entry_count >= TN_GUESTBOOK_MAX_ENTRIES) {
        tsdk_revert(TN_GUESTBOOK_ERR_GUESTBOOK_FULL);
    }

    ulong result = tsys_set_account_data_writable(account_idx);
    if (result != TSDK_SUCCESS) {
        tsdk_revert(TN_GUESTBOOK_ERR_ACCOUNT_SET_WRITABLE_FAILED);
    }

    tn_guestbook_entry_t *entry = &guestbook->entries[guestbook->entry_count];
    entry->message_len = args->message_len;

    for (uchar i = 0; i < args->message_len; i++) {
        entry->message[i] = args->message[i];
    }

    guestbook->entry_count++;

    uchar event_data[2];
    event_data[0] = guestbook->entry_count;
    event_data[1] = args->message_len;
    tsys_emit_event(event_data, 2);

    tsdk_return(TSDK_SUCCESS);
}

TSDK_ENTRYPOINT_FN void start(uchar const *instruction_data, ulong instruction_data_sz) {
    if (instruction_data_sz < sizeof(uint)) {
        tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
    }

    uint const *instruction_type = (uint const *)instruction_data;

    switch (*instruction_type) {
        case TN_GUESTBOOK_INSTRUCTION_CREATE:
            if (instruction_data_sz < sizeof(tn_guestbook_create_args_t)) {
                tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            tn_guestbook_create_args_t const *create_args = (tn_guestbook_create_args_t const *)instruction_data;
            ulong expected_size = sizeof(tn_guestbook_create_args_t) + create_args->proof_size;
            if (instruction_data_sz != expected_size) {
                tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_create(instruction_data, instruction_data_sz);
            break;

        case TN_GUESTBOOK_INSTRUCTION_SIGN:
            if (instruction_data_sz != sizeof(tn_guestbook_sign_args_t)) {
                tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE);
            }
            handle_sign(instruction_data, instruction_data_sz);
            break;

        default:
            tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_TYPE);
    }

    tsdk_revert(TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_TYPE);
}
