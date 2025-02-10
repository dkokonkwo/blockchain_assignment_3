#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#define DATASIZE_MAX 1024
#define BLOCKCHAIN_DATABASE "blockchain.dat"
#define TRANSACTION_DATABASE "transaction.dat"
#define INITIAL_DIFFICULTY 1  /* Starting difficulty level */

typedef struct transaction_s {
    int index;
    char sender[DATASIZE_MAX];
    char receiver[DATASIZE_MAX];
    char amount[20];
    struct transaction_s *next;
} transaction_t;

typedef struct list_of_transactions {
    transaction_t *head;
    transaction_t *tail;
    int nb_trans;
} list_of_transactions;

typedef struct block_s {
    int index;
    int nonce;
    uint64_t timestamp;
    list_of_transactions *transactions;
    unsigned char prevHash[SHA256_DIGEST_LENGTH];
    unsigned char currHash[SHA256_DIGEST_LENGTH];
    struct block_s *next;
} block_t;

typedef struct Blockchain {
    block_t *head;
    block_t *tail;
    int length;
    int difficulty;
} Blockchain;


/* TRANSACTION FUNCTIONS */
int serializeUnspent(list_of_transactions *unspent);
list_of_transactions *deserializeUnspent(void);
int addTransactionToUnspent(const char *sender, const char *receiver, const char *amount);
void freeTransactions(list_of_transactions *transactions);

/* BLOCK MINING FUNCTIONS */
void mine_block(block_t *block, int difficulty);
void calculateHash(block_t *block, unsigned int nonce, unsigned char *hash);
int is_valid_hash(unsigned char *hash, int difficulty);
void hash_to_hex(unsigned char *hash, char *output);

/* BLOCKCHAIN FUNCTIONS */
Blockchain *deserializeBlockchain(void);
int serializeBlockchain(Blockchain *blockchain);
Blockchain *initBlockchain(void);
int validateBlockchain(Blockchain *blockchain);
void printBlockchain(Blockchain *blockchain);
void freeBlockchain(Blockchain *blockchain);
list_of_transactions *createTransactions(const char *sender, const char *receiver, const char *amount);
int adjustDifficulty(uint64_t prevTime, uint64_t currentTime, int currentDifficulty);

/* BLOCK FUNCTIONS */
block_t *createBlock(int index, list_of_transactions *transactions, const unsigned char *prevHash, int difficulty);
void addBlock(Blockchain *blockchain, block_t *block);

#endif /* blockchain.h */