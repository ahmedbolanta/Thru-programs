# Voting Program

A decentralized voting system for creating and managing proposals on-chain.

## Deployed Address

| | Address |
|---|---------|
| **Program** | `taP41Wj4PMLed_IL9b3ErpK_qbc4UOl8PGD6hfUIt4waGN` |
| **Meta** | `taCR1Rx9YY2HyfpGaXdelKEo6UKON34mGlmV9zxDWRhcRE` |

## Features

- **Create Proposal**: Initialize a new proposal with yes=0, no=0, is_open=true
- **Vote Yes**: Increment yes vote count (only if proposal is open)
- **Vote No**: Increment no vote count (only if proposal is open)
- **Close Proposal**: Finalize voting and prevent further votes

## Data Structure

```c
typedef struct __attribute__((packed)) {
    ulong yes_votes;  // 8 bytes
    ulong no_votes;   // 8 bytes
    uchar is_open;    // 1 byte (1 = open, 0 = closed)
} tn_voting_proposal_t;
```

**Total: 17 bytes**

## Instructions

### Create Proposal (0x00)

Creates a new proposal account.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x00000000` |
| account_index | 2 bytes | `0x0200` |
| seed | 32 bytes | Proposal seed (padded) |
| proof_size | 4 bytes | Size of proof data |
| proof_data | variable | State proof |

### Vote Yes (0x01)

Casts a yes vote on an open proposal.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x01000000` |
| account_index | 2 bytes | `0x0200` |

**Total: 6 bytes**

### Vote No (0x02)

Casts a no vote on an open proposal.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x02000000` |
| account_index | 2 bytes | `0x0200` |

**Total: 6 bytes**

### Close Proposal (0x03)

Closes voting on a proposal.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x03000000` |
| account_index | 2 bytes | `0x0200` |

**Total: 6 bytes**

## Example Usage

### Vote Yes on Existing Proposal

```bash
thru-cli txn execute --fee 0 --fee-payer mykey \
  --readwrite-accounts <PROPOSAL_ADDRESS> \
  taP41Wj4PMLed_IL9b3ErpK_qbc4UOl8PGD6hfUIt4waGN \
  010000000200
```

### Vote No

```bash
thru-cli txn execute --fee 0 --fee-payer mykey \
  --readwrite-accounts <PROPOSAL_ADDRESS> \
  taP41Wj4PMLed_IL9b3ErpK_qbc4UOl8PGD6hfUIt4waGN \
  020000000200
```

### Close Proposal

```bash
thru-cli txn execute --fee 0 --fee-payer mykey \
  --readwrite-accounts <PROPOSAL_ADDRESS> \
  taP41Wj4PMLed_IL9b3ErpK_qbc4UOl8PGD6hfUIt4waGN \
  030000000200
```

## Events

On vote or close, emits event with vote counts (16 bytes):
- Bytes 0-7: yes_votes (little-endian uint64)
- Bytes 8-15: no_votes (little-endian uint64)

## Error Codes

| Code | Name | Description |
|------|------|-------------|
| 0x2000 | INVALID_INSTRUCTION_DATA_SIZE | Wrong instruction data length |
| 0x2001 | INVALID_INSTRUCTION_TYPE | Unknown instruction |
| 0x2002 | ACCOUNT_CREATE_FAILED | Failed to create account |
| 0x2003 | ACCOUNT_SET_WRITABLE_FAILED | Failed to set writable |
| 0x2004 | ACCOUNT_RESIZE_FAILED | Failed to resize account |
| 0x2005 | ACCOUNT_DATA_ACCESS_FAILED | Failed to access data |
| 0x2006 | PROPOSAL_ALREADY_CLOSED | Cannot vote on closed proposal |

## Building

```bash
mkdir -p voting/examples
cd voting

cat > GNUmakefile << 'EOF'
BASEDIR:=$(CURDIR)/build
THRU_C_SDK_DIR:=$(HOME)/.thru/sdk/c/thru-sdk
include $(THRU_C_SDK_DIR)/thru_c_program.mk
EOF

cat > examples/Local.mk << 'EOF'
$(call make-bin,tn_voting_program_c,tn_voting_program,,-ltn_sdk)
EOF

# Copy source files to examples/
make
thru-cli program create my_voting build/thruvm/bin/tn_voting_program_c.bin
```
