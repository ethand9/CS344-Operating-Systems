#include <stdio.h>
#include <stdlib.h>

char* getInput();
void doExit();
void doCD();
void doStatus();
void doExec();

//global variables


int main(){
    // printf("hello world\n");
    // fflush(stdin);
    char* userInput;
    userInput = getInput();
    printf("%s%s\n", "output: ", userInput);
    if(strcmp(userInput, "exit\n") == 0){
        doExit();
    }
    else if(userInput == "cd\n"){
        doCD();
    }
    else if(userInput == "status\n"){
        doStatus();
    }
    else{
        char* token = strtok(userInput, " ");
        while(token != NULL){ 
            printf("%s\n", token); 
            token = strtok(NULL, " "); 
        }
        // if(){

        // }
        // else{
            // doExec();
        // }
    }
}

char* getInput(){
    static char str[100];
    printf("%s", ": ");
    fgets(str, 100, stdin);
    return str;
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
