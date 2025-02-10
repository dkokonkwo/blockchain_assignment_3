#include "blockchain.h"

/**
 * createTranactions: create list of transactions for a new block main use for genesis block
 * @sender: sender details
 * @receiver: receiver details
 * @amount: transaction amount
 */
list_of_transactions *createTransactions(const char *sender, const char *receiver, const char *amount)
{
    list_of_transactions *new_list;
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

    new_list = (list_of_transactions *)malloc(sizeof(list_of_transactions));
    if (!new_list)
    {
        perror("Could not allocate memory for data");
        free(new_trans);
        exit(EXIT_FAILURE);
    }
    new_list->head = new_trans;
    new_list->tail = new_trans;
    new_list->nb_trans = 1;
    return new_list;
}

/**
 * createBlock - creates new block
 * @transactions: pointer to transactions to add to block
 * @prevHash: previous block hash
 * @difficulty: Proof of Work difficulty level
 */
block_t *createBlock(int index, list_of_transactions *transactions, const unsigned char *prevHash, int difficulty)
{
    block_t *newBlock = (block_t *)malloc(sizeof(block_t));
    if (!newBlock) {
        printf("Failed to allocate memory for block\n");
        return NULL;
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
    list_of_transactions *new_unspent = calloc(1, sizeof(*new_unspent));
    if (!new_unspent)
    {
        free(newBlock);
        printf("Could not allocated memory for new unspent\n");
        return NULL;
    }
    if (!serializeUnspent(new_unspent))
    {
        printf("Could not serialize new unspent\n");
        free(newBlock);
        return NULL;
    }
    return newBlock;
}

/**
 * addBlock - adds block to blockchain
 * @blockchain: pointer to blockchain
 * @block: pointer to mined block
 * Return: Nothing
 */
void addBlock(Blockchain *blockchain, block_t *block)
{
    if (!block)
        return;
    if (!blockchain->head)
        blockchain->head = blockchain->tail = block;
    else
    {
        blockchain->tail->next = block;
        blockchain->tail = block;
    }
    blockchain->length++;
}


/**
 * initBlockchain - initializes new blockchain with genesis block
 * Return: pointer to blockchain
 */
Blockchain *initBlockchain(void)
{
    Blockchain *blockchain = (Blockchain *)malloc(sizeof(Blockchain));
    if (!blockchain) {
        perror("Failed to allocate memory for blockchain");
        exit(EXIT_FAILURE);
    }

    blockchain->difficulty = INITIAL_DIFFICULTY;

    // Create the genesis block
    list_of_transactions *genesis_transactions = createTransactions("Genesis", "Blockchain", "0");
    unsigned char genesisHash[SHA256_DIGEST_LENGTH] = {0};
    block_t *genesisBlock = createBlock(0, genesis_transactions, genesisHash, blockchain->difficulty);

    blockchain->head = blockchain->tail = genesisBlock;
    blockchain->length = 1;

    return blockchain;
}

/**
 * validateBlockchain - ensures that previous block's hash matches with new block's hash
 * @blockchain: pointer to blockchain to validate
 * Return: 1 if valid, or 0 if invalid
 */
int validateBlockchain(Blockchain *blockchain)
{
    if (!blockchain || !blockchain->head)
        return 0;
    unsigned char tmpHash[SHA256_DIGEST_LENGTH] = {0};
    unsigned char calculatedHash[SHA256_DIGEST_LENGTH];
    block_t *current = blockchain->head;

    while (current)
    {
        calculateHash(current, current->nonce, calculatedHash);
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
void printBlockchain(Blockchain *blockchain)
{
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
void freeBlockchain(Blockchain *blockchain)
{
    block_t *current = blockchain->head;
    while (current) {
        block_t *next = current->next;
        freeTransactions(current->transactions);
        free(current);
        current = next;
    }
    free(blockchain);
}

/**
 * adjustDifficulty - adjusts mining difficulty based on block time
 * @prevTime: timestamp of previous block
 * @currentTime: timestamp of current block
 * @currentDifficulty: current difficulty level
 * Return: new difficulty level
 */
int adjustDifficulty(uint64_t prevTime, uint64_t currentTime, int currentDifficulty)
{
    double timeDiff = difftime(currentTime, prevTime);
    if (timeDiff < 10)
        return currentDifficulty + 1;  // Increase difficulty
    else if (timeDiff > 40 && currentDifficulty > 1)
        return currentDifficulty - 1;  // Decrease difficulty
    return currentDifficulty;
}