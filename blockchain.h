#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#define DATASIZE_MAX 1024
#define FILENAME "/usr/bin/blockchain.dat"
#define INITIAL_DIFFICULTY 2  /* Starting difficulty level */

typedef struct transaction_s {
    int index;
    char sender[DATASIZE_MAX];
    char receiver[DATASIZE_MAX];
    char amount[512];
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

#endif /* blockchain.h */