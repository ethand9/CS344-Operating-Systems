#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT 100

void getInput();
void doExit();
void doCD();
void doStatus();
void doExec();

char userInput[MAX_INPUT];

int main(){
    // printf("hello world\n");
    // fflush(stdin);
    // char* userInput;
    // userInput = getInput();
    getInput();
    printf("%s%s\n", "output: ", userInput);
    if(strcmp(userInput, "exit\n") == 0){
        doExit();
    }
    else if(strcmp(userInput, "status\n") == 0){
        doStatus();
    }
    else{
        //check for cd
        
        if(userInput[0] == "c"){
            printf("true!\n");
        }
        else{
            doExec();
        }
    }
}

void getInput(){
    // static char str[100];
    printf("%s", ": ");
    fgets(userInput, MAX_INPUT, stdin);
    // return str;
}

//exit
void doExit(){
    printf("exit\n");
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
