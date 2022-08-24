#include <assert.h>
#include <errno.h>
#include "fileops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

int main(int argc, char *argv[]){
	if (argc < 4){
		printf("No or missing options provided\n");
		printf("Usage: ./server -a address.in.cidr.format -p portnumber\n");
		return -1;
	}	

	// Need to check the port option because the zmq_bind doesn't check if the address you give it is correct
	// Also the fact that the server should only operate after port 1024 as to not interfere with running programs
	long pc = strtol(argv[4], NULL, 10);
	if (pc < 1024 || pc > 65535){
		printf("[ERROR] Invalid port provided\n\tPlease provide a port that is in the range of 1024 - 65535\n");
		return -1;
	}

	char FILEBUFFER[10000] = {'\0'}; // 1 MiB buffer
	char *bindingAddr = (char *)malloc(29 * sizeof(char)); // Maximum length of the string if all values were maxed out
	getFullAddress(argv[2], argv[4], &bindingAddr);

	// Setting up zmq socket
	void *context = zmq_ctx_new();
	void *responder = zmq_socket(context, ZMQ_REP);
	int rc = zmq_bind(responder, bindingAddr);

	free(bindingAddr);
	bindingAddr = NULL;

	if (rc != 0){
    	printf("[ERROR] %s: ", strerror(errno));
    	printf("make sure address to connect to is correct\n");
    	return -1;
    }

	char buffer[1024] = {'\0'};
	zmq_recv(responder, buffer, 1024, 0);

	if (strcmp(buffer, "PUT") == 0){

		char *verbose = "[SERVER] RECIEVED PUT";
		zmq_send(responder, verbose, 22, 0);
		zmq_recv(responder, buffer, 1024, 0);

		char *fileName = buffer;

		zmq_send(responder, "[SERVER] RECV READY", 20, 0); 
		zmq_recv(responder, FILEBUFFER, 10000, 0);

		writeFileData(fileName, FILEBUFFER, strlen(FILEBUFFER));

	}
	else if (strcmp(buffer, "GET") == 0){
		char *verbose = "[SERVER] RECIEVED GET";
		int mlen = strlen(verbose);
		zmq_send(responder, verbose, mlen, 0);

		// Getting the filepath for file to send to client 
		char filePath[1024] = {'\0'};
		zmq_recv(responder, filePath, 1024, 0);

		char *contents = NULL;
		int fileLen = getFileData(filePath, &contents);
		if (fileLen == -1){
			// Sending error that server encounted
			char *message = strerror(errno);
			zmq_send(responder, message, strlen(message), 0);
		}

		zmq_send(responder, contents, fileLen, 0);

	zmq_close(responder);
	zmq_ctx_destroy(context);

	}

	return 0;
}