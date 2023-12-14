CC=gcc
OPT=-Wall -Wextra -std=gnu11 -O2 -Wpedantic
LIB_SERV=-L ./img-dist/ -limg-dist -lm -lpthread
LIB_CLIENT= -lpthread

# Code du serveur (img-search)
DIR_SERV=serveur

# Code du client (pokedex-client)
DIR_CLIENT=client

# Code commun au client & serveur (optionnel)
DIR_COMMON=commun

SRC_SERV= $(wildcard $(DIR_SERV)/*.c)
SRC_CLIENT= $(wildcard $(DIR_CLIENT)/*.c)
SRC_COMMON= $(wildcard $(DIR_COMMON)/*.c)

#source files excluding main.c
OBJS_SERV= $(patsubst $(DIR_SERV)/%.c, %.o, $(SRC_SERV))
OBJS_CLIENT= $(patsubst $(DIR_CLIENT)/%.c, %.o, $(SRC_CLIENT))
OBJS_COMMON= $(patsubst $(DIR_COMMON)/%.c, %.o, $(SRC_COMMON))

#remove main.c from source files
OBJS_SERV := $(filter-out main.o, $(OBJS_SERV))
OBJS_CLIENT := $(filter-out main.o, $(OBJS_CLIENT))


all: img-search pokedex-client

launch: all
	gnome-terminal --tab --title="Serveur" --command=" ./img-search" \
	--tab --title="Client" --command=" ./pokedex-client"

run: all
	./img-search & ./pokedex-client

tests: all
	cd test && ./tests

test_multiple_file_same_time: all
	cd test && ./test_multiple_file_same_time

libimg-dist.a:
	(cd img-dist ; make)

img-search: libimg-dist.a $(DIR_SERV)/main.c $(OBJS_SERV) $(OBJS_COMMON)
	echo $(OBJS_SERV)
	echo $(OBJS_COMMON)
	$(CC) $(OPT)  $(DIR_SERV)/main.c -o img-search $(OBJS_SERV) $(OBJS_COMMON) $(LIB_SERV)

pokedex-client: $(DIR_CLIENT)/main.c $(OBJS_CLIENT) $(OBJS_COMMON) 
	echo $(OBJS_CLIENT)
	echo $(OBJS_COMMON)
	$(CC) $(OPT)  $(DIR_CLIENT)/main.c -o pokedex-client $(OBJS_CLIENT) $(OBJS_COMMON) $(LIB_CLIENT)

%.o: $(DIR_SERV)/%.c $(DIR_SERV)/%.h
	$(CC) $(OPT) $(DBG_OPT) -c $< -o $@

%.o: $(DIR_CLIENT)/%.c $(DIR_CLIENT)/%.h
	$(CC) $(OPT) $(DBG_OPT) -c $< -o $@

%.o: $(DIR_COMMON)/%.c $(DIR_COMMON)/%.h
	$(CC) $(OPT) $(DBG_OPT) -c $< -o $@



.PHONY: clean

clean:
	rm -f *.o
	rm -f img-search
	rm -f pokedex-client
	pkill img-search
