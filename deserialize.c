#include "blockchain.h"

/**
 * deserializeBlockchain - deserializes blockchain from a file
 * Return: pointer to blockchain or NULL on failure
 */
Blockchain *deserializeBlockchain(void)
{
    FILE *file = fopen(BLOCKCHAIN_DATABASE, "rb");
    if (!file)
    {
        perror("Failed to open blockchain file, initializing a new blockchain...");
        return initBlockchain();
    }

    Blockchain *blockchain = (Blockchain *)malloc(sizeof(Blockchain));
    if (!blockchain)
    {
        perror("Failed to allocate memory for blockchain");
        fclose(file);
        return NULL;
    }

    blockchain->head = blockchain->tail = NULL;
    blockchain->length = 0;

    if (fread(&blockchain->difficulty, sizeof(blockchain->difficulty), 1, file) != 1)
    {
        perror("Failed to read blockchain difficulty");
        free(blockchain);
        fclose(file);
        return NULL;
    }

    block_t *prevBlock = NULL;
    while (1)
    {
        block_t *block = (block_t *)malloc(sizeof(block_t));
        if (!block)
        {
            perror("Failed to allocate memory for block");
            break;
        }

        if (fread(&block->index, sizeof(block->index), 1, file) != 1)
        {
            free(block);
            break;
        }

        fread(&block->timestamp, sizeof(block->timestamp), 1, file);
        fread(&block->nonce, sizeof(block->nonce), 1, file);
        fread(block->prevHash, SHA256_DIGEST_LENGTH, 1, file);
        fread(block->currHash, SHA256_DIGEST_LENGTH, 1, file);

        list_of_transactions *transactions = (list_of_transactions *)malloc(sizeof(list_of_transactions));
        if (!transactions)
        {
            perror("Failed to allocate memory for transactions");
            free(block);
            break;
        }

        if (fread(&transactions->nb_trans, sizeof(transactions->nb_trans), 1, file) != 1)
        {
            free(transactions);
            free(block);
            break;
        }

        transactions->head = transactions->tail = NULL;

        for (int i = 0; i < transactions->nb_trans; i++)
        {
            transaction_t *trans = (transaction_t *)malloc(sizeof(transaction_t));
            if (!trans)
            {
                perror("Failed to allocate memory for transaction");
                break;
            }

            if (fread(&trans->index, sizeof(trans->index), 1, file) != 1 ||
                fread(trans->sender, sizeof(trans->sender), 1, file) != 1 ||
                fread(trans->receiver, sizeof(trans->receiver), 1, file) != 1 ||
                fread(trans->amount, sizeof(trans->amount), 1, file) != 1)  // Fixed by adding '&'
            {
                free(trans);
                break;
            }

            trans->next = NULL;
            if (transactions->head == NULL)
            {
                transactions->head = transactions->tail = trans;
            }
            else
            {
                transactions->tail->next = trans;
                transactions->tail = trans;
            }
        }

        block->transactions = transactions;
        block->next = NULL;

        if (blockchain->head == NULL)
        {
            blockchain->head = blockchain->tail = block;
        }
        else
        {
            prevBlock->next = block;
            blockchain->tail = block;
        }

        prevBlock = block;
        blockchain->length++;
    }

    fclose(file);
    return blockchain;
}