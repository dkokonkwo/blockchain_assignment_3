CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -pedantic

# linkers
CLINKERS = -lssl -lcrypto

# Binary directory
BIN_DIR = /usr/bin

# Header files
HEADERS = blockchain.h

# Object files
OBJS = blockchain.o serialize.o deserialize.o transactions.o mine.o

# Default target: build all CLI tools
all: create_blockchain add_transaction mine_block print_blockchain

# Compile object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


# create_blockchain CLI command
create_blockchain: create_blockchain.c $(HEADERS)
		$(CC) $(CFLAGS) -o $(BIN_DIR)/create_blockchain create_blockchain.c blockchain.c serialize.c mine.c transactions.c $(CLINKERS)

# add_transaction CLI command
add_transaction: add_transaction.c $(HEADERS)
		$(CC) $(CFLAGS) -o $(BIN_DIR)/add_transaction add_transaction.c blockchain.c transactions.c serialize.c mine.c $(CLINKERS)

# mine_block CLI command
mine_block: mine_block.c $(HEADERS)
		$(CC) $(CFLAGS) -o /usr/bin/mine_block mine_block.c mine.c blockchain.c serialize.c deserialize.c transactions.c $(CLINKERS)

# print_blockchain CLI command
print_blockchain: print_blockchain.c $(HEADERS)
		$(CC) $(CFLAGS) -o /usr/bin/print_blockchain print_blockchain.c blockchain.c deserialize.c serialize.c mine.c transactions.c $(CLINKERS)

# Clean up the build
clean:
	rm -f *.o *.dat $(BIN_DIR)/mine_block $(BIN_DIR)/add_transaction $(BIN_DIR)/create_blockchain $(BIN_DIR)/print_blockchain

# Rebuild everything
rebuild: clean all