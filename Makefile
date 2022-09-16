ARGS = -lzmq -Wall -Wextra -Wno-unused -fsanitize=undefined -fsanitize=address -fstack-protector-all -v

server: ftp-server.c
	gcc $< $(ARGS) -o $@

client: ftp-client.c
	gcc $< $(ARGS) -o $@
