#ifndef TN_GUESTBOOK_PROGRAM_H
#define TN_GUESTBOOK_PROGRAM_H

#include <thru-sdk/c/tn_sdk.h>

/* Error codes */
#define TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_DATA_SIZE  (0x4000UL)
#define TN_GUESTBOOK_ERR_INVALID_INSTRUCTION_TYPE       (0x4001UL)
#define TN_GUESTBOOK_ERR_ACCOUNT_CREATE_FAILED          (0x4002UL)
#define TN_GUESTBOOK_ERR_ACCOUNT_SET_WRITABLE_FAILED    (0x4003UL)
#define TN_GUESTBOOK_ERR_ACCOUNT_RESIZE_FAILED          (0x4004UL)
#define TN_GUESTBOOK_ERR_ACCOUNT_DATA_ACCESS_FAILED     (0x4005UL)
#define TN_GUESTBOOK_ERR_MESSAGE_TOO_LONG               (0x4006UL)
#define TN_GUESTBOOK_ERR_GUESTBOOK_FULL                 (0x4007UL)

/* Instruction types */
#define TN_GUESTBOOK_INSTRUCTION_CREATE  (0U)
#define TN_GUESTBOOK_INSTRUCTION_SIGN    (1U)

/* Constants */
#define TN_GUESTBOOK_MAX_MESSAGE_LEN  (64U)
#define TN_GUESTBOOK_MAX_ENTRIES      (10U)

/* Create guestbook instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
    uchar guestbook_seed[TN_SEED_SIZE];
    uint proof_size;
    /* proof_data follows dynamically based on proof_size */
} tn_guestbook_create_args_t;

/* Sign guestbook instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
    uchar message_len;
    uchar message[TN_GUESTBOOK_MAX_MESSAGE_LEN];
} tn_guestbook_sign_args_t;

/* Single guestbook entry */
typedef struct __attribute__((packed)) {
    uchar message_len;
    uchar message[TN_GUESTBOOK_MAX_MESSAGE_LEN];
} tn_guestbook_entry_t;

/* Guestbook account data structure */
typedef struct __attribute__((packed)) {
    uchar entry_count;
    tn_guestbook_entry_t entries[TN_GUESTBOOK_MAX_ENTRIES];
} tn_guestbook_account_t;

#endif
