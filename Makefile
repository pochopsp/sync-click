server:
	gcc -g -pthread -o server src/server/main.c src/common/network_constants.c src/common/string_functions/string_functions.c src/server/network_functions/network_functions.c src/server/client_handling/client_handling.c

client:
	gcc -o client src/client/main.c src/client/left_click.c src/client/string_util.c -lws2_32