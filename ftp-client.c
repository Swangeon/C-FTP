#include <assert.h>
#include <errno.h>
#include "fileops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

int main(int argc, char *argv[]){
	
	if (argc < 7){
		printf("No or missing options provided\n");
		printf("Usage: ./client -a address.in.cidr.format -p portnumber --put/get filename.extension\n");
		return -1;
	}

	char *connectAddr = (char *)malloc(29 * sizeof(char)); // 29 is the maximum the string can be with ip and port maxed out
	getFullAddress(argv[2], argv[4], &connectAddr);

	// Setting up zmq socket
	void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    int rc = zmq_connect(requester, connectAddr);

    free(connectAddr);
	connectAddr = NULL;

    if (rc != 0){
    	printf("[ERROR] %s: ", strerror(errno));
    	printf("make sure address to connect to is correct\n");
    	return -1;
    }

	if (strcmp(argv[5], "--put") == 0){
		char buffer[1024] = {'\0'};
		char *option = "PUT";
		int mlen = strlen(option);

		printf("[INFO] Collecting Data from File\n");
		char *contents = NULL;
		int fileLen = getFileData(argv[6], &contents);

		printf("[INFO] Collected file contents\n");

		if (fileLen == -1){
			printf("[ERROR] %s\n", strerror(errno));
			return -1;
		}

		// Making sure the server knows the operation the client wants to do
		zmq_send(requester, option, mlen, 0);
		zmq_recv(requester, buffer, 1024, 0);

		const char *filename = getFileName(argv[6]);

		zmq_send(requester, filename, strlen(filename), 0);

		zmq_recv(requester, buffer, 20, 0);
		zmq_send(requester, contents, fileLen, 0);

		printf("[INFO] Sent Data\n");

	}

	else if(strcmp(argv[5], "--get") == 0){
		char buffer[1024] = {'\0'};
		char FILEBUFFER[10000] = {'\0'}; // 1 MiB buffer
		char *option = "GET";
		int mlen = strlen(option);

		// Making sure the server knows the operation the client wants to do
		zmq_send(requester, option, mlen, 0);
		zmq_recv(requester, buffer, 1024, 0);
		printf("%s\n", buffer);

		// Sending the file that the client wants
		zmq_send(requester, argv[6], strlen(argv[6]), 0);
		zmq_recv(requester, FILEBUFFER, 10000, 0);
		
		// Need to check if the server didn't error out with the request made
		if (strcmp(FILEBUFFER, "No such file or directory") == 0){
			printf("[ERROR] %s\n", FILEBUFFER);
			return -1;
		}
		else if (strcmp(FILEBUFFER, "Permission denied") == 0){
			printf("[ERROR] %s\n", FILEBUFFER);
			return -1;
		}

		printf("[INFO] Collected file contents\n");

		const char *filename = getFileName(argv[6]);
		writeFileData(filename, FILEBUFFER, 10000);
	}

	else{
		printf("Incorrect options provided\n");
		printf("Usage: ./client -a address.in.cidr.format -p portnumber --put/get filename.extension\n");
		return -1;
	}	

	return 0;
}

