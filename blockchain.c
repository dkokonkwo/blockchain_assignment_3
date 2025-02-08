#include "blockchain.h"

/**
 * createData: create data_t structure for a new block
 * @sender: sender details
 * @receiver: receiver details
 * @amount: transaction amount
 */
data_t *createData(const char *sender, const char *receiver, const char *amount) {
    data_t *new_data;
    transaction_t *new_trans = (transaction_t *)malloc(sizeof(transaction_t));
    if (!new_trans)
    {
        perror("Could not allocate memory for transaction");
        exit(EXIT_FAILURE);
    }
    strncpy(new_trans->sender, sender, DATASIZE_MAX - 1);
    new_trans->sender[DATASIZE_MAX - 1] = '\0';
    
    strncpy(new_trans->receiver, receiver, DATASIZE_MAX - 1);
    new_trans->receiver[DATASIZE_MAX - 1] = '\0';
    
    strncpy(new_trans->amount, amount, sizeof(new_trans->amount) - 1);
    new_trans->amount[sizeof(new_trans->amount) - 1] = '\0';

    new_trans->next = NULL;
    new_trans->index = 0;

    new_data = (data_t *)malloc(sizeof(data_t));
    if (!new_data)
    {
        perror("Could not allocate memory for data");
        free(new_trans);
        exit(EXIT_FAILURE);
    }
    new_data->head = new_trans;
    new_data->tail = new_trans;
    new_data->nb_transactions = 1;
    return new_data;
}

/**
 * createBlock - creates new block
 * @data: pointer to data to add to block
 * @prevHash: previous block hash
 * @difficulty: Proof of Work difficulty level
 */
block_t *createBlock(int index, list_of_transactions *transactions, const unsigned char *prevHash, int difficulty) {
    block_t *newBlock = (block_t *)malloc(sizeof(block_t));
    if (!newBlock) {
        perror("Failed to allocate memory for block");
        exit(EXIT_FAILURE);
    }

    newBlock->index = index;
    newBlock->timestamp = (uint64_t)time(NULL);
    newBlock->transactions = transactions;
    if (prevHash)
        memcpy(newBlock->prevHash, prevHash, SHA256_DIGEST_LENGTH);
    else
        memset(newBlock->prevHash, 0, SHA256_DIGEST_LENGTH); 
    memset(newBlock->currHash, 0, SHA256_DIGEST_LENGTH);
    newBlock->next = NULL;
    newBlock->nonce = 0;

    mine_block(newBlock, difficulty);
    return newBlock;
}

/**
 * addBlock - adds block to blockchain
 * @blockchain: pointer to blockchain
 * @transactions: pointer to list of transactions
 * Return: Nothing
 */
void addBlock(Blockchain *blockchain, list_of_transactions *transactions) {
    block_t *newBlock = createBlock(blockchain->length, transactions, blockchain->tail->currHash, blockchain->difficulty);

    blockchain->tail->next = newBlock;
    blockchain->tail = newBlock;
    blockchain->length++;
}


/**
 * initBlockchain - initializes new blockchain with genesis block
 * Return: pointer to blockchain
 */
Blockchain *initBlockchain() {
    Blockchain *blockchain = (Blockchain *)malloc(sizeof(Blockchain));
    if (!blockchain) {
        perror("Failed to allocate memory for blockchain");
        exit(EXIT_FAILURE);
    }

    // Create the genesis block
    list_of_transactions *genesis_transactions = createData("Genesis", "Blockchain", "0");
    unsigned char genesisHash[SHA256_DIGEST_LENGTH] = {0};
    block_t *genesisBlock = createBlock(0, genesis_transactions, genesisHash);

    blockchain->head = blockchain->tail = genesisBlock;
    blockchain->length = 1;

    return blockchain;
}

/**
 * validateBlockchain - ensures that previous block's hash matches with new block's hash
 * @blockchain: pointer to blockchain to validate
 * Return: 1 if valid, or 0 if invalid
 */
int validateBlockchain(Blockchain *blockchain) {
    if (!blockchain || !blockchain->head)
        return 0;
    unsigned char tmpHash[SHA256_DIGEST_LENGTH] = {0};
    unsigned char calculatedHash[SHA256_DIGEST_LENGTH];
    block_t *current = blockchain->head;

    while (current)
    {
        calculateHash(current, calculatedHash);
        if (memcmp(current->currHash, calculatedHash, SHA256_DIGEST_LENGTH) != 0 || memcmp(current->prevHash, tmpHash, SHA256_DIGEST_LENGTH) != 0) {
            return 0;
        }    
        memcpy(tmpHash, current->currHash, SHA256_DIGEST_LENGTH);
        current = current->next;
    }
    return 1;
}

/**
 * printBlockchain: prints blocks in blockchain
 * @blockchain: pointer to blockchain
 * Return: Nothing
 */
void printBlockchain(Blockchain *blockchain) {
    block_t *current = blockchain->head;
    while (current) {
        printf("Block %d\n", current->index);
        printf("Timestamp: %lu\n", current->timestamp);
        transaction_t *trans = current->transactions->head;
        while (trans)
        {
            printf("  Transaction %d: %s -> %s, Amount: %s\n", trans->index, trans->sender, trans->receiver, trans->amount);
            trans = trans->next;
        }

        printf("Previous Hash: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            printf("%02x", current->prevHash[i]);
        }
        printf("\n");

        printf("Current Hash: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            printf("%02x", current->currHash[i]);
        }
        printf("\n\n");

        current = current->next;
    }
}

/**
 * freeBlockchain - free block in blockchain and then the blockchain itself
 * @blockchain: pointer to blockchain
 * Return: Nothing
 */
void freeBlockchain(Blockchain *blockchain) {
    block_t *current = blockchain->head;
    while (current) {
        block_t *next = current->next;
        transaction_t *trans = current->transactions->head;
        while (trans) {
            transaction_t *next_trans = trans->next;
            free(trans);
            trans = next_trans;
        }
        free(current->transactions);
        free(current);
        current = next;
    }
    free(blockchain);
}
