CC=gcc
CFLAGS=-I.
DEPS = types.h trie.h payload.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server.o trie.o payload.o
	$(CC) -o server server.o trie.o payload.o 

client: client.o payload.o
	$(CC) -o client client.o payload.o 