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
void doExec();
void test1();

//global variables
char userInput[MAX_INPUT];
char* arguments[MAX_ARGS];
char dirName[MAX_INPUT];
// char inputFile[MAX_INPUT];
// char outputFile[MAX_INPUT];
char tmpPid[MAX_SIZE];

int main(){
    // int counter = 0;
    int i, j, counter, counter2, comment, inputR, outputR, last, background, 
    cpid, tmpvar1, tmpvar2, ioNum, iNum, oNum, dupNum;
    char *first, *tmpChar, *tmpChar2, *p, *inputFile, *outputFile;
    // char *tmpChar;
    // char* tmpChar2;
    // char* p;
    // char *inputFile, *outputFile; //, *outputFile;
    // char* outputFile;
    pid_t spawnPid = -5;
    pid_t actualPid = -5;
    int ten = 10;
    int statusNum = 0;
    int termSig = -5;
    int fileIn = -1;
    int shellPid = getpid();
    printf("%s%i\n", "shellPid: ", shellPid);

    // printf("here1!\n");

    //loop for getting input
    while(1 == 1){
        //reset values and clear the arguments
        counter = 0;
        counter2 = 0;
        comment = 0;
        inputR = -1;
        outputR = -1;
        background = 0;
        fileIn = -1;
        ioNum = 0;
        iNum = 0;
        oNum - 0;
        inputFile = NULL;
        outputFile = NULL;

        for(i = 0; i < MAX_ARGS; i++)
            arguments[i] = NULL;

        getInput(); //get input from user
        // printf("%s%s\n", "input: ", userInput);
        for(i = 0; i < strlen(userInput); i++){
            if((userInput[i] == '$') && (userInput[i+1] == '$')){
                // how to get pid of parent
                // printf("here103\n");
                sprintf(tmpPid, "%i", shellPid);
                // printf("%i\n", tmpPid);

                for(j = 0; j < strlen(tmpPid); j++){
                    userInput[i+j] = tmpPid[j];
                }
                // printf("%s%s\n", "input: ", userInput);
            }
        }
        // if(userInput[])
        

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
                // fflush(stdout);
                // arguments[counter] = token;

                // change so if it detects < or > then not add
                tmpChar2 = token;
                // if((*tmpChar2 == '<') || (*tmpChar2 == '>')){
                    // printf("here10\n");
                    // ioNum = 1;
                // }
                if(*tmpChar2 == '<'){
                    // printf("here<\n");
                    // *tmpChar = '0';
                    // ; //pass
                    iNum = 1;
                }
                else if(*tmpChar2 == '>'){
                    // printf("here>\n");
                    // ; //pass
                    // ioNum = 1;
                    oNum = 1;
                }
                // else if(ioNum == 1){
                //     ioNum = 0;
                // }
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

                // printf("%s%s\n", "arguments: ", arguments[counter]);
                
                // tmpChar = token;
                // if(*tmpChar == '$'){
                //     printf("true!\n");
                //     *tmpChar = '0';
                // }

                // printf("%s%s\n", "p: ", p);


                //sprintf 

                // for(tmpChar = token; *tmpChar; ++tmpChar){
                //     // printf("here2\n");
                //     // printf("%s\n", *tmpChar);
                //     if(*tmpChar == '$'){
                //         *tmpChar = 'z';
                //         counter2++;
                //         if(counter2 == 2){

                //         }
                //     }
                // }

                // token = strtok(NULL, " \n"); 
                // counter++;
            }

            // //print arguments
            // for(i = 0; i < counter; i++){
            //     printf("%s%i%s%s\n", "argument", i+1, ": ", arguments[i]);
            // }






            if(strcmp(arguments[counter - 1], "&") == 0){
                // printf("background!\n");
                background = 1;
                arguments[counter - 1] = NULL;
                counter--;
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
                    // perror("error: too many arguments\n");
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
                    ; //pass
                    // printf("inside background\n");
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
                        // if(inputR != -1){
                        if(inputFile != NULL){
                            // tmpvar1 = open(arguments[inputR], O_RDONLY);
                            tmpvar1 = open(inputFile, O_RDONLY);
                            if(tmpvar1 == -1){
                            // printf("smallsh: cannot open %s for input\n", arguments[inputR]);
                            printf("smallsh: cannot open %s for input\n", inputFile);
                            fflush(stdout);
                            _Exit(1);
                        }
                        //expand this
                        dupNum = dup2(tmpvar1, 0);
                        // if(dup2(tmpvar1, 0) == -1){
                        if(dupNum == -1){
                            // perror("dup2");
                            printf("error: dup2");
                            fflush(stdout);
                            exit(1);
                        }
                        close(tmpvar1);
                        }
                        // if(outputR != -1){
                        if(outputFile != NULL){
                            // tmpvar2 = open(arguments[outputR], O_WRONLY | O_CREAT | O_TRUNC, 0744);
                            tmpvar2 = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0744);
                            if(tmpvar2 == -1){
                            // printf("smallsh: cannot open %s\n", arguments[outputR]);
                            printf("smallsh: cannot open %s\n", outputFile);
                            fflush(stdout);
                            exit(1);
                        }
                        dupNum = dup2(tmpvar2, 1);
                        // if(dup2(tmpvar2, 1) == -1){
                        if(dupNum == -1){
                            // perror("dup2");
                            printf("error: dup2");
                            fflush(stdout);
                            exit(1);
                        }
                        close(tmpvar2);
                        }
                        // printf("%s%d\n", "child: ", getpid());
                        execvp(arguments[0], arguments);
                        
                        // perror("Invalid command\n");
                        printf("%s%s\n", arguments[0], ": command not found");
                        // perror("%s%s\n", arguments[0], ": command not found");
                        fflush(stdout);
                        exit(1);
                        break;
                    default:
                        //if background dont wait
                        //put spawnpid into bg array
                        if(background != 1){
                            actualPid = waitpid(spawnPid, &statusNum, 0);
                        }


                        // printf("%s%d\n", "parent: ", getpid());
                        // printf("PARENT(%d): Wait()ingfor child(%d) to terminate\n", getpid(), spawnPid);
                        // actualPid = waitpid(spawnPid, &statusNum, 0);
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



//questions: 
//background
// no wait


//how to grading script
//how to do $$
//kill processes
//check kill

//ctrl c and z with signals?

//input / output
