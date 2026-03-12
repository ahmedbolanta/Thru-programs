#ifndef TN_ESCROW_PROGRAM_H
#define TN_ESCROW_PROGRAM_H

#include <thru-sdk/c/tn_sdk.h>

/* Error codes */
#define TN_ESCROW_ERR_INVALID_INSTRUCTION_DATA_SIZE  (0x3000UL)
#define TN_ESCROW_ERR_INVALID_INSTRUCTION_TYPE       (0x3001UL)
#define TN_ESCROW_ERR_ACCOUNT_CREATE_FAILED          (0x3002UL)
#define TN_ESCROW_ERR_ACCOUNT_SET_WRITABLE_FAILED    (0x3003UL)
#define TN_ESCROW_ERR_ACCOUNT_RESIZE_FAILED          (0x3004UL)
#define TN_ESCROW_ERR_ACCOUNT_DATA_ACCESS_FAILED     (0x3005UL)
#define TN_ESCROW_ERR_ALREADY_FUNDED                 (0x3006UL)
#define TN_ESCROW_ERR_NOT_FUNDED                     (0x3007UL)
#define TN_ESCROW_ERR_ALREADY_COMPLETED              (0x3008UL)

/* Instruction types */
#define TN_ESCROW_INSTRUCTION_INITIALIZE  (0U)
#define TN_ESCROW_INSTRUCTION_FUND        (1U)
#define TN_ESCROW_INSTRUCTION_RELEASE     (2U)
#define TN_ESCROW_INSTRUCTION_REFUND      (3U)

/* Escrow status */
#define TN_ESCROW_STATUS_INITIALIZED  (0U)
#define TN_ESCROW_STATUS_FUNDED       (1U)
#define TN_ESCROW_STATUS_RELEASED     (2U)
#define TN_ESCROW_STATUS_REFUNDED     (3U)

/* Initialize escrow instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
    uchar escrow_seed[TN_SEED_SIZE];
    uint proof_size;
    /* proof_data follows dynamically based on proof_size */
} tn_escrow_init_args_t;

/* Fund escrow instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
    ulong amount;
} tn_escrow_fund_args_t;

/* Release escrow instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
} tn_escrow_release_args_t;

/* Refund escrow instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
} tn_escrow_refund_args_t;

/* Escrow account data structure */
typedef struct __attribute__((packed)) {
    ulong amount;
    uchar status;
} tn_escrow_account_t;

#endif
