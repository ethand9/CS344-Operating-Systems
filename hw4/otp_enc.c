#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define MAX_BUFFER 256

int main(int argc, char*argv[]){
    int i, tmpVar1, portNumber, charsWritten, charsRead;
    int plaintextFile, plaintextFileSize, keyFile, keyFileSize;
    int socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    struct sockaddr_in serverAddress, clientAddress;
    struct hostent* serverHostInfo;
    char textBuffer[MAX_BUFFER], tmpBuffer[1], keyBuffer[MAX_BUFFER];

    if(argc < 4){
        perror("error: too few arguments");
        exit(1);
    }

    //convert num to int
    portNumber = atoi(argv[3]);

    //socket() failed
    if(socketFD == -1){
        perror("error: socket() failed");
        exit(1);
    }

    //clear out buffers
    memset(textBuffer, '\0', sizeof(textBuffer));
    memset(tmpBuffer, '\0', sizeof(tmpBuffer));
    memset(keyBuffer, '\0', sizeof(keyBuffer));
    memset((char*)&serverAddress, '\0', sizeof(serverAddress));

    plaintextFile = open(argv[1], O_RDONLY);
    if(plaintextFile < 0){
        perror("error: cannot open file");
        exit(1);
    }
    // plaintextFileSize = lseek(plaintextFile, 0 , SEEK_END);
    plaintextFileSize = read(plaintextFile, textBuffer, MAX_BUFFER);
    //check for bad characters
    close(plaintextFile);

    keyFile = open(argv[2], O_RDONLY);
    if(keyFile < 0){
        perror("error: cannot open file");
        exit(1);
    }
    // keyFileSize = lseek(keyFile, 0, SEEK_END);
    keyFileSize = read(keyFile, keyBuffer, MAX_BUFFER);
    //check for bad characters
    close(keyFile);

    if(plaintextFileSize > keyFileSize){
        perror("error: keyFileSize too small");
        exit(1);
    }


    
    
    //set up server
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    serverHostInfo = gethostbyname("localhost");
    //error connecting to host
    if(serverHostInfo == NULL){
        perror("error: could not resolve server host name");
        exit(2);
    }

    if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress))){
        perror("error: connect() failed");
        exit(2);
    }

    //send plaintext
    charsWritten = write(socketFD, textBuffer, sizeof(textBuffer));
    if(charsWritten < plaintextFileSize){
    // if(charsWritten < strlen(textBuffer)){
        printf("CLIENT: WARNING: Not all data written to socket!\n");
    }

    //ack from server
    charsRead = read(socketFD, tmpBuffer, sizeof(tmpBuffer));
    if(charsRead < 0){
       perror("error: read() failed");
       exit(2);
    }

    //write key to server
    charsWritten = write(socketFD, keyBuffer, sizeof(keyBuffer));
    if(charsWritten < keyFileSize){
        perror("error: write() failed");
        exit(2);
    }

    memset(textBuffer, '\0', sizeof(textBuffer)); //clear buffer

    //receive encryption from server
    // do{
    charsRead = read(socketFD, textBuffer, sizeof(textBuffer));
    // }while(charsRead > 0);
    if(charsRead < 0){
       perror("error: read() failed");
       exit(2);
    }

    //print encrypted message
    for(i = 0; i < plaintextFileSize - 1; i++){
        printf("%c", textBuffer[i]);
    }
    printf("\n");

    close(socketFD);

    return 0;
}
