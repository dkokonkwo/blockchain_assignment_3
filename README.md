Question 

You are tasked with developing a simple blockchain-based supply chain tracking system. The goal is to ensure the transparency and immutability of the supply chain records. The system will include the following components: blockchain data structures, transactions, block mining, and a command-line interface (CLI) for interacting with the blockchain.
Requirements:

Blockchain Crypto: Implement basic cryptographic functions to secure the blockchain. Each block should have a cryptographic hash that ensures its integrity.
Blockchain Data Structures: Define the data structures for blocks and the blockchain. Each block should contain a list of transactions, a timestamp, the previous block's hash, and a nonce used for mining.
Transactions: Create a transaction structure that includes the item's ID, the item's description, and a digital signature. Ensure that each transaction is validated before being added to a block.
Block Mining: Implement a proof-of-work algorithm to mine new blocks. The mining process should involve finding a nonce that satisfies a given difficulty level (e.g., the hash of the block must start with a certain number of zeros).
CLI Implementation for Blockchain: Develop a simple command-line interface to interact with the blockchain. The CLI should support the following commands:
create_blockchain: Initialize a new blockchain.
add_transaction: Add a new transaction to the blockchain.
mine_block: Mine a new block and add it to the blockchain.
print_blockchain: Display the entire blockchain.


# Blockchain CLI Tool

This command-line tool allows you to create, manage, and interact with a blockchain. It provides functionalities for creating a blockchain, adding transactions, mining new blocks, and printing the current state of the blockchain.

## Installation

### **1. Clone the Repository**
```sh
$ git clone <repository-url>
$ cd <repository-folder>
```

### **2. Compile the Source Code**
Use the provided **Makefile** to build the executable commands:
```sh
$ sudo make
```
This will generate the following CLI tools inside `/usr/bin/`:
- `create_blockchain`
- `add_transaction`
- `mine_block`
- `print_blockchain`

If needed, you can clean up the build files using:
```sh
$ sudo make clean
```

## Usage

### **1. Create a New Blockchain**
To initialize a new blockchain:
```sh
$ create_blockchain
```
This creates the genesis block and serializes the blockchain.

### **2. Add a New Transaction**
To add a new transaction to the unspent transaction pool:
```sh
$ add_transaction
```
You'll be prompted to enter:
- Sender address
- Receiver address
- Amount

### **3. Mine a New Block**
To mine a new block, process transactions, and update the blockchain:
```sh
$ mine_block
```
This will:
- Validate and process transactions
- Perform Proof-of-Work (PoW) mining
- Adjust difficulty based on mining time
- Add the mined block to the blockchain
- Empty the unspent transaction pool

### **4. Print the Blockchain**
To view the current blockchain state:
```sh
$ print_blockchain
```
This will display all blocks with their details, including transactions and hashes.

## File Storage
The blockchain and transactions are stored in serialized files:
- `BLOCKCHAIN_DATABASE`: Stores blockchain data
- `TRANSACTION_DATABASE`: Stores unspent transactions

## Troubleshooting
- **Permission Issues:** Ensure that you have write access to `/usr/bin/` or modify the Makefile to place binaries in `/<current folder>`.
- **File Not Found Errors:** Run `create_blockchain` first to initialize the blockchain.
- **Segmentation Faults:** Check that the blockchain and transaction pool files exist and are correctly formatted.

## Contributing
Feel free to submit issues or pull requests to improve the tool.

## License
This project is licensed under the MIT License.


