#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>

#define MAX_ARRAY_SIZE 100

void getInput();
void doExit();
void doCD();
void doStatus();
void doExec();
void test1();

//global variables
// char userInput[MAX_ARRAY_SIZE];
// char* arguments[MAX_ARRAY_SIZE];

int main(){
    char userInput[MAX_ARRAY_SIZE];
    char* arguments[MAX_ARRAY_SIZE];
    int counter = 0;
    int i;

    // printf("hello world\n");
    // fflush(stdin);
    // char* userInput;
    // userInput = getInput();

    // getInput();
    // printf("%s%s\n", "output: ", userInput);
    // fflush(stdout);


    // test1();

    printf("%s", ": ");
    fflush(stdout);
    fgets(userInput, MAX_ARRAY_SIZE, stdin);


    // arguments[0] = "hello";
    // printf("%s\n", arguments[0]);


    if(strcmp(userInput, "exit\n") == 0){
        doExit();
    }
    // else if(userInput == "cd\n"){
    //     doCD();
    // }
    else if(strcmp(userInput, "status\n") == 0){
        doStatus();
    }
    else{
        char* token; 
        token = strtok(userInput, " ");
        while(token != NULL){ 
            printf("%s\n", token); 
            fflush(stdout);
            // arguments[counter] = token;
            
            if(strcmp(token, "cd") == 0){
                doCD();
            }

            token = strtok(NULL, " \n"); 
            counter++;
        }

        // if(){

        // }
        // else{
            doExec();
        // }
    }

    // for(i = 0; i < counter; i++){
    //     printf("%s%s\n", "arguments: ", arguments[i]);
    // }
}

void getInput(){
    // static char str[100];
    // printf("%s", ": ");
    // fflush(stdout);
    // fgets(userInput, MAX_ARRAY_SIZE, stdin);
    // fgets(str, 100, stdin);
    // return str;
}

//exit
void doExit(){
    // printf("exit\n");
    exit(0);
}

//cd
void doCD(){
    printf("cd\n");
}

//status
void doStatus(){
    printf("status\n");
}

//exec
void doExec(){
    printf("exec\n");
}

void test1(){
    printf("here1\n");
}
