# Counter Program

A simple on-chain counter demonstrating fundamental Thru program development.

## Deployed Address

| | Address |
|---|---------|
| **Program** | `taSgjTJTEgwm64_fYDIYYPGJZsqUdbAFkG1FmDRUCt6uyr` |
| **Meta** | `taEcLF3gkAOt1WJZBkJxtePZlcNiBFZhq6v3nsMsndKCT0` |

## Features

- **Create Counter**: Initialize a new counter with value 0
- **Increment**: Increase counter by 1, emits event with new value

## Data Structure

```c
typedef struct __attribute__((packed)) {
    ulong counter_value;  // 8 bytes
} tn_counter_account_t;
```

## Instructions

### Create Counter (0x00)

Creates a new counter account.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x00000000` |
| account_index | 2 bytes | `0x0200` |
| seed | 32 bytes | Account seed (padded) |
| proof_size | 4 bytes | Size of proof data |
| proof_data | variable | State proof |

### Increment (0x01)

Increments the counter by 1.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x01000000` |
| account_index | 2 bytes | `0x0200` |

**Total: 6 bytes**

## Example Usage

### Create a Counter

```bash
# 1. Derive account address
thru-cli program derive-address taSgjTJTEgwm64_fYDIYYPGJZsqUdbAFkG1FmDRUCt6uyr my_counter

# 2. Create state proof
thru-cli txn make-state-proof creating <DERIVED_ADDRESS>

# 3. Execute create transaction
# Seed "my_counter" = 6d795f636f756e746572 (padded to 32 bytes)
thru-cli txn execute --fee 0 --fee-payer mykey \
  --readwrite-accounts <DERIVED_ADDRESS> \
  taSgjTJTEgwm64_fYDIYYPGJZsqUdbAFkG1FmDRUCt6uyr \
  000000000200<SEED_HEX_64>68000000<PROOF_HEX_208>
```

### Increment Counter

```bash
thru-cli txn execute --fee 0 --fee-payer mykey \
  --readwrite-accounts <COUNTER_ADDRESS> \
  taSgjTJTEgwm64_fYDIYYPGJZsqUdbAFkG1FmDRUCt6uyr \
  010000000200
```

## Events

On increment, emits an event with the new counter value (8 bytes, little-endian).

Example: `0100000000000000` = counter value 1

## Error Codes

| Code | Name | Description |
|------|------|-------------|
| 0x1000 | INVALID_INSTRUCTION_DATA_SIZE | Wrong instruction data length |
| 0x1001 | INVALID_INSTRUCTION_TYPE | Unknown instruction |
| 0x1002 | ACCOUNT_CREATE_FAILED | Failed to create account |
| 0x1003 | ACCOUNT_SET_WRITABLE_FAILED | Failed to set writable |
| 0x1004 | ACCOUNT_RESIZE_FAILED | Failed to resize account |
| 0x1005 | ACCOUNT_DATA_ACCESS_FAILED | Failed to access data |

## Building

```bash
# Setup project
mkdir -p counter/examples
cd counter

# Create GNUmakefile
cat > GNUmakefile << 'EOF'
BASEDIR:=$(CURDIR)/build
THRU_C_SDK_DIR:=$(HOME)/.thru/sdk/c/thru-sdk
include $(THRU_C_SDK_DIR)/thru_c_program.mk
EOF

# Create Local.mk
cat > examples/Local.mk << 'EOF'
$(call make-bin,tn_counter_program_c,tn_counter_program,,-ltn_sdk)
EOF

# Copy source files to examples/
# Build
make

# Deploy
thru-cli program create my_counter build/thruvm/bin/tn_counter_program_c.bin
```
