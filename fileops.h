#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void getFullAddress(char *addr, char *port, char **binding){
	/*
	Function to properly format the address string needed for zmq_bind/zmq_connect
	*/
	strcpy(*binding, "tcp://");

	strncat(*binding, addr, strlen(addr));
	strncat(*binding, ":", 2);
	strncat(*binding, port, strlen(port));
	
	return;

}

int getLength(FILE *filePointer){
	/*
	Function to get the length of the file for reading said file
	*/
	int prev = ftell(filePointer);
	fseek(filePointer, SEEK_CUR, SEEK_END);
	int length = ftell(filePointer);
	fseek(filePointer, prev, SEEK_SET); // Reset file pointer for reading 
	return length;
}

int getFileData(const char *filename, char **dataBuffer){
	/*
	Function to read the file contents and store them in the pointer provided by dataBuffer
	*/

	FILE *pfile = NULL;

	pfile = fopen(filename, "rb");
	if (!pfile){
		return -1;
	}

	char fileBuffer[10000]; // 1 MiB buffer for file to send
	char c;
	int index = 0;
	int len = getLength(pfile);
	
	while (index < 10000 && (c = fgetc(pfile)) != EOF){
		// Making sure we don't go over the buffer and adding each character
		fileBuffer[index++] = c;
	}
	fileBuffer[index] = '\0';

	fclose(pfile);

	*dataBuffer = fileBuffer;

	return len;
}

void writeFileData(const char *newFileName, const char *data, const int dataLen){
	/*
	Function to make and write the data to newFileName
	*/

	FILE *pfile = NULL;

	pfile = fopen(newFileName, "wb");
	if (pfile == NULL){
		perror("[ERROR]");
		exit(EXIT_FAILURE);
	}

	fprintf(pfile, "%s", data);

	fclose(pfile);


	return;
}

char *getFileName(char *fullFilepath){
	/*
	Function to properly get the actual file name provided as to not make the file ../../something.txt
	*/

	const char s[20] = "/"; // Wanted to make sure that the program could handle long paths for files
	char *token = strtok(fullFilepath, s);
	char *literalName = NULL;

	while (token != NULL){
		literalName = token;

		token = strtok(NULL, s);
	}

	return literalName;

}