#include "blockchain.h"

/**
 * main - adds transaction to unspent transaction pool for PoW
 * return: 0 always
 */
int main(void)
{
    char sender[DATASIZE_MAX], receiver[DATASIZE_MAX], amount[20]; /* They are all strings */
    printf("Sender: ");
    if (!fgets(sender, sizeof(sender), stdin))
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    printf("Receiver: ");
    if (!fgets(sender, sizeof(receiver), stdin))
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    printf("Amount: ");
    if (!fgets(sender, sizeof(amount), stdin))
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    /* Removing newlines */
    sender[strcspn(sender, "\n")] = '\0';
    receiver[strcspn(receiver, "\n")] = '\0';
    amount[strcspn(amount, "\n")] = '\0';

    if (!addTransactionToUnspent(sender, receiver, amount))
    {
        fprintf(stderr, "Could not add transactions to unspent pool\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}