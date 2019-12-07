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
    int socketFD = socket(AF_INET, SOCK_STREAM, 0); //create the socket
    struct sockaddr_in serverAddress, clientAddress;
    struct hostent* serverHostInfo = gethostbyname("localhost"); //set the host
    char textBuffer[MAX_BUFFER], tmpBuffer[1], keyBuffer[MAX_BUFFER];

    //less than four arguments
    if(argc < 4){
        perror("error: too few arguments");
        exit(1);
    }

    //socket() failed
    if(socketFD == -1){
        perror("error: socket() failed");
        exit(1);
    }

    //error connecting to host
    if(serverHostInfo == NULL){
        perror("error: could not resolve server host name");
        exit(2);
    }

    //convert num to int
    portNumber = atoi(argv[3]);

    //clear out buffers
    memset(textBuffer, '\0', sizeof(textBuffer));
    memset(tmpBuffer, '\0', sizeof(tmpBuffer));
    memset(keyBuffer, '\0', sizeof(keyBuffer));
    memset((char*)&serverAddress, '\0', sizeof(serverAddress));

    //set up server
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber);
    
    //connection failed
    if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress))){
        perror("error: connect() failed");
        exit(2);
    }

    //get text file
    plaintextFile = open(argv[1], O_RDONLY); 
    if(plaintextFile == -1){ //invalid file
        perror("error: cannot open file");
        exit(1);
    }
    plaintextFileSize = read(plaintextFile, textBuffer, MAX_BUFFER); //read the text file

    //check for bad characters
    for(i = 0; i < plaintextFileSize - 1; i++){
        //check for ascii values out of capital range and not a space
        if((((int)textBuffer[i] < 65) && (textBuffer[i] != ' ')) || ((int)textBuffer[i] > 90)){
            perror("error: input has bad characters");
            exit(1);
        }
    }
    close(plaintextFile);

    //get key file
    keyFile = open(argv[2], O_RDONLY); 
    if(keyFile == -1){ //invalid file
        perror("error: cannot open file");
        exit(1);
    }
    keyFileSize = read(keyFile, keyBuffer, MAX_BUFFER); //read the key

    //check for valid key size
    if(plaintextFileSize > keyFileSize){
        perror("error: keyFileSize too small");
        exit(1);
    }
    close(keyFile);

    charsWritten = write(socketFD, textBuffer, sizeof(textBuffer)); //write text to server
    charsWritten = write(socketFD, keyBuffer, sizeof(keyBuffer)); //write key to server

    //receive encryption from server
    memset(textBuffer, '\0', sizeof(textBuffer)); //clear buffer
    charsRead = read(socketFD, textBuffer, sizeof(textBuffer));
    if(charsRead == -1){ //read() failed
       perror("error: read() failed");
       exit(2);
    }

    //print encrypted message
    for(i = 0; i < plaintextFileSize - 1; i++){
        printf("%c", textBuffer[i]);
    }
    printf("\n");

    exit(0);
}
