# Guestbook Program

On-chain message storage demonstrating string handling in Thru programs.

## Deployed Address

| | Address |
|---|---------|
| **Program** | `taTPjHiniTYVemWFK-cOlRqzOBCoiIdQNXQ-prqtJ7eOtK` |
| **Meta** | `taCikRjdVS_9ae83bdndVC1Lx7b6e40TCBVBIHGYrTexo4` |

## Features

- **Create Guestbook**: Initialize a new guestbook with 0 entries
- **Sign**: Add a message (up to 64 bytes) to the guestbook
- **Capacity**: Stores up to 10 entries

## Data Structure

```c
/* Single entry */
typedef struct __attribute__((packed)) {
    uchar message_len;                        // 1 byte
    uchar message[TN_GUESTBOOK_MAX_MESSAGE_LEN]; // 64 bytes
} tn_guestbook_entry_t;  // Total: 65 bytes per entry

/* Guestbook account */
typedef struct __attribute__((packed)) {
    uchar entry_count;                              // 1 byte
    tn_guestbook_entry_t entries[TN_GUESTBOOK_MAX_ENTRIES]; // 650 bytes
} tn_guestbook_account_t;  // Total: 651 bytes
```

## Instructions

### Create Guestbook (0x00)

Creates a new guestbook account.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x00000000` |
| account_index | 2 bytes | `0x0200` |
| seed | 32 bytes | Guestbook seed (padded) |
| proof_size | 4 bytes | Size of proof data |
| proof_data | variable | State proof |

### Sign Guestbook (0x01)

Adds a message to the guestbook.

**Instruction Data:**
| Field | Size | Description |
|-------|------|-------------|
| instruction_type | 4 bytes | `0x01000000` |
| account_index | 2 bytes | `0x0200` |
| message_len | 1 byte | Length of message (1-64) |
| message | 64 bytes | Message data (padded) |

**Total: 71 bytes (142 hex chars)**

## Example Usage

### Sign the Deployed Guestbook

To sign with message "Hello Thru!" (11 chars = 0x0B):

**Step 1: Convert message to hex**
```
"Hello Thru!" = 48656c6c6f205468727521
```

**Step 2: Pad to 64 bytes (128 hex chars)**
```
48656c6c6f20546872752100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```

**Step 3: Build instruction**
```
01000000    - instruction type (SIGN)
0200        - account index
0b          - message length (11)
48656c6c6f20546872752100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 - message
```

**Step 4: Execute**
```bash
# First create your own guestbook or use an existing one
thru-cli txn execute --fee 0 --fee-payer mykey \
  --readwrite-accounts <GUESTBOOK_ADDRESS> \
  taTPjHiniTYVemWFK-cOlRqzOBCoiIdQNXQ-prqtJ7eOtK \
  010000000200oBB48656c6c6f20546872752100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```

### Common Messages in Hex

| Message | Hex (padded to 128 chars) |
|---------|---------------------------|
| "Hello!" (6 chars, len=06) | `48656c6c6f210000...` (122 zeros) |
| "GM" (2 chars, len=02) | `474d0000...` (124 zeros) |
| "Thru rocks!" (11 chars, len=0B) | `5468727520726f636b732100...` (106 zeros) |

## Events

On sign, emits event (2 bytes):
- Byte 0: entry_count (total entries after signing)
- Byte 1: message_len (length of signed message)

Example: `0211` = 2 entries total, last message was 17 bytes

## Error Codes

| Code | Name | Description |
|------|------|-------------|
| 0x4000 | INVALID_INSTRUCTION_DATA_SIZE | Wrong instruction data length |
| 0x4001 | INVALID_INSTRUCTION_TYPE | Unknown instruction |
| 0x4002 | ACCOUNT_CREATE_FAILED | Failed to create account |
| 0x4003 | ACCOUNT_SET_WRITABLE_FAILED | Failed to set writable |
| 0x4004 | ACCOUNT_RESIZE_FAILED | Failed to resize account |
| 0x4005 | ACCOUNT_DATA_ACCESS_FAILED | Failed to access data |
| 0x4006 | MESSAGE_TOO_LONG | Message exceeds 64 bytes |
| 0x4007 | GUESTBOOK_FULL | Guestbook has 10 entries |

## Building

```bash
mkdir -p guestbook/examples
cd guestbook

cat > GNUmakefile << 'EOF'
BASEDIR:=$(CURDIR)/build
THRU_C_SDK_DIR:=$(HOME)/.thru/sdk/c/thru-sdk
include $(THRU_C_SDK_DIR)/thru_c_program.mk
EOF

cat > examples/Local.mk << 'EOF'
$(call make-bin,tn_guestbook_program_c,tn_guestbook_program,,-ltn_sdk)
EOF

# Copy source files to examples/
make
thru-cli program create my_guestbook build/thruvm/bin/tn_guestbook_program_c.bin
```

## ASCII to Hex Reference

| Char | Hex | Char | Hex | Char | Hex |
|------|-----|------|-----|------|-----|
| A | 41 | a | 61 | 0 | 30 |
| B | 42 | b | 62 | 1 | 31 |
| C | 43 | c | 63 | 2 | 32 |
| ... | ... | ... | ... | ... | ... |
| Z | 5A | z | 7A | 9 | 39 |
| (space) | 20 | ! | 21 | ? | 3F |
