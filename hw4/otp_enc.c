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
    char buffer1[MAX_BUFFER], buffer2[1], buffer3[MAX_BUFFER];

    if (argc < 4){
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

    plaintextFile = open(argv[1], O_RDONLY);
    if(plaintextFile < 0){
        perror("error: cannot open file");
        exit(1);
    }
    // plaintextFileSize = lseek(plaintextFile, 0 , SEEK_END);
    plaintextFileSize = read(plaintextFile, buffer1, MAX_BUFFER);
    //check for bad characters
    close(plaintextFile);

    keyFile = open(argv[2], O_RDONLY);
    if(keyFile < 0){
        perror("error: cannot open file");
        exit(1);
    }
    // keyFileSize = lseek(keyFile, 0, SEEK_END);
    keyFileSize = read(keyFile, buffer3, MAX_BUFFER);
    //check for bad characters
    close(keyFile);

    if(plaintextFileSize > keyFileSize){
        perror("error: keyFileSize too small");
        exit(1);
    }


    memset(&serverAddress, '\0', sizeof(serverAddress));
    // memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //clear address struct
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
    // memset(buffer1, '\0', sizeof(buffer1));
    // charsWritten = send(socketFD, buffer1, strlen(buffer1), 0);
    charsWritten = write(socketFD, buffer1, plaintextFileSize - 1);
    if(charsWritten < plaintextFileSize - 1){
    // if (charsWritten < strlen(buffer1)){
        printf("CLIENT: WARNING: Not all data written to socket!\n");
    }

    //ack from server
    memset(buffer2, 0, 1);
    charsRead = read(socketFD, buffer2, 1);
    if (charsRead < 0){
       perror("error: read enc1() failed");
       exit(2);
    }

    // send key
    charsWritten = write(socketFD, buffer3, keyFileSize - 1);
    if(charsWritten < keyFileSize - 1){
        perror("error: write enc1() failed");
        exit(2);
    }

    // memset(buffer1, 0, MAX_BUFFER);
    memset(buffer1, '\0', sizeof(buffer1)); //clear buffer

    // receive ciphertext from otp_enc_d
    // do{
    charsRead = read(socketFD, buffer1, plaintextFileSize - 1);
    // charsRead = read(socketFD, buffer1, plaintextFileSize);
    // }while(charsRead > 0);
    if (charsRead < 0){
       printf("Error receiving ciphertext from otp_enc_d\n");
       exit(2);
    }

    // send key to otp_enc_d
    charsWritten = write(socketFD, buffer2, keyFileSize - 1);
    // charsWritten = write(socketFD, buffer2, keyFileSize);
    if (charsWritten < keyFileSize - 1){
    // if(charsWritten < keyFileSize){
        perror("error: write enc2() failed");
        exit(2);
    }

    // output ciphertext to stdout
    for (i = 0; i < plaintextFileSize - 1; i++){
        printf("%c", buffer1[i]);
    }
    // add newline to ciphertext ouput
    printf("\n");

    // close socket
    close(socketFD);

    return 0;
}
