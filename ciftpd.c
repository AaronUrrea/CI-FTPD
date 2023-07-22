//
// Created by Aaron Urrea on 2/26/2023.
//

#include "tftp.h"

void interruptHandler(){
    printf("\n\nInterrupt Signal detected, graceful termination commencing...\n");
    if(file != NULL)
        fclose(file);

    close(sockfd);
    exit(EXIT_FAILURE);
}

void createUDPServer(){
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("SOCKET CREATE ERROR");
        exit(EXIT_FAILURE);
    }

    // Bind to Address
    struct sockaddr_in srvaddr = {};
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvaddr.sin_port = htons(DEFAULT_PORT);

    if (bind(sockfd, (struct sockaddr *) &srvaddr, sizeof(srvaddr)) < 0) {
        perror("SERVER BIND ERROR");
        exit(EXIT_FAILURE);
    }
}

void sendData(FILE* file, struct sockaddr_in cliaddr, int dataSocket){

    char blockToSend[MAX_BUFFER];
    char characterBuffer;

    uint16_t currentBlock = 1;

    int i = 4;

    sendto(dataSocket, "\x00\x04\x00\x00", 4, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));

    for(;characterBuffer != EOF;){
        characterBuffer = (char) fgetc(file);

        if(characterBuffer != EOF){
            blockToSend[i] = characterBuffer;
            i++;
        }

        if(i == BLOCK_SIZE + 4 || characterBuffer == EOF){
            uint16_t opcode = htons(3);
            uint16_t block = htons(currentBlock);

            memcpy(blockToSend, &opcode, 2);
            memcpy(blockToSend + 2, &block, 2);

            // Send block, opcode = 3
            sendto(dataSocket, blockToSend, i, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));

            memset(blockToSend, 0, MAX_BUFFER);

            // Wait for ack, opcode = 4
            recvfrom(dataSocket, (char*) blockToSend, MAX_BUFFER, MSG_WAITALL, (struct sockaddr *) &cliaddr, (socklen_t *) sizeof(cliaddr));

            i = 4;
            currentBlock++;
        }
    }
    printf("Data successfully sent.\n");
}

void receiveData(FILE* file, struct sockaddr_in cliaddr, int dataSocket){

    char blockToReceive[MAX_BUFFER];

    uint16_t currentBlock = 1;

    // Send back ACK packet
    sendto(dataSocket, "\x00\x04\x00\x00", 4, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));

    bool last = false;

    for(;!last; currentBlock++){
        recvfrom(dataSocket, (char*) blockToReceive, MAX_BUFFER, MSG_WAITALL, (struct sockaddr *) &cliaddr, (socklen_t *) sizeof(cliaddr));

        fputs(blockToReceive + 4, file);

        if(strlen(blockToReceive+4) < BLOCK_SIZE)
            last = true;

        uint16_t opcode;
        uint16_t block;

        memset(blockToReceive, 0, MAX_BUFFER);

        opcode = htons(4);
        block = htons(currentBlock);

        memcpy(blockToReceive, &opcode, 2);
        memcpy(blockToReceive + 2, &block, 2);

        sendto(dataSocket, blockToReceive, 4, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));

    }
    printf("Data successfully received.\n");
}

void handleError(int error, struct sockaddr_in cliaddr){
    int errorSocket = socket(AF_INET, SOCK_DGRAM, 0);

    switch(error) {
        case 1:
            printf("Error code 1: File not found.\n");
            sendto(errorSocket, ERROR_1, ERROR_1_SIZE, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
            break;
        case 4:
            printf("Error code 4: Illegal FTFP operation.\n");
            sendto(errorSocket, ERROR_4, ERROR_4_SIZE, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
            break;
        case 6:
            printf("Error code 6: File already exists.\n");
            sendto(errorSocket, ERROR_6, ERROR_6_SIZE, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
            break;
        default:
            break;
    }

    fclose(file);
    close(errorSocket);
}

void handleRequest(char buffer[MAX_BUFFER], struct sockaddr_in cliaddr){

    // We should only be getting an opcode of 1 at this point

    int dataSocket;
    char filename[FILE_SIZE];
    uint16_t opcode;

    memcpy(&opcode, (uint16_t *) buffer, 2);
    opcode = ntohs(opcode);

    strcpy(filename, buffer + 2);

    char* filePath = malloc(strlen("./database/") + strlen(filename) + 1);
    strcpy(filePath, "./database/");
    strcat(filePath, filename);

    printf("Processing Request...\n");
    printf(" opcode: %d\n filename: %s\n mode: netascii\n", opcode, filename);

    if(opcode == 1) {
        if(access(filePath, F_OK) != 0)
            handleError(1, cliaddr); // File not found

        file = fopen(filePath, "r");

        dataSocket = socket(AF_INET, SOCK_DGRAM, 0);

        sendData(file, cliaddr, dataSocket);
    }
    else if(opcode == 2) {
        if(access(filePath, F_OK) == 0)
            handleError(6, cliaddr); // File found

        file = fopen(filePath, "w");

        dataSocket = socket(AF_INET, SOCK_DGRAM, 0);

        receiveData(file, cliaddr, dataSocket);
    }

    else {
        handleError(4, cliaddr); //Invalid opcode
    }

    free(filePath);
    fclose(file);
    close(dataSocket);

    exit(EXIT_SUCCESS);

}

void listenForConnections() {
    struct sockaddr_in cliaddr = {};
    socklen_t clilen = sizeof(cliaddr);

    char buffer[MAX_BUFFER];
    int n;
    printf("Scanning for connections...\n");

    while (true) {
        n = (int) recvfrom(sockfd, buffer, MAX_BUFFER, 0, (struct sockaddr *) &cliaddr, &clilen);

        if (n >= 0) {
            pid = fork();
            if (pid == 0) {
                printf("\nRequest received:\n");
                handleRequest(buffer, cliaddr);
            }
            else if (pid > 0) {
                memset(buffer, 0, MAX_BUFFER);
            }
            else if (pid < 0){
                perror("FORK CREATE ERROR");
                exit(EXIT_FAILURE);
            }

        }
    }
}

int main(int argc, char* argv[]) {
    // Catch CTRL-C
    signal(SIGINT, interruptHandler);

    createUDPServer();

    listenForConnections();
}
