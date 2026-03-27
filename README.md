# Thru Network Alphanet Developer Guide

A comprehensive, step-by-step guide to building on Thru Network's Alphanet. This repository includes setup instructions, token operations, nameservice usage, and five complete smart contract examples with deployed addresses you can interact with today.

## 🌐 What is Thru Network?

Thru Network is a high-performance blockchain platform. This guide covers development on the Alphanet testnet.

---

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [CLI Installation](#cli-installation)
- [Account Setup](#account-setup)
- [Funding Your Account](#funding-your-account)
- [Token Operations](#token-operations)
- [Nameservice](#nameservice)
- [Program Development](#program-development)
- [Deployed Programs](#deployed-programs)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

- macOS (Apple Silicon M1/M2/M3) or Linux
- Rust toolchain installed ([rustup.rs](https://rustup.rs))
- Homebrew (macOS)

---

## CLI Installation

### Step 1: Install buf CLI (required dependency)

```bash
# macOS - may need to fix permissions first
sudo chown -R $(whoami) /usr/local/bin

# Install buf
brew install bufbuild/buf/buf
```

### Step 2: Install Thru CLI

```bash
cargo install thru
```

> **Note:** The CLI was renamed from `thru-cli` to `thru` in v0.2.13+. All commands now use `thru` instead of `thru-cli`.

### Step 3: Verify Installation

```bash
thru --version
```

### Step 4: Verify Network Connection

```bash
thru rpc getversion
```

---

## Account Setup

### Step 1: Create a Key

```bash
thru keys generate mykey
```

This creates a keypair stored in `~/.thru/cli/config.yaml`

### Step 2: View Your Public Key

```bash
thru keys list
```

### Step 3: Get Your Full Public Key Address

```bash
thru keys show mykey
```

Save this address — it starts with `ta...` and you'll need it for all operations.

### Step 4: Create On-Chain Account

```bash
thru account create mykey
```

---

## Funding Your Account

Alphanet resets periodically. After a reset, accounts need to be recreated and funded via the faucet.

```bash
# Withdraw tokens FROM the faucet TO your account (up to 10,000 per tx)
thru faucet withdraw default 1000
thru faucet withdraw mykey 1000
```

> **Important:** Use `withdraw` (not `deposit`) to fund your account from the faucet. `deposit` sends tokens *into* the faucet program.

---

## Token Operations

### Create a Token Mint

```bash
thru token initialize-mint <YOUR_PUBLIC_KEY> <TOKEN_NAME> <32_BYTE_HEX_SEED>
```

### Create a Token Account

```bash
thru token initialize-account <MINT_ADDRESS> <YOUR_PUBLIC_KEY> <32_BYTE_HEX_SEED>
```

### Mint Tokens

```bash
thru token mint-to <MINT_ADDRESS> <TOKEN_ACCOUNT_ADDRESS> <YOUR_PUBLIC_KEY> <AMOUNT>
```

> **Note:** Amount includes decimals. For 1000 tokens with 9 decimals: `1000000000000`

### Transfer Tokens

```bash
thru token transfer <FROM_TOKEN_ACCOUNT> <TO_TOKEN_ACCOUNT> <AMOUNT>
```

---

## Nameservice

### Create a Registrar (Top-Level Domain)

```bash
thru nameservice init-root mydomain
```

### Register a Subdomain

```bash
thru nameservice register-subdomain <SUBDOMAIN_NAME> <REGISTRAR_ADDRESS>
```

### Add Records to Domain

```bash
thru nameservice append-record <DOMAIN_ADDRESS> twitter "@myhandle"
thru nameservice append-record <DOMAIN_ADDRESS> bio "Developer building on Thru"
thru nameservice append-record <DOMAIN_ADDRESS> github "https://github.com/myhandle"
```

### Resolve a Domain

```bash
thru nameservice resolve <DOMAIN_ADDRESS>
```

---

## Program Development

### Install the Toolchain

The automatic installer may fail on some systems. Use manual installation:

```bash
# For Apple Silicon (M1/M2/M3):
curl -L "https://github.com/user-attachments/files/19416109/thru-toolchain-Darwin-arm64-v0.2.6.tar.gz" -o /tmp/toolchain.tar.gz

mkdir -p ~/.thru/sdk/toolchain
tar -xzf /tmp/toolchain.tar.gz -C ~/.thru/sdk/toolchain

# Move contents up one level (important!)
mv ~/.thru/sdk/toolchain/thru-toolchain-Darwin-arm64-v0.2.6/* ~/.thru/sdk/toolchain/
rmdir ~/.thru/sdk/toolchain/thru-toolchain-Darwin-arm64-v0.2.6
rm /tmp/toolchain.tar.gz
```

### Install C SDK

```bash
thru dev sdk install c
```

### Program Structure

Every Thru C program needs this structure:

```
my-program/
├── GNUmakefile
└── examples/
    ├── Local.mk
    ├── my_program.h
    └── my_program.c
```

**GNUmakefile:**
```makefile
BASEDIR:=$(CURDIR)/build
THRU_C_SDK_DIR:=$(HOME)/.thru/sdk/c/thru-sdk
include $(THRU_C_SDK_DIR)/thru_c_program.mk
```

**examples/Local.mk:**
```makefile
$(call make-bin,my_program_c,my_program,,-ltn_sdk)
```

### Build and Deploy

```bash
# Build
cd my-program && make

# Fund account if needed
thru faucet withdraw default 1000

# Deploy
thru program create my_program_seed ~/my-program/build/thruvm/bin/my_program_c.bin

# Check deployment status
thru program status my_program_seed
```

### Interacting with Programs

```bash
# Derive account address
thru program derive-address <PROGRAM_ADDRESS> <SEED>

# Create state proof
thru txn make-state-proof creating <DERIVED_ADDRESS>

# Execute transaction (note: --authority instead of --fee-payer)
thru txn execute --fee 0 --readwrite-accounts <DERIVED_ADDRESS> <PROGRAM_ADDRESS> <INSTRUCTION_HEX>
```

### Instruction Data Format

#### Encoding Rules
- All multi-byte integers are **little-endian**
- Seeds are ASCII hex-encoded and **zero-padded to 32 bytes** (64 hex chars)
- The Create instruction is **146 bytes** (includes 104-byte state proof)

#### Little-Endian Quick Reference

| Value | uint32 LE | uint64 LE |
|-------|-----------|-----------|
| 0 | `00000000` | `0000000000000000` |
| 1 | `01000000` | `0100000000000000` |
| 104 | `68000000` | `6800000000000000` |
| 500 | `f4010000` | `f401000000000000` |
| 1000 | `e8030000` | `e803000000000000` |

#### Create Instruction Layout (146 bytes)
```
00000000                    # instruction_type = 0 (4 bytes LE)
0200                        # account_index = 2 (2 bytes LE)
[seed padded to 32 bytes]   # 32 bytes = 64 hex chars
68000000                    # proof_size = 104 (4 bytes LE)
[proof data]                # 104 bytes from make-state-proof
```

---

## Deployed Programs

These programs are live on Alphanet. You can interact with them directly!

> **Note:** Alphanet resets periodically. Addresses below reflect the most recent deployment.

### 1. Counter Program

A simple counter that demonstrates state management.

| | Address |
|---|---|
| Program | `taXKZxw-pjK1qmLwS0Gm4ZUVuWbeIYAalOFFE3dzSX_i3g` |
| Meta | `taJQrrpSZkLWEGeKW5t7HaaMWO6DFf9J1Io_0v1ZPU-h0j` |

**Features:**
- Create counter (instruction `0x00`)
- Increment counter (instruction `0x01`)

[View Source Code](./programs/counter/)

---

### 2. Voting Program

A decentralized voting system for proposals.

| | Address |
|---|---|
| Program | `taMCg1h3GYqR9_fyZPIA55MeTQG7TvKmpqf4ftTHrp61J_` |
| Meta | `taUShzPYg9R2U9B0plY-UYc45oqqPgeqwaS53lvgY2bjMr` |

**Features:**
- Create proposal (instruction `0x00`)
- Vote yes (instruction `0x01`)
- Vote no (instruction `0x02`)
- Close proposal (instruction `0x03`)

[View Source Code](./programs/voting/)

---

### 3. Escrow Program

A DeFi primitive for conditional fund transfers.

| | Address |
|---|---|
| Program | `taeu7BZAZx89eeCluEzYdpIQkG0BI7HdlMUQN6HVQQ58GY` |
| Meta | `taK0nXb2jTaPlgjJ5LXhAW1p1LUklgQWExkF70FKZIQ88x` |

**Features:**
- Initialize escrow (instruction `0x00`)
- Fund escrow (instruction `0x01`)
- Release funds (instruction `0x02`)
- Refund funds (instruction `0x03`)

[View Source Code](./programs/escrow/)

---

### 4. Guestbook Program

On-chain message storage demonstrating string handling.

| | Address |
|---|---|
| Program | `taXqFPQKYei0k7J5lq5GoMuw2VBydfmA7DuewfCUxYsGrD` |
| Meta | `taHC0ebrvFAscbbbAiF-KPMJrzKnMI7gL8a2pGc-yq87ke` |

**Features:**
- Create guestbook (instruction `0x00`)
- Sign with message up to 64 bytes (instruction `0x01`)
- Stores up to 10 entries

[View Source Code](./programs/guestbook/)

---

### 5. Auction Program

A fully on-chain English auction with bid validation and winner claiming.

| | Address |
|---|---|
| Program | `ta-ZnLvUsT9oJ5YFllzOpNNqPkkvxHkB4XRXyfmTdt15zx` |
| Meta | `tayA3-ayoBqUOk-1rXIejft5Ko6cnRqV1FTqlw7ECBgg5w` |

**Features:**
- Create auction (instruction `0x00`, 146 bytes with proof)
- Place bid (instruction `0x01`, 14 bytes)
- End auction (instruction `0x02`, 6 bytes)
- Claim winnings (instruction `0x03`, 6 bytes)

**Error Codes:**

| Code | Name |
|------|------|
| `0x5000` | INVALID_INSTRUCTION_DATA_SIZE |
| `0x5001` | INVALID_INSTRUCTION_TYPE |
| `0x5002` | ACCOUNT_CREATE_FAILED |
| `0x5006` | AUCTION_ENDED |
| `0x5007` | AUCTION_NOT_ENDED |
| `0x5008` | BID_TOO_LOW |
| `0x5009` | ALREADY_CLAIMED |
| `0x500A` | NO_BIDS |

[View Source Code](./programs/auction/)

---

## Troubleshooting

| Error | Solution |
|-------|----------|
| `Fee payer account not found` | Fund with faucet: `thru faucet withdraw <key> 1000` |
| `TN_RUNTIME_TXN_ERR_INVALID_PROGRAM_ACCOUNT` | Program doesn't exist; redeploy |
| `Timeout expired` | Network flakiness; just retry the command |
| `BadRecordMac` transport error | Network issue; retry immediately |
| `manager_error: Account not available (0x0504)` | Run `thru program status <seed>` — program may already be deployed |
| `user_error=0x1000` | Check instruction data byte count |
| `Upload buffer exists (cleanup needed)` | Run `thru uploader cleanup <seed>_temporary` |
| Toolchain not found for Darwin-x86_64 | Use manual arm64 download (see Program Development section) |

---

## 📚 Resources

- [Thru Network Documentation](https://docs.thru.org/)
- [Building a C Program Guide](https://docs.thru.org/program-development/building-a-c-program)
- [CLI Reference](https://docs.thru.org/cli-reference/overview)

---

## 🤝 Contributing

Found an issue or want to improve this guide? Open a PR!

---

## 📄 License

MIT License — feel free to use this guide and code examples.
