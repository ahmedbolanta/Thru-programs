# Escrow Program

A DeFi primitive for conditional fund transfers with state management.

## Deployed Address

| | Address |
|---|---------|
| **Program** | `ta-v68MOqS0J0IAuWuR8yvvU5sApM85Bc18EglXdZnywpw` |
| **Meta** | `taxqi8wcsx3FcoPd9oFHMTKchbgEAAt0rw00n0rBC3MVdo` |

## Features

- **Initialize**: Create a new escrow account
- **Fund**: Deposit funds into escrow (only from initialized state)
- **Release**: Release funds to recipient (only from funded state)
- **Refund**: Return funds to depositor (only from funded state)

## State Machine

```
INITIALIZED (0) --[fund]--> FUNDED (1) --[release]--> RELEASED (2)
                                      \--[refund]--> REFUNDED (3)
```

## Data Structure

```c
typedef struct __attribute__((packed)) {
    ulong amount;   // 8 bytes - escrowed amount
    uchar status;   // 1 byte - current state
} tn_escrow_account_t;
```

**Total: 9 bytes**

**Status values:**
- `0` = INITIALIZED
- `1` = FUNDED
- `2` = RELEASED
- `3` = REFUNDED

## Instructions

### Initialize (0x00)

Creates a new escrow account.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x00000000` |
| account_index | 2 bytes | `0x0200` |
| seed | 32 bytes | Escrow seed (padded) |
| proof_size | 4 bytes | Size of proof data |
| proof_data | variable | State proof |

### Fund (0x01)

Deposits funds into escrow.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x01000000` |
| account_index | 2 bytes | `0x0200` |
| amount | 8 bytes | Amount (little-endian uint64) |

**Total: 14 bytes (28 hex chars)**

**Example for 1000 units:**
```
010000000200e803000000000000
```

### Release (0x02)

Releases funds to recipient.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x02000000` |
| account_index | 2 bytes | `0x0200` |

**Total: 6 bytes**

### Refund (0x03)

Returns funds to depositor.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x03000000` |
| account_index | 2 bytes | `0x0200` |

**Total: 6 bytes**

## Example Usage

### Fund an Escrow with 1000 units

```bash
thru-cli txn execute --fee 0 --fee-payer mykey \
  --readwrite-accounts <ESCROW_ADDRESS> \
  ta-v68MOqS0J0IAuWuR8yvvU5sApM85Bc18EglXdZnywpw \
  010000000200e803000000000000
```

### Release Funds

```bash
thru-cli txn execute --fee 0 --fee-payer mykey \
  --readwrite-accounts <ESCROW_ADDRESS> \
  ta-v68MOqS0J0IAuWuR8yvvU5sApM85Bc18EglXdZnywpw \
  020000000200
```

### Refund Funds

```bash
thru-cli txn execute --fee 0 --fee-payer mykey \
  --readwrite-accounts <ESCROW_ADDRESS> \
  ta-v68MOqS0J0IAuWuR8yvvU5sApM85Bc18EglXdZnywpw \
  030000000200
```

## Events

On fund/release/refund, emits event (16 bytes):
- Bytes 0-7: amount (little-endian uint64)
- Bytes 8-15: status (little-endian uint64)

## Error Codes

| Code | Name | Description |
|------|------|-------------|
| 0x3000 | INVALID_INSTRUCTION_DATA_SIZE | Wrong instruction data length |
| 0x3001 | INVALID_INSTRUCTION_TYPE | Unknown instruction |
| 0x3002 | ACCOUNT_CREATE_FAILED | Failed to create account |
| 0x3003 | ACCOUNT_SET_WRITABLE_FAILED | Failed to set writable |
| 0x3004 | ACCOUNT_RESIZE_FAILED | Failed to resize account |
| 0x3005 | ACCOUNT_DATA_ACCESS_FAILED | Failed to access data |
| 0x3006 | ALREADY_FUNDED | Escrow already funded |
| 0x3007 | NOT_FUNDED | Escrow not in funded state |
| 0x3008 | ALREADY_COMPLETED | Escrow already released/refunded |

## Building

```bash
mkdir -p escrow/examples
cd escrow

cat > GNUmakefile << 'EOF'
BASEDIR:=$(CURDIR)/build
THRU_C_SDK_DIR:=$(HOME)/.thru/sdk/c/thru-sdk
include $(THRU_C_SDK_DIR)/thru_c_program.mk
EOF

cat > examples/Local.mk << 'EOF'
$(call make-bin,tn_escrow_program_c,tn_escrow_program,,-ltn_sdk)
EOF

# Copy source files to examples/
make
thru-cli program create my_escrow build/thruvm/bin/tn_escrow_program_c.bin
```
