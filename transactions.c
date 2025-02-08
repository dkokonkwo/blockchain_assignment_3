#include "blockchain.h"


/**
 * deserialize_unspent_transaction - get unpsent transactions from pool
 * Return: pointer to list of unpsent transactions else exit
 */
list_of_transactions *deserialize_unspent_transactions(void)
{
    FILE *file = fopen(TRANSACTION_DATABASE, "rb");
    if (!file) {
        perror("Failed to open file for deserialization of unpsent transactions");
        exit(EXIT_FAILURE);
    }

    list_of_transactions *unspent_transactions = (list_of_transactions *)malloc(sizeof(list_of_transactions));
    if (!unspent_transactions) {
        perror("Failed to allocate memory for unspent transactions");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    
    unspent_transactions->head = unspent_transactions->tail = NULL;
    unspent_transactions->nb_trans = 0;

    while (!feof(file))
    {
        transaction_t *transaction = (transaction_t *)malloc(sizeof(transaction_t));
        if (!transaction)
        {
            perror("Failed to allocate memory for transaction");
            fclose(file);
            free(unspent_transactions);
            exit(EXIT_FAILURE);
        }

        if (fread(&transaction->index, sizeof(transaction->index), 1, file) != 1) {
            free(transaction);
            break;
        }
        fread(transaction->sender, sizeof(transaction->sender), 1, file);
        fread(transaction->receiver, sizeof(transaction->sender), 1, file);
        fread(transaction->amount, sizeof(transaction->sender), 1, file);

        transaction->next = NULL;
        if (unspent_transactions->head == NULL) {
            unspent_transactions->head = unspent_transactions->tail = transaction;
        } else {
            unspent_transactions->tail->next = transaction;
            unspent_transactions->tail = transaction;
        }

        unspent_transactions->nb_trans++;
    }

    fclose(file);
    return unspent_transactions;
}

/**
 * addTransaction - adds transaction to unspent transactions pool(file)
 * @sender: sender details
 * @receiver: receiver details
 * @amount: amount of transaction
 * Return: 1 on success or 0 on failure
 */
int addTransaction(const char *sender, const char *receiver, const char *amount)
{
    

    transaction_t *new_trans = (transaction_t *)malloc(sizeof(transaction_t));
     if (!new_trans)
    {
        printf("Failed to allocate memory for new transaction");
        return 0;
    }
    strncpy(new_trans->sender, sender, DATASIZE_MAX - 1);
    new_trans->index = unspent_transactions->nb_trans++;
    new_trans->sender[DATASIZE_MAX - 1] = '\0';
    strncpy(new_trans->receiver, receiver, DATASIZE_MAX - 1);
    new_trans->receiver[DATASIZE_MAX - 1] = '\0';
    strncpy(new_trans->amount, amount, sizeof(new_trans->amount) - 1);
    new_trans->amount[sizeof(new_trans->amount) - 1] = '\0';

    new_trans->next = NULL;
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