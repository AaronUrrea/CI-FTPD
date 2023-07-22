//
// Created by aaron on 2/26/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define BLOCK_SIZE 512
#define FILE_SIZE 128
#define MODE_SIZE 10

#define MAX_BUFFER 1024
#define DEFAULT_PORT 69

#ifndef PROGRAM_2_TFTP_H
#define PROGRAM_2_TFTP_H

#define ERROR_1 "\x00\x05\x00\x01\x46\x69\x6c\x65\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64\x2e\x00"
#define ERROR_4 "\x00\x05\x00\x04\x49\x6C\x6C\x65\x67\x61\x6C\x20\x54\x46\x54\x50\x20\x6F\x70\x65\x72\x61\x74\x69\x6F\x6E\x2E\x00"
#define ERROR_6 "\x00\x05\x00\x06\x46\x69\x6C\x65\x20\x61\x6C\x72\x65\x61\x64\x79\x20\x65\x78\x69\x73\x74\x73\x2E\x00"

#define ERROR_1_SIZE 20
#define ERROR_4_SIZE 28
#define ERROR_6_SIZE 25

int sockfd;
pid_t pid;
FILE* file;

void interruptHandler();

void createUDPServer();

void sendData(FILE* file, struct sockaddr_in cliaddr, int dataSocket);

void receiveData(FILE* file, struct sockaddr_in cliaddr, int dataSocket);

void handleError(int error, struct sockaddr_in cliaddr);

void handleRequest(char buffer[MAX_BUFFER], struct sockaddr_in cliaddr);

void listenForConnections();

int main(int argc, char* argv[]);

#endif //PROGRAM_2_TFTP_H