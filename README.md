# C-FTP Server and Client
- Author: Sean Brady
- Date: 08/24/2022

## Features
- Using [zeromq](https://zeromq.org/) easy and reliable socket communication
- Both uploading and downloading file operations 

## How to Build
1. Make sure that you have the latest zeromq C api installed using `sudo apt install libzmq3-dev`
2. Next, use the `make` to compile the source code

## How to Use
- For the server you will only need to supply the address and the port number you wish to bind the server to
	- Ex: `./server -a 127.0.0.1 -p 9999` -> Make sure you put the address and port in that order
- For the client you will supply the server address and port, option to put or get, the file path (can be relative or absolute)
	- Ex: `./client -a 127.0.0.1 -p 9999 --put/get file.txt` -> Make sure you put the options in that order