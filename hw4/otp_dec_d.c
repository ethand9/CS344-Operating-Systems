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
    int cipherFile, cipherFileSize, keyFile, keyFileSize, textLetter, keyLetter, decMsg;
    int socketFD = socket(AF_INET, SOCK_STREAM, 0); //create socket
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo;
    pid_t spawnPid = -5;
    char textBuffer[MAX_BUFFER], keyBuffer[MAX_BUFFER], decBuffer[MAX_BUFFER];

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
    memset(decBuffer, '\0', sizeof(decBuffer));
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
                
                cipherFileSize = read(establishedConnectionFD, textBuffer, MAX_BUFFER); //read text from client
                if(cipherFileSize == -1){ //read() failed
                    perror("error: read() failed");
                    exit(2);
                }
                
                memset(keyBuffer, 0, sizeof(keyBuffer)); //clear buffer
                
                keyFileSize = read(establishedConnectionFD, keyBuffer, MAX_BUFFER); //read keyLetter from client
                if(keyFileSize == -1){ //read() failed
                    perror("error: read() failed");
                    exit(2);
                }

                //decrypt the message
                for(i = 0; i < cipherFileSize; i++){
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

                    //modular subtraction
                    if((textLetter - keyLetter) < 0){ //if outside of range
                        decMsg = (textLetter - keyLetter) + 27; //loop around
                    }
                    else{
                        decMsg = (textLetter - keyLetter);
                    }

                    decMsg += 65; //revert range
                    decBuffer[i] = (char)decMsg; //change back to letters

                    if(decBuffer[i] == '[') //change back to space
                        decBuffer[i] = ' ';
                }
                //write decryption to client
                charsWritten = write(establishedConnectionFD, decBuffer, cipherFileSize);
                break;
            default: //parent
                ; //pass
                break;
        }
    } //end of while
    exit(0);
}
