CC = gcc
#For Optimization
#CFLAGS = -O2
#For debugging
CFLAGS = -g

RM= rm -f

BIN= ./
INCLUDE = -I./

.c.o: 
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

all:	index search

index:	index.o 
	$(CC) -o $(BIN)$@ index.o

search: search.o trie.o
	$(CC) -o $(BIN)$@ search.o trie.o

clean: 	rmobj rmbin

rmobj:
	${RM} *.o 
rmbin:
	${RM} $(BIN)index
	${RM} $(BIN)search
