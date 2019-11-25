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
    // char userInput[MAX_SIZE];
    // char* arguments[MAX_SIZE];
    int counter = 0;
    int i;
    char* p;

    // printf("hello world\n");
    // fflush(stdin);
    // char* userInput;
    // userInput = getInput();

    getInput();
    // printf("%s%s\n", "output: ", userInput);
    // fflush(stdout);
    // printf(userInput[1]);


    // test1();

    // printf("%s", ": ");
    // fflush(stdout);
    // fgets(userInput, MAX_SIZE, stdin);


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
        token = strtok(userInput, " \n");
        while(token != NULL){ 
            // printf("%s\n", token); 
            fflush(stdout);
            arguments[counter] = token;
            // printf("%s%s\n", "args: ", arguments[counter]);
            
            // if(strcmp(token, "cd") == 0){
            //     doCD();
            // }

            

            // printf("here1\n");
            
            p = token;
            if(*p == '#'){
                printf("true!\n");
            }
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
        if(strcmp(arguments[0], "cd") == 0){
            // printf("true!\n");
            if(arguments[1] == NULL){
                // printf("none!\n");
                chdir(getenv("HOME"));
                getcwd(dirName, sizeof(dirName));
                printf("Current working dir: %s\n", dirName);
            }
            else if( (arguments[2] == NULL) && (!(arguments[1] == NULL)) ){
                doCD();
            }
            else{
                printf("error\n");
            }
        }

        // if(){

        // }
        // else{
            // doExec();
        // }
    }

    // for(i = 0; i < counter; i++){
    //     printf("%s%s\n", "arguments: ", arguments[i]);
    // }
}

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

//cd
void doCD(){
    
    // printf("inside cd\n");
    chdir(arguments[1]);
    getcwd(dirName, sizeof(dirName));
    printf("Current working dir: %s\n", dirName);

    // getcwd(dirName, sizeof(dirName));
    // printf("Current working dir: %s\n", dirName);

    // chdir(getenv("HOME"));
    // getcwd(dirName, sizeof(dirName));
    // printf("Current working dir: %s\n", dirName);
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
    printf("here10\n");

}
