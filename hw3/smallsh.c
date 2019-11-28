#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_INPUT 2048
#define MAX_ARGS 512
#define MAX_SIZE 100

void getInput();
void doExit();
void doCD();
void doStatus(int termSig, int statusNum);
void ctrlC(int signalNum1);
void ctrlZ(int signalNum2);

void catchSIGINT(int signo);
void catchSigStop(int signal);

//global variables
char userInput[MAX_INPUT];
char* arguments[MAX_ARGS];
char dirName[MAX_INPUT];
char tmpPid[MAX_SIZE];
int fgMode = 0;

int main(){
    int i, j, counter, counter2, comment, inputR, outputR, last, backgroundAction, 
    cpid, tmpvar1, tmpvar2, ioNum, iNum, oNum, dupNum, length, bgPid, background;
    char *first, *tmpChar, *tmpChar2, *p, *inputFile, *outputFile;
    struct sigaction cAction = {0};
    struct sigaction zAction = {0};
    pid_t spawnPid = -5;
    pid_t actualPid = -5;
    pid_t childPid = -5;
    int ten = 10;
    int statusNum = 0;
    int termSig = -5;
    int fileIn = -1;
    int bgRed = -5;
    int bgCounter = 0;
    int shellPid = getpid();
    sprintf(tmpPid, "%i", shellPid);

    cAction.sa_handler = SIG_IGN;
    sigaction(SIGINT, &cAction, NULL);

    zAction.sa_handler = ctrlZ;
    sigfillset(&zAction.sa_mask);
    zAction.sa_flags= 0;
    sigaction(SIGTSTP, &zAction, NULL);

    //loop for getting input
    while(1 == 1){
        //reset values and clear the arguments
        counter = 0;
        counter2 = 0;
        comment = 0;
        inputR = -1;
        outputR = -1;
        backgroundAction = 0;
        background = 0;
        fileIn = -1;
        bgRed = -5;
        ioNum = 0;
        iNum = 0;
        oNum - 0;
        inputFile = NULL;
        outputFile = NULL;

        for(i = 0; i < MAX_ARGS; i++)
            arguments[i] = NULL;


        getInput(); //get input from user
        
        for(i = 0; i < strlen(userInput); i++){
            if((userInput[i] == '$') && (userInput[i+1] == '$')){

                userInput[i] = '\0';
                userInput[i+1] = '\0';
                strncat(userInput, tmpPid, strlen(tmpPid));
            }
        }
        if(strcmp(userInput, "exit\n") == 0){ //check for exit command
            doExit();
        }
        else if(strcmp(userInput, "status\n") == 0){ //check for status command
            doStatus(termSig, statusNum);
        }
        else if(strcmp(userInput, "\n") == 0){
            ; //pass
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
                tmpChar2 = token;

                if(*tmpChar2 == '<'){
                    iNum = 1;
                }
                else if(*tmpChar2 == '>'){
                    oNum = 1;
                }
                else if(iNum == 1){ // make sure this cannot happen twice
                    inputFile = token;
                    iNum = 0;
                }
                else if(oNum == 1){
                    outputFile = token;
                    oNum = 0;
                }
                else{
                    arguments[counter] = token;
                    counter++;
                }
                token = strtok(NULL, " \n"); 
            }

            if(strcmp(arguments[counter - 1], "&") == 0){
                arguments[counter - 1] = NULL;
                counter--;
                background = 1;
                backgroundAction = 1;

                if(fgMode == 1){
                    backgroundAction = 0;
                }
            }

            for(i = 0; i < counter; i++){
                if(strcmp(arguments[i], "<") == 0){
                    printf("input!\n");
                    inputR = i + 1;
                }
                else if(strcmp(arguments[i], ">") == 0){
                    printf("output!\n");
                    outputR = i + 1;
                }
            }

            if((comment == 1) || (arguments[0] == NULL)){
                ; //pass
            }
            else if((strcmp(arguments[0], "exit") == 0) && (background == 1) && 
            (arguments[1] == NULL)){
                doExit();
            }
            else if((strcmp(arguments[0], "status") == 0) && (background == 1) &&
            (arguments[1] == NULL)){ 
                doStatus(termSig, statusNum);
            }
            else if(strcmp(arguments[0], "cd") == 0){ //check for cd command
                if(arguments[1] == NULL){ //if no given directory
                    chdir(getenv("HOME"));
                    statusNum = 0;
                }
                else if((arguments[2] == NULL) && (!(arguments[1] == NULL))){ //if there are only two arguments
                    doCD();
                    statusNum = 0;
                }
                else{ //more than two arguments
                    printf("error: too many arguments\n");
                    fflush(stdout);
                }
            }
            else{
                //check for input
                if(inputR != -1){
                    printf("%s%s\n", "input file: ", arguments[inputR]);
                }
                //check for output
                if(outputR != -1){
                    printf("%s%s\n", "output file: ", arguments[outputR]);
                }

                spawnPid = fork();
                switch(spawnPid){
                    case -1:
                        printf("error\n");
                        fflush(stdout);
                        exit(1);
                        break;
                    case 0:
                        if(inputFile != NULL){
                            tmpvar1 = open(inputFile, O_RDONLY);
                            if(tmpvar1 == -1){
                            printf("%s%s\n", "error: cannot open ", inputFile);
                            fflush(stdout);
                            exit(1);
                        }
                        dupNum = dup2(tmpvar1, 0);
                        if(dupNum == -1){
                            printf("error: dup2");
                            fflush(stdout);
                            exit(1);
                        }
                        close(tmpvar1);
                        }
                        if(outputFile != NULL){
                            tmpvar2 = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0744);
                            if(tmpvar2 == -1){
                            printf("%s%s\n", "error: cannot open ", outputFile);
                            fflush(stdout);
                            exit(1);
                        }
                        dupNum = dup2(tmpvar2, 1);
                        if(dupNum == -1){
                            printf("error: dup2");
                            fflush(stdout);
                            exit(1);
                        }
                        close(tmpvar2);
                        }
                        
                        execvp(arguments[0], arguments);
                        
                        printf("%s%s\n", arguments[0], ": command not found");
                        fflush(stdout);
                        exit(1);
                        break;
                    default:
                        //if background dont wait
                        if(backgroundAction == 0){
                            actualPid = waitpid(spawnPid, &statusNum, 0);
                        }
                        else{
                            printf("%s%i\n", "background pid is ", spawnPid);
                        }
                        break;
                }
            }

        }

        //check for bg processes that have terminated
        do{
            childPid = waitpid(-1, &statusNum, WNOHANG);
            if(childPid > 0){
                printf("%s%i%s", "background pid ", childPid, " is done: ");
                doStatus(termSig, statusNum);
            }
        }while(childPid > 0);

    } //end of while
}

