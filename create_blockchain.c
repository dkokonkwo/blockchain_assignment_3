#include "blockchain.h"


/**
 * main - initializes blockchain from scratch
 * Return: 0 always
 */
int main()
{
    Blockchain *blockchain = initBlockchain();
    if (!blockchain)
    {
        fprintf(stderr, "Could not initialize blockchain\n");
        exit(EXIT_FAILURE);
    }
    if (!serializeBlockchain(blockchain))
    {
        fprintf(stderr, "Could not serialize created blockchain\n");
        fflush(stdout);
        freeBlockchain(blockchain);
        exit(EXIT_FAILURE);
    }

    printf("Blockchain created!\n");
    fflush(stdout);
    return 0;
}