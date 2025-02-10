#include "blockchain.h"
/**
 * serializeUnspent - serialize unspent transactions to a file
 * @unspent: pointer to list of unspent transactions
 * Return: 1 on sucess else 0 on failure
 */
int serializeUnspent(list_of_transactions *unspent)
{
    FILE *file = fopen(TRANSACTION_DATABASE, "wb");
    if (!file)
    {
        printf("Failed to open file for serialization\n");
        fclose(file);
        return 0;
    }
    transaction_t *current = unspent->head;
    while (current)
    {
        fwrite(&current->index, sizeof(current->index), 1, file);
        fwrite(current->sender, sizeof(current->sender), 1, file);
        fwrite(current->receiver, sizeof(current->receiver), 1, file);
        fwrite(current->amount, sizeof(current->amount), 1, file);
        current = current->next;
    }
    fclose(file);
    return 1;
}

/**
 * deserializeUnspent - get unpsent transactions from pool(file)
 * Return: pointer to list of unpsent transactions else exit
 */
list_of_transactions *deserializeUnspent(void)
{
    FILE *file = fopen(TRANSACTION_DATABASE, "rb");
    if (!file) {
        perror("Failed to open file for deserialization of unpsent transactions");
        return NULL;
    }

    list_of_transactions *unspent_transactions = (list_of_transactions *)malloc(sizeof(list_of_transactions));
    if (!unspent_transactions) {
        perror("Failed to allocate memory for unspent transactions");
        fclose(file);
        return NULL;
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
            return NULL;
        }

        if (fread(&transaction->index, sizeof(transaction->index), 1, file) != 1) {
            free(transaction);
            break;
        }
        fread(transaction->sender, sizeof(transaction->sender), 1, file);
        fread(transaction->receiver, sizeof(transaction->receiver), 1, file);
        fread(transaction->amount, sizeof(transaction->amount), 1, file);

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
 * addTransactionToUnspent - adds transaction to unspent transactions pool(file)
 * @sender: sender details
 * @receiver: receiver details
 * @amount: amount of transaction
 * Return: 1 on success or 0 on failure
 */
int addTransactionToUnspent(const char *sender, const char *receiver, const char *amount)
{
    list_of_transactions *unspent;
    transaction_t *new_trans;
    if (!sender || !receiver || !amount)
    {
        fprintf(stderr, "Wrong details\n");
        return 0;
    }

    unspent = deserializeUnspent();
    if (!unspent)
    {
        fprintf(stderr, "Error deserializing unspent transactions\n");
        return 0;
    }

    new_trans = (transaction_t *)malloc(sizeof(transaction_t));
     if (!new_trans)
    {
        fprintf(stderr, "Failed to allocate memory for new transaction\n");
        free(unspent);
        return 0;
    }
    new_trans->index = unspent->nb_trans;
    unspent->nb_trans++;

    strncpy(new_trans->sender, sender, DATASIZE_MAX - 1);
    new_trans->sender[DATASIZE_MAX - 1] = '\0';
    strncpy(new_trans->receiver, receiver, DATASIZE_MAX - 1);
    new_trans->receiver[DATASIZE_MAX - 1] = '\0';
    strncpy(new_trans->amount, amount, sizeof(new_trans->amount) - 1);
    new_trans->amount[sizeof(new_trans->amount) - 1] = '\0';
    new_trans->next = NULL;

    if (!unspent->head)
        unspent->head = unspent->tail = new_trans;
    else
    {
        unspent->tail->next = new_trans;
        unspent->tail = new_trans;
    }

    if (!serializeUnspent(unspent))
    {
        fprintf(stderr, "Could not serialize unspent with new transaction\n");
        free(new_trans);
        free(unspent);
        return 0;
    }

    printf("Transaction saved!\n");
    free(new_trans);
    free(unspent);
    return 1;
}

/**
 * freeTransactions - frees list of transactions
 * transactions: pointer to list of transactions
 */
void freeTransactions(list_of_transactions *transactions)
{
    transaction_t *current = transactions->head;
    while (current)
    {
        transaction_t *next = current->next;
        free(current);
        current = next;
    }
    free(transactions);
}