//get input from the user
void getInput(){ 
    printf("%s", ": ");
    fflush(stdout);
    fgets(userInput, MAX_INPUT, stdin);
}

//exit the program
void doExit(){
    exit(0);
}

//change to the given directory
void doCD(){
    chdir(arguments[1]);
}

//status
void doStatus(int termSig, int statusNum){
    if(WIFEXITED(statusNum) != 0){ //exited normally
        printf("%s%i\n", "exit value ", WEXITSTATUS(statusNum));
        fflush(stdout);
    }
    else{
        printf("%s%i\n", "exit value ", statusNum);
    }
}

//exec
void doExec(){
    printf("inside exec\n");
}

void test1(){
    printf("here10\n");

}

void ctrlC(int signalNum1){
    ; //pass
}

void ctrlZ(int signalNum2){
    int i;
    userInput[0] = '\n';
    arguments[0] = NULL;
    for(i = 1; i < MAX_ARGS; i++){
        arguments[i] = NULL;
        userInput[i] = '\0';
    }

    char* message;
    if(fgMode == 0){
        message = "Entering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 49);
        fflush(stdout);
        fgMode = 1;
    }
    else{
        message = "Exiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 29);
        fflush(stdout);
        fgMode = 0;
    }
}


// void catchSIGINT(int signo){                //Code to catch and display message for signals
// 	char* message = "\nCaught SIGINT.\n";
// 	write(STDOUT_FILENO, message, 38);
// }

// void catchSigStop(int signal){
// 	char* message = "\nCaught SIGTSTP.\n";
// 	write(STDOUT_FILENO, message, 25);
// 	// exit(0);
// }



//questions: 
//background
// no wait


//how to grading script
//how to do $$
//kill processes
//check kill

//ctrl c and z with signals?

//input / output
