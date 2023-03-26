CC := gcc
CFLAGS :=

SRC := ./src
BIN := ./bin

# TODO mettere unity in altro path e modificare DEPS e target main
DEPS := ./src/process_args.h ./src/unity.h

main: ./bin/process_args.o ./bin/unity.o
	$(CC) -o $@ main.c $^ $(CFLAGS)

$(BIN)/%.o : $(SRC)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)



# TODO vedere perché devo referenziare OS per la prima volta qui per averla popolata dopo
OS := $(OS)
ifeq ($(OS), Windows_NT)
	RM = del /Q
	FixPath = $(subst /,\,$1)
else
	ifeq ($(shell uname), Linux)
		RM = rm -f
		FixPath = $1
	endif
endif

# TODO verificare phony
clean:
	$(RM) $(call FixPath,bin/*)