

default:
	gcc main.c -o main -Llib -Iinclude -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -Wextra -pedantic -std=c99

debug:
	gcc main.c -o main -Llib -Iinclude -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -Wextra -pedantic -std=c99 -fsanitize=address,undefined -g -O0
