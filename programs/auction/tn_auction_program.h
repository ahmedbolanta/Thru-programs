#ifndef TN_AUCTION_PROGRAM_H
#define TN_AUCTION_PROGRAM_H

#include <thru-sdk/c/tn_sdk.h>

/* Error codes */
#define TN_AUCTION_ERR_INVALID_INSTRUCTION_DATA_SIZE  (0x5000UL)
#define TN_AUCTION_ERR_INVALID_INSTRUCTION_TYPE       (0x5001UL)
#define TN_AUCTION_ERR_ACCOUNT_CREATE_FAILED          (0x5002UL)
#define TN_AUCTION_ERR_ACCOUNT_SET_WRITABLE_FAILED    (0x5003UL)
#define TN_AUCTION_ERR_ACCOUNT_RESIZE_FAILED          (0x5004UL)
#define TN_AUCTION_ERR_ACCOUNT_DATA_ACCESS_FAILED     (0x5005UL)
#define TN_AUCTION_ERR_AUCTION_ENDED                  (0x5006UL)
#define TN_AUCTION_ERR_AUCTION_NOT_ENDED              (0x5007UL)
#define TN_AUCTION_ERR_BID_TOO_LOW                    (0x5008UL)
#define TN_AUCTION_ERR_ALREADY_CLAIMED                (0x5009UL)
#define TN_AUCTION_ERR_NO_BIDS                        (0x500AUL)

/* Instruction types */
#define TN_AUCTION_INSTRUCTION_CREATE    (0U)
#define TN_AUCTION_INSTRUCTION_BID       (1U)
#define TN_AUCTION_INSTRUCTION_END       (2U)
#define TN_AUCTION_INSTRUCTION_CLAIM     (3U)

/* Auction status */
#define TN_AUCTION_STATUS_ACTIVE   (0U)
#define TN_AUCTION_STATUS_ENDED    (1U)
#define TN_AUCTION_STATUS_CLAIMED  (2U)

/* Create auction instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
    uchar auction_seed[TN_SEED_SIZE];
    uint proof_size;
    /* proof_data follows dynamically based on proof_size */
} tn_auction_create_args_t;

/* Bid instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
    ulong bid_amount;
} tn_auction_bid_args_t;

/* End auction instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
} tn_auction_end_args_t;

/* Claim instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
} tn_auction_claim_args_t;

/* Auction account data structure */
typedef struct __attribute__((packed)) {
    ulong highest_bid;
    ulong bid_count;
    uchar status;
} tn_auction_account_t;

#endif
