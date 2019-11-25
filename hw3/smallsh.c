#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>

#define MAX_SIZE 100

void getInput();
void doExit();
void doCD();
void doStatus();
void doExec();
void test1();

//global variables
char userInput[MAX_SIZE];
char* arguments[MAX_SIZE];
char dirName[MAX_SIZE];

int main(){
    // int counter = 0;
    int i, counter, status, comment, inputR, outputR, last, background;
    char* first, outputFile, inputFile;

    while(1 == 1){
        //reset values and clear the arguments
        counter = 0;
        comment = 0;
        inputR = -1;
        outputR = -1;
        background = 0;
        for(i = 0; i < MAX_SIZE; i++)
            arguments[i] == NULL;

        getInput(); //get input from user

        if(strcmp(userInput, "exit\n") == 0){ //check for exit command
            doExit();
        }
        else if(strcmp(userInput, "status\n") == 0){ //check for status command
            doStatus();
        }
        else{
            char* token; 
            token = strtok(userInput, " \n");
            //check to see if input is a comment
            first = token;
            if(*first == '#')
                    comment = 1;
            //get all arguments from user and store it in an array
            while(token != NULL){ 
                // printf("%s\n", token); 
                fflush(stdout);
                arguments[counter] = token;
                // printf("%s%s\n", "arguments: ", arguments[counter]);
                
                // p = token;
                // if(*p == '#'){
                //     comment = 1;
                // }

                // printf("%s%s\n", "p: ", p);

                // for(p = token; *p; ++p){
                //     // printf("here2\n");
                //     // printf("%s\n", *p);
                //     if(*p == '#'){
                //         printf("true!\n");
                //     }
                // }

                token = strtok(NULL, " \n"); 
                counter++;
            }

            if(strcmp(arguments[counter - 1], "&") == 0){
                printf("background!\n");
                background = 1;
            }

            for(i = 0; i < counter; i++){
                // printf("%s%s\n", "arguments: ", arguments[i]);
                if(strcmp(arguments[i], "<") == 0){
                    printf("input!\n");
                    inputR = i + 1;
                }
                else if(strcmp(arguments[i], ">") == 0){
                    printf("output!\n");
                    outputR = i + 1;
                }
            }

            if(comment == 1){
                ; //pass
            }
            else if(strcmp(arguments[0], "cd") == 0){ //check for cd command
                if(arguments[1] == NULL){ //if no given directory
                    chdir(getenv("HOME"));
                    getcwd(dirName, sizeof(dirName));
                    printf("Current working dir: %s\n", dirName);
                    fflush(stdout);
                }
                else if((arguments[2] == NULL) && (!(arguments[1] == NULL))) //if there are only two arguments
                    doCD();
                else{ //more than two arguments
                    printf("error: too many arguments\n");
                    fflush(stdout);
                }
            }
            // else if( (strcmp(arguments[0], "status") == 0) && (arguments[1] == NULL))
            //     doExec();
            else{
                //check for input
                if(inputR != -1){
                    // printf("inside input\n");
                    printf("%s%s\n", "input file: ", arguments[inputR]);
                    // inputFile = arguments[inputR];
                }
                //check for output
                if(outputR != -1){
                    // printf("inside output\n");
                    printf("%s%s\n", "output file: ", arguments[outputR]);
                    // outputFile = arguments[outputR];
                }
                //check for background
                if(background == 1){
                    printf("inside background\n");
                }

                printf("invalid command\n");
                fflush(stdout);
            }

        }

        
    }
}

//get input from the user
void getInput(){ 
    // static char str[100];
    printf("%s", ": ");
    fflush(stdout);
    fgets(userInput, MAX_SIZE, stdin);
    // fgets(str, 100, stdin);
    // return str;
}

//exit
void doExit(){
    // printf("exit\n");
    exit(0);
}

//change to the given directory
void doCD(){
    // printf("inside cd\n");
    chdir(arguments[1]);
    getcwd(dirName, sizeof(dirName));
    printf("Current working dir: %s\n", dirName);
    fflush(stdout);

    // getcwd(dirName, sizeof(dirName));
    // printf("Current working dir: %s\n", dirName);

    // chdir(getenv("HOME"));
    // getcwd(dirName, sizeof(dirName));
    // printf("Current working dir: %s\n", dirName);
}

//status
void doStatus(){
    printf("inside status\n");
}

//exec
void doExec(){
    printf("inside exec\n");
}

void test1(){
    printf("here10\n");

}
