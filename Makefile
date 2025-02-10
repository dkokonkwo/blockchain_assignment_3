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
create_blockchain: create_blockchain.o $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/create_blockchain create_blockchain.o $(OBJS) $(CLINKERS)

# add_transaction CLI command
add_transaction: add_transaction.o $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/add_transaction add_transaction.o $(OBJS) $(CLINKERS)

# mine_block CLI command
mine_block: mine_block.o $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/mine_block mine_block.o $(OBJS) $(CLINKERS)

# print_blockchain CLI command
print_blockchain: print_blockchain.o $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/print_blockchain print_blockchain.o $(OBJS) $(CLINKERS)

# Clean up the build
clean:
	rm -f *.o $(BIN_DIR)/mine_block $(BIN_DIR)/add_transaction $(BIN_DIR)/create_blockchain $(BIN_DIR)/print_blockchain

# Rebuild everything
rebuild: clean all