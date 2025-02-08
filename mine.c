#include "blockchain.h"

/**
 * hash_to_hex - converts binary hash to hex string
 * @hash: pointer to hash
 * @output: pointer to buffer to store converted data
 * Return: Nothing
 */
void hash_to_hex(unsigned char *hash, char *output)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[SHA256_DIGEST_LENGTH * 2] = '\0';
}

/**
 * is_valid_hash - checks to see that diffculty requisite is met for the hash
 * @hash: pointer to hash to check validity
 * @difficulty: PoW difficulty level
 * Return: 1 if valid else 0
 */
int is_valid_hash(unsigned char *hash, int difficulty)
{
    for (int i = 0; i < difficulty; i++) {
        if (hash[i] != 0)
            return 0;
    }
    return 1;
}

/**
 * calculateHash - calculates the hash of a block
 * @block: pointer to block to calculate hash of
 * @hash: pointer to address to store hash
 * Return: Nothing
 */
void calculateHash(block_t *block, unsigned int nonce, unsigned char *hash)
{
    transaction_t *current_trans;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create EVP_MD_CTX\n");
        exit(EXIT_FAILURE);
    }

    /* Adding block elements to hash calculation */
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
        EVP_DigestUpdate(ctx, &block->index, sizeof(block->index)) != 1 ||
        EVP_DigestUpdate(ctx, &block->timestamp, sizeof(block->timestamp)) != 1 ||
        EVP_DigestUpdate(ctx, block->prevHash, SHA256_DIGEST_LENGTH) != 1) {
        fprintf(stderr, "Failed to calculate hash\n");
        EVP_MD_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    /* Adding nonce to hash calculation */
    if (EVP_DigestUpdate(ctx, &nonce, sizeof(nonce)) != 1) {
        fprintf(stderr, "Failed to calculate hash with nonce\n");
        EVP_MD_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    /* adding block's transactions to hash*/
    current_trans = block->transactions->head;
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
 * mine_block - mines a block in a blockchain
 * @block: pointer to block to mine
 * @difficulty: PoW difficulty level
 * Return: Nothing
 */
void mine_block(block_t *block, int difficulty) 
{
    unsigned int nonce = 0;
    unsigned char hash[SHA256_DIGEST_LENGTH];

    printf("Mining block %d at difficulty %d...\n", block->index, difficulty);

    do {
        block->nonce = nonce;
        calculateHash(block, nonce, hash);
        nonce++;
    } while (!is_valid_hash(hash, difficulty));

    memcpy(block->currHash, hash, SHA256_DIGEST_LENGTH);

    printf("Block %d mined with nonce: %u\n", block->index, nonce - 1);
}