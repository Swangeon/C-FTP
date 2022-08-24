LIBS = -lzmq -Wall -Wextra -Wno-unused -fsanitize=undefined -fsanitize=address -fstack-protector-all -v

all: server client

server: ftp-server.c
	gcc $< $(LIBS) -o $@

client: ftp-client.c
	gcc $< $(LIBS) -o $@