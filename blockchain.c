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
 */
block_t *createBlock(int index, data_t *data, const unsigned char *prevHash) {
    block_t *newBlock = (block_t *)malloc(sizeof(block_t));
    if (!newBlock) {
        perror("Failed to allocate memory for block");
        exit(EXIT_FAILURE);
    }

    newBlock->index = index;
    newBlock->timestamp = (uint64_t)time(NULL);
    newBlock->data = data;
    memcpy(newBlock->prevHash, prevHash, SHA256_DIGEST_LENGTH);
    calculateHash(newBlock, newBlock->currHash);
    newBlock->next = NULL;
    return newBlock;
}

/**
 * calculateHash - calculates the hash of a block
 * @block: pointer to block to calculate hash of
 * @hash: pointer to address to store hash
 * Return: Nothing
 */
void calculateHash(block_t *block, unsigned char *hash) {
    transaction_t *current_trans;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create EVP_MD_CTX\n");
        exit(EXIT_FAILURE);
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
        EVP_DigestUpdate(ctx, &block->index, sizeof(block->index)) != 1 ||
        EVP_DigestUpdate(ctx, &block->timestamp, sizeof(block->timestamp)) != 1 ||
        EVP_DigestUpdate(ctx, block->prevHash, SHA256_DIGEST_LENGTH) != 1) {
        fprintf(stderr, "Failed to calculate hash\n");
        EVP_MD_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    /* adding block's data to hash*/
    current_trans = block->data->head;
    while (current_trans) {
        EVP_DigestUpdate(ctx, current_trans->sender, sizeof(current_trans->sender));
        EVP_DigestUpdate(ctx, current_trans->receiver, sizeof(current_trans->receiver));
        EVP_DigestUpdate(ctx, current_trans->amount, sizeof(current_trans->amount));
        current_trans = current_trans->next;
    }

    if (EVP_DigestFinal_ex(ctx, hash, NULL) != 1) {
        fprintf(stderr, "Failed to finalize hash\n");
        EVP_MD_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    EVP_MD_CTX_free(ctx);
}

/**
 * addBlock - adds block to blockchain
 * @blockchain: pointer to blockchain
 * @data: pointer to new data
 * Return: Nothing
 */
void addBlock(Blockchain *blockchain, data_t *data) {
    block_t *newBlock = createBlock(blockchain->length, data, blockchain->tail->currHash);

    blockchain->tail->next = newBlock;
    blockchain->tail = newBlock;
    blockchain->length++;
}

/**
 * addTransaction - adds transaction to a block if its tail else new block
 * @blockchain: pointer to blockchain
 * @block: pointer to block to add transaction to
 * @sender: sender details
 * @receiver: receiver details
 * @amount: amount of transaction
 * Return: 1 on success or 0 on failure
 */
int addTransaction(Blockchain *blockchain, block_t *block, const char *sender, const char *receiver, const char *amount) {
    if (!block || !blockchain)
    {
        printf("Blockchain or block not valid\n");
        return 0;
    }
    if (blockchain->tail == block)
    {
        transaction_t *new_trans = (transaction_t *)malloc(sizeof(transaction_t));
        if (!new_trans)
        {
            printf("Failed to allocate memory for new transaction");
            return 0;
        }
        new_trans->index = block->data->nb_transactions++;
        
        strncpy(new_trans->sender, sender, DATASIZE_MAX - 1);
        new_trans->sender[DATASIZE_MAX - 1] = '\0';
        strncpy(new_trans->receiver, receiver, DATASIZE_MAX - 1);
        new_trans->receiver[DATASIZE_MAX - 1] = '\0';
        strncpy(new_trans->amount, amount, sizeof(new_trans->amount) - 1);
        new_trans->amount[sizeof(new_trans->amount) - 1] = '\0';

        new_trans->next = NULL;
        if (block->data->tail) {
            block->data->tail->next = new_trans;
        }
        else {
            block->data->head = new_trans;
        }
        block->data->tail = new_trans;
        return 1;
    }
    printf("Creating new block to add transaction to.....\n");
    data_t *new_data = createData(sender, receiver, amount);
    if (!new_data)
    {
        fprintf(stderr, "Failed to create new data for block\n");
        return 0;
    }
    addBlock(blockchain, new_data);
    printf("Added new block to blockchain\n");
    return 1;
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
    data_t *genesisData = createData("Genesis", "Blockchain", "0");
    unsigned char genesisHash[SHA256_DIGEST_LENGTH] = {0};
    block_t *genesisBlock = createBlock(0, genesisData, genesisHash);

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
