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

void getInput();
void doExit();
void doCD();
void doStatus(int termSig, int statusNum);
void doExec();
void test1();

//global variables
char userInput[MAX_INPUT];
char* arguments[MAX_INPUT];
char dirName[MAX_INPUT];

int main(){
    // int counter = 0;
    int i, counter, comment, inputR, outputR, last, background, cpid, tmpvar1, tmpvar2;
    char* first;
    pid_t spawnPid = -5;
    pid_t actualPid = -5;
    int ten = 10;
    int statusNum = 0;
    int termSig = -5;
    int fileIn = -1;

    //loop for getting input
    while(1 == 1){
        //reset values and clear the arguments
        counter = 0;
        comment = 0;
        inputR = -1;
        outputR = -1;
        background = 0;
        fileIn = -1;
        
        for(i = 0; i < MAX_INPUT; i++)
            arguments[i] = NULL;

        getInput(); //get input from user
        // printf("%s%s\n", "input: ", userInput);

        if(strcmp(userInput, "exit\n") == 0){ //check for exit command
            doExit();
        }
        else if(strcmp(userInput, "status\n") == 0){ //check for status command
            // printf("here1\n");
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
                // printf("%s\n", token); 
                fflush(stdout);
                arguments[counter] = token;

                // change so if it detects < or > then not add


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

            if((comment == 1) || (arguments[0] == NULL)){
                ; //pass
            }
            else if((strcmp(arguments[0], "exit") == 0) && (background == 1) && 
            (strcmp(arguments[1], "&") == 0) && (arguments[2] == NULL)){
                doExit();
            }
            else if((strcmp(arguments[0], "status") == 0) && (background == 1) && 
            (strcmp(arguments[1], "&") == 0) && (arguments[2] == NULL)){
                doStatus(termSig, statusNum);
            }
            else if(strcmp(arguments[0], "cd") == 0){ //check for cd command
                // printf("%s%s\n", "arg 0: ", arguments[0]);
                // printf("%s%s\n", "arg 1: ", arguments[1]);
                // printf("%s%s\n", "arg 2: ", arguments[2]);
                if(arguments[1] == NULL){ //if no given directory
                    chdir(getenv("HOME"));
                    statusNum = 0;
                    // getcwd(dirName, sizeof(dirName));
                    // printf("Current working dir: %s\n", dirName);
                    // fflush(stdout);
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
                    // fileIn = open("/dev/null", O_RDONLY); //Redirect to null device to ignore information
                    // if(fileIn == -1){
                    //     perror("open");
                    //     _Exit(1);
                    // }
                    // if(dup2(fileIn, 0) == -1){
                    //     perror("dup2");
                    //     _Exit(1);
                    // }
                    // close(fileIn);
                }

                // printf("invalid command\n");
                // fflush(stdout);

                // //fork and status testing
                // spawnPid = fork();
                // switch(spawnPid){
                //     case -1:
                //         perror("hull breach!\n");
                //         exit(1);
                //         break;
                //     case 0:
                //         ten++;
                //         printf("I am the child! ten = %d\n", ten);
                //         break;
                //     default:
                //         ten = ten -1;
                //         printf("I am the parent! ten = %d\n", ten);
                //         break;
                // }
                // printf("both!\n");
                spawnPid = fork();
                switch(spawnPid){
                    case -1:
                        printf("error\n");
                        fflush(stdout);
                        exit(1);
                        break;
                    case 0:
                        if(inputR != -1){
                            tmpvar1 = open(arguments[inputR], O_RDONLY);
                            if(tmpvar1 == -1){
                            printf("smallsh: cannot open %s for input\n", arguments[inputR]);
                            fflush(stdout);
                            _Exit(1);
                        }
                        if(dup2(tmpvar1, 0) == -1){
                            perror("dup2");
                            _Exit(1);
                        }
                        close(tmpvar1);      //Need to close since it was opened
                        }
                        if(outputR != -1){
                            tmpvar2 = open(arguments[outputR], O_WRONLY | O_CREAT | O_TRUNC, 0744);
                            if(tmpvar2 == -1) {
                            printf("smallsh: cannot open %s\n", arguments[outputR]);
                            fflush(stdout);
                            _Exit(1);
                        }
                        if(dup2(tmpvar2, 1) == -1){
                            perror("dup2");
                            _Exit(1);
                        }
                        close(tmpvar2);
                        }
                        // printf("%s%d\n", "child: ", getpid());
                        execvp(arguments[0], arguments);
                        
                        // perror("Invalid command\n");
                        printf("%s%s\n", arguments[0], ": command not found");
                        fflush(stdout);
                        exit(1);
                        break;
                    default:
                        // printf("%s%d\n", "parent: ", getpid());
                        // printf("PARENT(%d): Wait()ingfor child(%d) to terminate\n", getpid(), spawnPid);
                        actualPid = waitpid(spawnPid, &statusNum, 0);
                        // printf("PARENT(%d): Child(%d) terminated, Exiting!\n", getpid(), actualPid);
                        // exit(0);
                        break;
                }
            }

        }

        // cpid = waitpid(-1, &statusNum, WNOHANG); //Check if any process has completed; Returns 0 if no terminated processes
        // while(cpid > 0){
        //     printf("background process, %i, is done: ", cpid);
        //     doStatus(termSig, statusNum);
        //     cpid = waitpid(-1, &statusNum, WNOHANG);
        // }
    } //end of while
}

//get input from the user
void getInput(){ 
    // static char str[100];
    printf("%s", ": ");
    fflush(stdout);
    fgets(userInput, MAX_INPUT, stdin);
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
    // getcwd(dirName, sizeof(dirName));
    // printf("Current working dir: %s\n", dirName);
    // fflush(stdout);

    // getcwd(dirName, sizeof(dirName));
    // printf("Current working dir: %s\n", dirName);

    // chdir(getenv("HOME"));
    // getcwd(dirName, sizeof(dirName));
    // printf("Current working dir: %s\n", dirName);
}

//status
void doStatus(int termSig, int statusNum){
    // printf("inside status\n");
    if(WIFEXITED(statusNum) != 0){ //exited normally
        // printf("%s%i\n", "exit value ", statusNum);
        printf("%s%i\n", "exit value ", WEXITSTATUS(statusNum));
        fflush(stdout);
    }
    else{
        printf("%s%i\n", "terminated by signal ", termSig);
        fflush(stdout);
    }


    // if((termSig == -5) && (statusNum == -5)){
    //     printf("exit value 0\n");
    //     fflush(stdout);
    // }
    // else{
    //     // printf("not default\n");
    //     printf("%s%d\n", "exit value ", statusNum);
    //     fflush(stdout);
    // }
}

//exec
void doExec(){
    printf("inside exec\n");
}

void test1(){
    printf("here10\n");

}


//todo:
//status and pid
