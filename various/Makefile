# paths
INCLUDE = ../include
MODULES = ../modules
#SOURCE = ../../programs/useful_make
#ARGS = 32

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -g -I$(INCLUDE)
LDFLAGS = -lm

SET_SG_PATH = $(MODULES)/ADTSet/UsingScapegoat_Path/ADTSet.o
VECTOR_DYN_ARRAY = $(MODULES)/ADTVector/ADTVector.o


# Αρχεία .o
OBJS = ADTSet_benchmark_test.o $(SET_SG_PATH) $(VECTOR_DYN_ARRAY)

# Το εκτελέσιμο πρόγραμμα
EXEC = ADTSet_benchmark_test

# Παράμετροι για δοκιμαστική εκτέλεση

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS) 

valgrind: $(EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)
