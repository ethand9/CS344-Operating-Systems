#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <unistd.h>

#define ROOM_TOTAL 10
#define ROOM_MAX 7
#define NAME_LENGTH 8
#define CONNECT_TOTAL 6
#define DIR_LENGTH 30

enum roomType{ //enum of room types
    START, END, MID
};

struct room{
    char*roomName; //Room name
    enum roomType type; //Type of room
    int connections[CONNECT_TOTAL]; //paths to other rooms
    int connectionsNum; //number of connections this room has
    int chosen; //bool if a room has been selected to be built or not
    int connected; //bool if a room has been connected or not
    int picked; //bool if a room has been connected to the current room
};

void nameRooms(struct room*room); //name all rooms
void clearConnections(struct room*room); //set all connections to null
void choose(struct room*room); //randomly choose the rooms to build
void connect(struct room*room); //randomly assign room types and randomly connect the rooms to each other
int isConnectionsMet(struct room*room); //checks if each built room has at least 3 connections
void makeDirectory(struct room*room); //make a folder and put the room files in it

main(){
    srand(time(0)); // randomize the seed

    struct room rooms[ROOM_TOTAL];

    nameRooms(rooms); //name the rooms 
    clearConnections(rooms); //set all connections to null
    choose(rooms); //randomly choose the rooms to build
    connect(rooms); //randomly connect the rooms
    makeDirectory(rooms); //create the folder with the files and their contents

}

//name the rooms
void nameRooms(struct room*room){
    room[0].roomName = "cavern";
    room[1].roomName = "lake";
    room[2].roomName = "tunnel";
    room[3].roomName = "mine";
    room[4].roomName = "dungeon";
    room[5].roomName = "alcove";
    room[6].roomName = "ruins";
    room[7].roomName = "sinkhole";
    room[8].roomName = "hideout";
    room[9].roomName = "rubble";
}

//reset all connections
void clearConnections(struct room*room){
    int i, j;
    for(i=0; i<ROOM_TOTAL; i++){ //loop for all rooms
        for(j=0; j<CONNECT_TOTAL; j++){ //loop for all connections
            room[i].connections[j] = -1; //set the connection to null
            room[i].connectionsNum = 0; //set the number of connections to 0
        }
    }
}

//randomly choose the rooms to build
void choose(struct room*room){
    int i, elem;
    int num = 0;

    //set all rooms to not chosen
    for(i=0; i<ROOM_TOTAL; i++){ //loop for all rooms
        room[i].chosen = 0;
    }

    //randomly choose the rooms to build
    while(num < ROOM_MAX){ //build seven rooms
        elem = rand()%ROOM_TOTAL; //randomly choose a room to build
        if(room[elem].chosen != 1){ //check that the room is not already built
            room[elem].chosen = 1; //indicate that the room is built
            num++;
        }
    }
}

//randomly connect the rooms together
void connect(struct room*room){
    int i, elem, elem2;
    int flag1 = 1;

    //set all rooms types to MID
    for(i=0; i<ROOM_TOTAL; i++){
        room[i].type = MID;
    }

    //randomly choose a room to assign as START
    while(flag1 == 1){
        elem = rand()%ROOM_TOTAL; //randomly choose a built room
        //check that the room is chosen and that it has type MID
        if((room[elem].chosen == 1) && (room[elem].type == MID)){ 
            room[elem].type = START;
            flag1 = 0;
        }
    }
    flag1 = 1;

    //randomly choose a room to assign as END
    while(flag1 == 1){
        elem = rand()%ROOM_TOTAL; //randomly choose a built room
        //check that the room is chosen and that it has type MID
        if((room[elem].chosen == 1) && (room[elem].type == MID)){ 
            room[elem].type = END;
            flag1 = 0;
        }
    }

    do{ //loop until connections requirement is met
        //randomly choose a room
        elem = rand()%ROOM_TOTAL;
        //check that it is a built room
        if(room[elem].chosen == 1){
            //randomly choose another room
            elem2 = rand()%ROOM_TOTAL;
            //check that it is a built room
            //check that it is not the same room
            if((room[elem2].chosen == 1) && (elem != elem2)){
                //check that the rooms have not been connected already
                flag1 = 0;
                for(i = 0; i < CONNECT_TOTAL; i++){
                    if(room[elem].connections[i] == elem2){
                        flag1 = 1;
                    }
                }
                //connect the rooms and increment the connections
                if(flag1 == 0){
                    room[elem].connections[room[elem].connectionsNum] = elem2;
                    room[elem].connectionsNum++;
                    room[elem2].connections[room[elem2].connectionsNum] = elem;
                    room[elem2].connectionsNum++;
                }
            }
        }
    }while(isConnectionsMet(room) == 0);
}

int isConnectionsMet(struct room*room){ //checks if each built room has at least three connections
    int i;
    int met = 1;
    for(i = 0; i < ROOM_TOTAL; i++){
        if((room[i].chosen == 1) && (room[i].connectionsNum < 3)){ //conditions not met
            met = 0;
            break;
        }
    }
    return met;
}

void makeDirectory(struct room*room){ //create a directory and store the room files in it
    int i, j;
    int pid = getpid(); //get the pid
    char folderName[DIR_LENGTH];
    FILE*roomFile;

    sprintf(folderName, "duonget.rooms.%d", pid); //create the folder name
    mkdir(folderName); //create the folder
    chdir(folderName); //go to the folder

    //create the files and their contents
    for(i = 0; i < ROOM_TOTAL; i++){ //loop for all rooms
        if(room[i].chosen == 1){ //only write the built rooms
            roomFile = fopen(room[i].roomName, "a"); //create a file with the room name

            //write the file's contents
            //write the room name
            fprintf(roomFile, "%s%s%s", "ROOM NAME: ", room[i].roomName, "\n");

            //write the connections
            for(j = 0; j < CONNECT_TOTAL; j++){
                if(room[i].connections[j] == -1){
                    break;
                }
                fprintf(roomFile, "%s%d%s%s%s", "CONNECTION ", j+1, ": ", room[room[i].connections[j]].roomName, "\n");
            }

            //write the room type
            fprintf(roomFile, "%s", "ROOM TYPE: ");
            if(room[i].type == 0){
                fprintf(roomFile, "START_ROOM\n");
            }
            else if(room[i].type == 1){
                fprintf(roomFile, "END_ROOM\n");
            }
            else if(room[i].type == 2){
                fprintf(roomFile, "MID_ROOM\n");
            }
            else{
                fprintf(roomFile, "ERROR\n");
            }

            fclose(roomFile); //close the file
        }        
    }
}
