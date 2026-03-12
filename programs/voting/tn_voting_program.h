#ifndef TN_VOTING_PROGRAM_H
#define TN_VOTING_PROGRAM_H

#include <thru-sdk/c/tn_sdk.h>

/* Error codes */
#define TN_VOTING_ERR_INVALID_INSTRUCTION_DATA_SIZE  (0x2000UL)
#define TN_VOTING_ERR_INVALID_INSTRUCTION_TYPE       (0x2001UL)
#define TN_VOTING_ERR_ACCOUNT_CREATE_FAILED          (0x2002UL)
#define TN_VOTING_ERR_ACCOUNT_SET_WRITABLE_FAILED    (0x2003UL)
#define TN_VOTING_ERR_ACCOUNT_RESIZE_FAILED          (0x2004UL)
#define TN_VOTING_ERR_ACCOUNT_DATA_ACCESS_FAILED     (0x2005UL)
#define TN_VOTING_ERR_PROPOSAL_ALREADY_CLOSED        (0x2006UL)

/* Instruction types */
#define TN_VOTING_INSTRUCTION_CREATE_PROPOSAL  (0U)
#define TN_VOTING_INSTRUCTION_VOTE_YES         (1U)
#define TN_VOTING_INSTRUCTION_VOTE_NO          (2U)
#define TN_VOTING_INSTRUCTION_CLOSE_PROPOSAL   (3U)

/* Create proposal instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
    uchar proposal_seed[TN_SEED_SIZE];
    uint proof_size;
    /* proof_data follows dynamically based on proof_size */
} tn_voting_create_args_t;

/* Vote instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
} tn_voting_vote_args_t;

/* Close proposal instruction arguments */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
} tn_voting_close_args_t;

/* Proposal account data structure */
typedef struct __attribute__((packed)) {
    ulong yes_votes;
    ulong no_votes;
    uchar is_open;  /* 1 = open, 0 = closed */
} tn_voting_proposal_t;

#endif
