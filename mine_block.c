#include "blockchain.h"

/**
 * main - mines new block and adds it to blockchain
 * return: 0 always
 */
int main(void)
{
    Blockchain *blockchain;
    block_t *newBlock;
    uint64_t startTime, endTime;
    list_of_transactions *unspent;

    blockchain = deserializeBlockchain();
    if (!blockchain)
    {
        fprintf(stderr, "Could not deserialize blockchain\n");
        exit(EXIT_FAILURE);
    }

    if (!blockchain->tail)
    {
        fprintf(stderr, "Blockchain is empty. Initializing new blockchain...\n");
        freeBlockchain(blockchain);
        blockchain = initBlockchain();
    }

    unspent = deserializeUnspent();
    if (!unspent)
    {
        fprintf(stderr, "Could not deserialize unspent transactions\n");
        freeBlockchain(blockchain);
        exit(EXIT_FAILURE);
    }

    if (unspent->nb_trans == 0)
    {
        fprintf(stderr, "No transactions to mine\n");
        freeBlockchain(blockchain);
        freeTransactions(unspent);  // Properly free unspent transactions
        exit(EXIT_FAILURE);
    }

    printf("------MINING BLOCK------\n");
    startTime = (uint64_t)time(NULL);

    newBlock = createBlock(blockchain->length, unspent, blockchain->tail ? blockchain->tail->currHash : NULL, blockchain->difficulty);
    if (!newBlock)
    {
        fprintf(stderr, "Could not create new block\n");
        freeBlockchain(blockchain);
        freeTransactions(unspent);
        exit(EXIT_FAILURE);
    }

    endTime = (uint64_t)time(NULL);
    addBlock(blockchain, newBlock);
    printf("Time taken to mine block: %lu seconds\n", (endTime - startTime));
    printf("\n\n");

    blockchain->difficulty = adjustDifficulty(startTime, endTime, blockchain->difficulty);
    printf("New Difficulty Level: %d\n", blockchain->difficulty);

    printf("\n------VERIFYING BLOCKCHAIN INTERGRITY-------\n");
    if (!validateBlockchain(blockchain))
    {
        fprintf(stderr, "Blockchain is not valid\n");
        freeBlockchain(blockchain);
        freeTransactions(unspent);
        exit(EXIT_FAILURE);
    }

    printf("Blockchain is valid\n");

    if (!serializeBlockchain(blockchain))
    {
        fprintf(stderr, "Blockchain with new block could not be serialized\n");
        freeBlockchain(blockchain);
        freeTransactions(unspent);
        exit(EXIT_FAILURE);
    }

    printf("Serialized new blockchain\n");

    unspent = (list_of_transactions *)malloc(sizeof(list_of_transactions));
    if (!unspent)
    {
        fprintf(stderr, "Memory allocation failed for new unspent transactions\n");
        exit(EXIT_FAILURE);
    }
    unspent->head = unspent->tail = NULL;
    unspent->nb_trans = 0;
    
    if (!serializeUnspent(unspent))
    {
        fprintf(stderr, "Failed to reset unspent transactions\n");
        freeTransactions(unspent);
        exit(EXIT_FAILURE);
    }
    freeTransactions(unspent);

    printf("MINING COMPLETE. NEW BLOCK ADDED TO BLOCKCHAIN\n");
    return 0;
}