#include "blockchain.h"

/**
 * serializeBlockchain - serializes a blockchain to a file
 * blockchain: pointer to blockchain to serialize
 * filename: file to add blockchain to
 */
void serializeBlockchain(Blockchain *blockchain, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for serialization");
        exit(EXIT_FAILURE);
    }

    fwrite(&blockchain->difficulty, sizeof(blockchain->difficulty), 1, file);

    block_t *current = blockchain->head;
    while (current) {
        fwrite(&current->index, sizeof(current->index), 1, file);
        fwrite(&current->timestamp, sizeof(current->timestamp), 1, file);
        fwrite(&current->nonce, sizeof(current->nonce), 1, file);
        fwrite(current->prevHash, SHA256_DIGEST_LENGTH, 1, file);
        fwrite(current->currHash, SHA256_DIGEST_LENGTH, 1, file);

        fwrite(&current->transactions->nb_trans, sizeof(current->transactions->nb_trans), 1, file);

        transaction_t *trans = current->transactions->head;
        while (trans) {
            fwrite(&trans->index, sizeof(trans->index), 1, file);
            fwrite(trans->sender, sizeof(trans->sender), 1, file);
            fwrite(trans->receiver, sizeof(trans->receiver), 1, file);
            fwrite(trans->amount, sizeof(trans->amount), 1, file);
            trans = trans->next;
        }

        current = current->next;
    }

    fclose(file);
}