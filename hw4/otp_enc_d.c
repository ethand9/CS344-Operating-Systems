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
    int plaintextFile, plaintextFileSize, keyFile, keyFileSize, textLetter, keyLetter, encMsg;
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

    //socket() failed
    if(socketFD == -1){
        perror("error: socket() failed");
        exit(1);
    }

    //convert num to int
    portNumber = atoi(argv[1]);

    //clear out buffers
    memset(textBuffer, '\0', sizeof(textBuffer));
    memset(keyBuffer, '\0', sizeof(keyBuffer));
    memset(encBuffer, '\0', sizeof(encBuffer));
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //clear address struct

    //set up server
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber);

    //bind the socket to the given port
    if(bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
        perror("error: bind() failed");
        exit(2);
    }

    //listen for other connections
    listen(socketFD, MAX_QUEUE);
    while(1 == 1){
        sizeOfClientInfo = sizeof(clientAddress);
        establishedConnectionFD = accept(socketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
        if(establishedConnectionFD == -1){
            perror("error: accept() failed");
        }

        //fork new connections
        spawnPid = fork();
        switch(spawnPid){
            case -1: //error
                perror("error: fork() failed");
                exit(1);
                break;
            case 0: //encryption
                memset(textBuffer, 0, sizeof(textBuffer)); //clear buffer
                
                plaintextFileSize = read(establishedConnectionFD, textBuffer, MAX_BUFFER); //read text from client
                if(plaintextFileSize == -1){ //read() failed
                    perror("error: read() failed");
                    exit(2);
                }
                
                memset(keyBuffer, 0, sizeof(keyBuffer)); //clear buffer
                
                keyFileSize = read(establishedConnectionFD, keyBuffer, MAX_BUFFER); //read keyLetter from client
                if(keyFileSize == -1){ //read() failed
                    perror("error: read() failed");
                    exit(2);
                }

                //encrypt the message
                for(i = 0; i < plaintextFileSize; i++){
                    //if character is a space change it to [
                    //then change value to ascii
                    //subtract 65 to change range to 27 characters
                    if(keyBuffer[i] != ' '){
                        keyLetter = (int)keyBuffer[i];
                        keyLetter -= 65;
                    }
                    else{
                        keyBuffer[i] = '[';
                        keyLetter = (int)keyBuffer[i];
                        keyLetter -= 65;
                    }
                    if(textBuffer[i] != ' '){
                        textLetter = (int)textBuffer[i];
                        textLetter -= 65;
                    }
                    else{
                        textBuffer[i] = '[';
                        textLetter = (int)textBuffer[i];
                        textLetter -= 65;
                    }

                    encMsg = (keyLetter + textLetter) % 27; //modular addition
                    encMsg += 65; //revert range
                    encBuffer[i] = (char)encMsg; //change back to letters

                    if(encBuffer[i] == '[') //change back to space
                        encBuffer[i] = ' ';
                }
                //write encryption to client
                charsWritten = write(establishedConnectionFD, encBuffer, plaintextFileSize);
                break;
            default: //parent
                ; //pass
                break;
        }
    } //end of while
    exit(0);
}
