WINDOWS =
ifeq ($(OS),Windows_NT)
   WINDOWS = yes
endif

CC=gcc
CFLAGS=
SER_LIBS=-lpthread
CLI_LIBS=-lws2_32

SER_SRC=src/server
CLI_SRC=src/client
CMN_SRC=src/common

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

server: $(SER_SRC)/main.o $(CMN_SRC)/network_constants.o $(CMN_SRC)/string_functions/string_functions.o $(SER_SRC)/network_functions/network_functions.o $(SER_SRC)/client_handling/client_handling.o
	$(CC) $(CFLAGS) -o server $(SER_SRC)/main.o $(CMN_SRC)/network_constants.o $(CMN_SRC)/string_functions/string_functions.o $(SER_SRC)/network_functions/network_functions.o $(SER_SRC)/client_handling/client_handling.o $(SER_LIBS)

client:
	$(CC) $(CFLAGS) -o client $(CLI_SRC)/main.c $(CLI_SRC)/left_click.c $(CLI_SRC)/string_util.c $(CLI_SRC)/usleep.c src/common/string_functions/string_functions.c src/common/network_constants.c $(CLI_LIBS)

.PHONY: clean

clean:
ifdef WINDOWS
	del server client.exe
	powershell -noprofile -command "ls *.o -Recurse | foreach {rm $$_}"
else
	rm -f server client.exe
	find . -type f -name '*.o' -delete
endif