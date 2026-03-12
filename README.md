# Thru Network Alphanet Developer Guide

A comprehensive, step-by-step guide to building on Thru Network's Alphanet. This repository includes setup instructions, token operations, nameservice usage, and four complete smart contract examples with deployed addresses you can interact with today.

## 🌐 What is Thru Network?

Thru Network is a high-performance blockchain platform. This guide covers development on the Alphanet testnet.

## 📋 Table of Contents

1. [Prerequisites](#prerequisites)
2. [CLI Installation](#cli-installation)
3. [Account Setup](#account-setup)
4. [Token Operations](#token-operations)
5. [Nameservice](#nameservice)
6. [Program Development](#program-development)
7. [Deployed Programs](#deployed-programs)

---

## Prerequisites

- **macOS** (Apple Silicon M1/M2/M3) or Linux
- **Rust** toolchain installed ([rustup.rs](https://rustup.rs))
- **Homebrew** (macOS)

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
cargo install thru-cli --locked
```

**Important:** Always use `--locked` to avoid dependency conflicts.

### Step 3: Verify Installation

```bash
thru-cli --version
```

Expected output: `thru-cli 0.2.1` or higher

### Step 4: Verify Network Connection

```bash
thru-cli getversion
```

Expected output should show node version and RPC details.

---

## Account Setup

### Step 1: Create a Key

```bash
thru-cli keys generate mykey
```

This creates a keypair stored in `~/.thru/cli/config.yaml`

### Step 2: View Your Public Key

```bash
thru-cli keys list
```

### Step 3: Get Your Full Public Key Address

```bash
thru-cli keys show mykey
```

**Save this address!** It starts with `ta...` and you'll need it for all operations.

### Step 4: Create On-Chain Account

```bash
thru-cli account create mykey
```

---

## Token Operations

### Create a Token Mint

```bash
thru-cli token initialize-mint <YOUR_PUBLIC_KEY> <TOKEN_NAME> <32_BYTE_HEX_SEED> --fee-payer mykey
```

**Example:**
```bash
thru-cli token initialize-mint taXXX...XXX MYTOKEN 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef --fee-payer mykey
```

### Create a Token Account

```bash
thru-cli token initialize-account <MINT_ADDRESS> <YOUR_PUBLIC_KEY> <32_BYTE_HEX_SEED> --fee-payer mykey
```

### Mint Tokens

```bash
thru-cli token mint-to <MINT_ADDRESS> <TOKEN_ACCOUNT_ADDRESS> <YOUR_PUBLIC_KEY> <AMOUNT> --fee-payer mykey
```

**Note:** Amount includes decimals. For 1000 tokens with 9 decimals: `1000000000000`

### Transfer Tokens

```bash
thru-cli token transfer <FROM_TOKEN_ACCOUNT> <TO_TOKEN_ACCOUNT> <AMOUNT> --fee-payer mykey
```

---

## Nameservice

### Create a Registrar (Top-Level Domain)

```bash
thru-cli nameservice init-root <REGISTRAR_NAME> --fee-payer mykey
```

**Example:**
```bash
thru-cli nameservice init-root mydomain --fee-payer mykey
```

### Register a Subdomain

```bash
thru-cli nameservice register-subdomain <SUBDOMAIN_NAME> <REGISTRAR_ADDRESS> --fee-payer mykey
```

### Add Records to Domain

```bash
thru-cli nameservice append-record <DOMAIN_ADDRESS> <KEY> "<VALUE>" --fee-payer mykey
```

**Example:**
```bash
thru-cli nameservice append-record taXXX...XXX twitter "@myhandle" --fee-payer mykey
thru-cli nameservice append-record taXXX...XXX bio "Developer building on Thru" --fee-payer mykey
```

### Resolve a Domain

```bash
thru-cli nameservice resolve <DOMAIN_ADDRESS>
```

---

## Program Development

### Install the Toolchain

The automatic installer may fail on some systems. Use manual installation:

```bash
# Download toolchain for your platform
# For Apple Silicon (M1/M2/M3):
curl -L "https://github.com/user-attachments/files/19416109/thru-toolchain-Darwin-arm64-v0.2.6.tar.gz" -o /tmp/toolchain.tar.gz

# Extract to correct location
mkdir -p ~/.thru/sdk/toolchain
tar -xzf /tmp/toolchain.tar.gz -C ~/.thru/sdk/toolchain

# Move contents up one level (important!)
mv ~/.thru/sdk/toolchain/thru-toolchain-Darwin-arm64-v0.2.6/* ~/.thru/sdk/toolchain/
rmdir ~/.thru/sdk/toolchain/thru-toolchain-Darwin-arm64-v0.2.6

# Clean up
rm /tmp/toolchain.tar.gz
```

### Install C SDK

```bash
thru-cli dev sdk install c
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
cd my-program
make

# Deploy
thru-cli program create my_program_seed ~/my-program/build/thruvm/bin/my_program_c.bin
```

### Interacting with Programs

1. **Derive account address:**
```bash
thru-cli program derive-address <PROGRAM_ADDRESS> <SEED>
```

2. **Create state proof:**
```bash
thru-cli txn make-state-proof creating <DERIVED_ADDRESS>
```

3. **Execute transaction:**
```bash
thru-cli txn execute --fee 0 --fee-payer mykey --readwrite-accounts <DERIVED_ADDRESS> <PROGRAM_ADDRESS> <INSTRUCTION_HEX>
```

---

## Deployed Programs

These programs are live on Alphanet. You can interact with them directly!

### 1. Counter Program

A simple counter that demonstrates state management.

| | Address |
|---|---------|
| **Program** | `taSgjTJTEgwm64_fYDIYYPGJZsqUdbAFkG1FmDRUCt6uyr` |
| **Meta** | `taEcLF3gkAOt1WJZBkJxtePZlcNiBFZhq6v3nsMsndKCT0` |

**Features:**
- Create counter (instruction `0x00`)
- Increment counter (instruction `0x01`)

[View Source Code](./programs/counter/)

### 2. Voting Program

A decentralized voting system for proposals.

| | Address |
|---|---------|
| **Program** | `taP41Wj4PMLed_IL9b3ErpK_qbc4UOl8PGD6hfUIt4waGN` |
| **Meta** | `taCR1Rx9YY2HyfpGaXdelKEo6UKON34mGlmV9zxDWRhcRE` |

**Features:**
- Create proposal (instruction `0x00`)
- Vote yes (instruction `0x01`)
- Vote no (instruction `0x02`)
- Close proposal (instruction `0x03`)

[View Source Code](./programs/voting/)

### 3. Escrow Program

A DeFi primitive for conditional fund transfers.

| | Address |
|---|---------|
| **Program** | `ta-v68MOqS0J0IAuWuR8yvvU5sApM85Bc18EglXdZnywpw` |
| **Meta** | `taxqi8wcsx3FcoPd9oFHMTKchbgEAAt0rw00n0rBC3MVdo` |

**Features:**
- Initialize escrow (instruction `0x00`)
- Fund escrow (instruction `0x01`)
- Release funds (instruction `0x02`)
- Refund funds (instruction `0x03`)

[View Source Code](./programs/escrow/)

### 4. Guestbook Program

On-chain message storage demonstrating string handling.

| | Address |
|---|---------|
| **Program** | `taTPjHiniTYVemWFK-cOlRqzOBCoiIdQNXQ-prqtJ7eOtK` |
| **Meta** | `taCikRjdVS_9ae83bdndVC1Lx7b6e40TCBVBIHGYrTexo4` |

**Features:**
- Create guestbook (instruction `0x00`)
- Sign with message up to 64 bytes (instruction `0x01`)
- Stores up to 10 entries

[View Source Code](./programs/guestbook/)

---

## 📚 Resources

- [Thru Network Documentation](https://docs.thru.org/)
- [Building a C Program Guide](https://docs.thru.org/program-development/building-a-c-program)

---

## 🤝 Contributing

Found an issue or want to improve this guide? Open a PR!

---

## 📄 License

MIT License - feel free to use this guide and code examples.
