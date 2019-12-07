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
    int plaintextFile, plaintextFileSize, keyFile, keyFileSize, msgMod, keyMod, cipher;
    int socketFD = socket(AF_INET, SOCK_STREAM, 0); //create socket
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo;
    pid_t spawnPid = -5;
    char buffer1[MAX_BUFFER], buffer2[MAX_BUFFER], buffer3[MAX_BUFFER];

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

    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //clear address struct
    //set up server
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // //clear buffer for reuse
    // memset(buffer, '\0', sizeof(buffer));

    // printf("here11\n");

    if(bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
        perror("error: bind() failed");
        exit(2);
    }

    if(listen(socketFD, MAX_QUEUE) < 0){
        perror("error: listen() failed");
        exit(2);
    }

    // establishedConnectionFD = accept(socketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
    // if(establishedConnectionFD < 0){
    //     perror("accept() failed");
    //     exit(1);
    // }

    // printf("here2\n");
    sizeOfClientInfo = sizeof(clientAddress);
    while(1 == 1){
        // printf("here while start\n");
        establishedConnectionFD = accept(socketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
        if(establishedConnectionFD < 0){
            perror("error: accept() failed");
        }
        // printf("here3\n");
        spawnPid = fork();
        // printf("here4\n");
        switch(spawnPid){
            case -1: //error
                perror("error: fork()");
                exit(1);
                break;
            case 0: 
                // printf("here1\n");



                // zero out buffer
                memset(buffer1, 0, sizeof(buffer1));

                // receive plaintext from otp_enc
                // printf("here5\n");
                plaintextFileSize = read(establishedConnectionFD, buffer1, MAX_BUFFER);
                // plaintextFileSize = lseek(plaintextFile, 0 , SEEK_END);
                if (plaintextFileSize < 0){
                    perror("error: read _d1 failed");
                    // printf("Error: otp_end_d could not read plaintext on port %d\n", port);
                    exit(2);
                }

                // printf("here6\n");
                // send acknowledgement to client
                charsWritten = write(establishedConnectionFD, "!", 1);
                if (charsWritten < 0){
                    printf("otp_enc_d error sending acknowledgement to client\n");
                    exit(2);
                }

                // zero out buffer
                memset(buffer2, 0, sizeof(buffer2));

                // printf("here7\n");
                // receive key from otp_enc
                keyFileSize = read(establishedConnectionFD, buffer2, MAX_BUFFER);
                // keyFileSize = lseek(keyFile, 0, SEEK_END);
                if(keyFileSize < 0){
                    perror("error: read _d2 failed");
                    // printf("Error: otp_end_d could not read key on port %d\n", port);
                    exit(2);
                }

                //validate key

                //validate plaintext

                // compare length of plaintext to that of key
                if (keyFileSize < plaintextFileSize){ 
                    printf("otp_enc_d error: key is too short\n");
                    exit(1);
                }


                //encryption
                for (i = 0; i < plaintextFileSize; i++){
                    // change spaces to asterisks
                    if (buffer1[i] == ' '){
                        buffer1[i] = '@';
                    }
                    if (buffer2[i] == ' '){
                        buffer2[i] = '@';
                    }

                    // type conversion to int
                    msgMod = (int)buffer1[i];
                    keyMod = (int)buffer2[i];

                    // subtract 64 so that range is 0 - 26 (27 characters)
                    msgMod = msgMod - 64;
                    keyMod = keyMod - 64;
                    // printf("%s%d%s%d\n", "msgMod: ", msgMod, ". keyMod: ", keyMod);

                    // combine key and message using modular addition
                    cipher = (msgMod + keyMod) % 27;

                    // add 64 back to that range is 64 - 90
                    cipher = cipher + 64;
                    // printf("%s%d\n", "cipher: ", cipher);

                    // type conversion back to char
                    buffer3[i] = (char)cipher + 0;

                    // after encryption, change asterisks to spaces
                    if (buffer3[i] == '@'){
                        buffer3[i] = ' ';
                    }
                }

                //send ciphertext to otp_enc
                charsWritten = write(establishedConnectionFD, buffer3, plaintextFileSize);
                if (charsWritten < plaintextFileSize){
                    printf("otp_enc_d error writing to socket\n");
                    exit(2);
                }

                // printf("here case 0 end\n");
                // exit(0);
                // printf("here100\n");
                break;
            default: 
                // printf("here465\n");
                break;
        }
        // printf("hello world\n");
        // exit(0);
    } // end of while
    exit(0);
}
