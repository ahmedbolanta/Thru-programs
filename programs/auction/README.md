# Auction Program

A fully on-chain English auction built for the Thru Network Alphanet using the C SDK.

## Deployed Address

| | Address |
|---|---|
| Program | `ta-ZnLvUsT9oJ5YFllzOpNNqPkkvxHkB4XRXyfmTdt15zx` |
| Meta | `tayA3-ayoBqUOk-1rXIejft5Ko6cnRqV1FTqlw7ECBgg5w` |

## Instructions

| Instruction | Type ID | Data Size |
|-------------|---------|-----------|
| Create | 0 | 146 bytes (with 104-byte proof) |
| Bid | 1 | 14 bytes |
| End | 2 | 6 bytes |
| Claim | 3 | 6 bytes |

## Instruction Hex Formats

### Create (146 bytes)
```
00000000                    # instruction_type = 0 (4 bytes LE)
0200                        # account_index (2 bytes LE)
[seed padded to 32 bytes]   # 64 hex chars
68000000                    # proof_size = 104 (4 bytes LE)
[proof data hex]            # 104 bytes from make-state-proof
```

### Bid (14 bytes)
```
01000000          # instruction_type = 1 (4 bytes LE)
0200              # account_index (2 bytes LE)
f401000000000000  # bid_amount as uint64 LE (e.g. 500)
```

### End (6 bytes)
```
02000000  # instruction_type = 2 (4 bytes LE)
0200      # account_index (2 bytes LE)
```

### Claim (6 bytes)
```
03000000  # instruction_type = 3 (4 bytes LE)
0200      # account_index (2 bytes LE)
```

## Error Codes

| Code | Name |
|------|------|
| `0x5000` | INVALID_INSTRUCTION_DATA_SIZE |
| `0x5001` | INVALID_INSTRUCTION_TYPE |
| `0x5002` | ACCOUNT_CREATE_FAILED |
| `0x5003` | ACCOUNT_SET_WRITABLE_FAILED |
| `0x5004` | ACCOUNT_RESIZE_FAILED |
| `0x5005` | ACCOUNT_DATA_ACCESS_FAILED |
| `0x5006` | AUCTION_ENDED |
| `0x5007` | AUCTION_NOT_ENDED |
| `0x5008` | BID_TOO_LOW |
| `0x5009` | ALREADY_CLAIMED |
| `0x500A` | NO_BIDS |

## Build

```bash
cd ~/thru-auction && make
```

Output: `build/thruvm/bin/tn_auction_program_c.bin`

## Full Lifecycle Example

```bash
# 1. Deploy
thru program create auction_v2 ~/thru-auction/build/thruvm/bin/tn_auction_program_c.bin

# 2. Derive state account
thru program derive-address <PROGRAM_ACCOUNT> auction_one

# 3. Get state proof
thru txn make-state-proof creating <DERIVED_ADDRESS>

# 4. Create auction
thru txn execute --fee 0 --readwrite-accounts <DERIVED> <PROGRAM> <CREATE_HEX>

# 5. Bid
thru txn execute --fee 0 --readwrite-accounts <DERIVED> <PROGRAM> 010000000200f401000000000000

# 6. End auction
thru txn execute --fee 0 --readwrite-accounts <DERIVED> <PROGRAM> 020000000200

# 7. Claim
thru txn execute --fee 0 --readwrite-accounts <DERIVED> <PROGRAM> 030000000200
```
