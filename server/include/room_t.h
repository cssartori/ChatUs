/*
 * Developed by:
 * Carlo Sulzbach Sartori
 * Project done for "INF01151-Operating Systems II" class
 * 2015 - UFRGS
 *
 * The project is a Chat Client and Server in order to exercise the knowledge of
 * mutex/lock, semaphores, threads, concurrency and interprocess comunication.
 *
 * Originally, the was request for a program done in pure C, but to implement a Graphical User Interface the professor
 * allowed the implamentation to be done in C++ (strictly for the GUI).
 * That's the reason most of the core code is done using C notation and only the GUI uses OO. Also
 * some trickies had to be done in order to communicate between GUI and C pure code.
*/

#include "../../protocol/include/constants.h"
#include "user_t.h"

#ifndef _ROOM_T_H
#define _ROOM_T_H

#define ROOM_SIZE sizeof(Room_t)

/*Keeps room information*/
typedef struct Room_t
{
    /*Room ID in server (for fast access)*/
    int room_id;
    /*Room name in server*/
    char room_name[MAX_ROOMNAME_SIZE];
    /*Number of users joined in room*/
    int number_users;
    /*List of the users in the room*/
    User_t *users_in_room[MAX_USERS_ROOM];
} Room_t;

/*Basic error and return messages of room functions*/
enum{ ROOMALLOC_ERROR,
      ROOMID_ERROR,
      ROOM_FULL,
      USER_NOT_FOUND
};

enum{ ROOM_INITIALIZED = USER_NOT_FOUND+1,
      USER_JOINED_ROOM,
      USER_UNJOINED_ROOM,
      ROOM_DESTROYED



};

int init_room_t(Room_t* room, const char* room_name, int room_id);
int join_user_room_t(Room_t* room, User_t* user_to_join);
int unjoin_user_room_t(Room_t* room, User_t* user_to_unjoin);
int destroy_room_t(Room_t* room);
#endif
