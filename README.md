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
