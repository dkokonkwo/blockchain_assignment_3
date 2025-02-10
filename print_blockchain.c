#include "blockchain.h"

/**
 * main - prints blockchain
 * Return: 0 always
 */
int main(void)
{
    Blockchain *blockchain = deserializeBlockchain();
    if (!blockchain)
    {
        fprintf(stderr, "Could not get blockchain from file\n");
        exit(EXIT_FAILURE);
    }
    if (!blockchain->head)
    {
        printf("Blockchain is empty\n");
        return;
    }
    printBlockchain(blockchain);
    freeBlockchain(blockchain);
    return 0;
}