#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define MAX_QUEUE 5
#define MAX_BUFFER 256

int main(int argc, char*argv[]){
    int i, portNumber, establishedConnectionFD, listenSocketFD, charsWritten, charsRead;
    int plaintextFile, plaintextFileSize, keyFile, keyFileSize, msgLetter, keyLetter, cipher;
    int socketFD = socket(AF_INET, SOCK_STREAM, 0); //create socket
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo;
    pid_t spawnPid = -5;
    char textBuffer[MAX_BUFFER], keyBuffer[MAX_BUFFER], encBuffer[MAX_BUFFER];

    //too few arguments
    if(argc < 2){
        perror("error: too few arguments");
        exit(1);
    }

    //convert num to int
    portNumber = atoi(argv[1]);

    //socket() failed
    if(socketFD == -1){
        perror("error: socket() failed");
        exit(1);
    }

    //clear out buffers
    memset(textBuffer, '\0', sizeof(textBuffer));
    memset(keyBuffer, '\0', sizeof(keyBuffer));
    memset(encBuffer, '\0', sizeof(encBuffer));
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //clear address struct

    //set up server
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
        perror("error: bind() failed");
        exit(2);
    }

    if(listen(socketFD, MAX_QUEUE) == -1){
        perror("error: listen() failed");
        exit(2);
    }

    sizeOfClientInfo = sizeof(clientAddress);
    while(1 == 1){
        establishedConnectionFD = accept(socketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
        if(establishedConnectionFD == -1){
            perror("error: accept() failed");
        }

        spawnPid = fork();
        switch(spawnPid){
            case -1: //error
                perror("error: fork() failed");
                exit(1);
                break;
            case 0: //encryption
                memset(textBuffer, 0, sizeof(textBuffer)); //clear buffer

                //read text from client
                plaintextFileSize = read(establishedConnectionFD, textBuffer, MAX_BUFFER);
                // plaintextFileSize = lseek(plaintextFile, 0 , SEEK_END);
                //read() failed
                if(plaintextFileSize == -1){
                    perror("error: read() failed");
                    exit(2);
                }

                // //write ack to client
                // charsWritten = write(establishedConnectionFD, "!", 1);
                // if(charsWritten == -1){
                //     perror("error: write() failed");
                //     exit(2);
                // }

                //clear buffer
                memset(keyBuffer, 0, sizeof(keyBuffer));

                //read key from client
                keyFileSize = read(establishedConnectionFD, keyBuffer, MAX_BUFFER);
                // keyFileSize = lseek(keyFile, 0, SEEK_END);
                //read() failed
                if(keyFileSize == -1){
                    perror("error: read() failed");
                    exit(2);
                }

                //validate key

                //validate plaintext

                //encryption
                for(i = 0; i < plaintextFileSize; i++){
                    //if character is a space change it to [
                    //then change value to ascii
                    if(keyBuffer[i] != ' ')
                        keyLetter = (int)keyBuffer[i];
                    else{
                        keyBuffer[i] = '[';
                        keyLetter = (int)keyBuffer[i];
                    }
                    if(textBuffer[i] != ' ')
                        msgLetter = (int)textBuffer[i];
                    else{
                        textBuffer[i] = '[';
                        msgLetter = (int)textBuffer[i];
                    }
                    //subtract 65 to change range to 27 characters
                    keyLetter -= 65;
                    msgLetter -= 65;
    
                    cipher = (keyLetter + msgLetter) % 27; //modular addition
                    cipher += 65; //revert range
                    encBuffer[i] = (char)cipher; //change back to letters

                    if(encBuffer[i] == '[')
                        encBuffer[i] = ' ';

                    // // change spaces to asterisks
                    // if(textBuffer[i] == ' '){
                    //     textBuffer[i] = '@';
                    // }
                    // if(keyBuffer[i] == ' '){
                    //     keyBuffer[i] = '@';
                    // }

                    // // type conversion to int
                    // msgLetter = (int)buffer1[i];
                    // keyLetter = (int)buffer2[i];

                    // // subtract 64 so that range is 0 - 26 (27 characters)
                    // msgLetter = msgLetter - 64;
                    // keyLetter = keyLetter - 64;

                    // // combine key and message using modular addition
                    // cipher = (msgLetter + keyLetter) % 27;

                    // // add 64 back to that range is 64 - 90
                    // cipher = cipher + 64;

                    // // type conversion back to char
                    // encBuffer[i] = (char)cipher + 0;

                    // // after encryption, change asterisks to spaces
                    // if(encBuffer[i] == '@'){
                    //     encBuffer[i] = ' ';
                    // }
                }

                //write encryption to client
                charsWritten = write(establishedConnectionFD, encBuffer, plaintextFileSize);
                //not all characters sent
                if(charsWritten < plaintextFileSize){
                    perror("error: write() failed");
                    exit(2);
                }

                break;
            default: //parent
                ; //pass
                break;
        }
    } //end of while
    exit(0);
}
