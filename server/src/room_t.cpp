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


#include "../include/room_t.h"
#include <stdlib.h>
#include <string.h>

int init_room_t(Room_t* room, const char* room_name, int room_id) {
    
	if(room == NULL)
		return ROOMALLOC_ERROR;

    if(strlen(room_name) > MAX_ROOMNAME_SIZE)
        return ROOMNAMESIZE_ERROR;

    if(room_id < 0)
        return ROOMID_ERROR;

    strcpy(room->room_name, room_name);
    room->room_id = room_id;
    room->number_users=0;
    int i;
    for(i=0;i<MAX_USERS_ROOM;i++)
        room->users_in_room[i] = NULL;

    return ROOM_INITIALIZED;
}

int join_user_room_t(Room_t* room, User_t* user_to_join) {

    int i=0;
    for(;i<MAX_USERS_ROOM;i++) {
        if(room->users_in_room[i] == NULL)
        {
            room->number_users++;
            join_user_t(user_to_join, room->room_id);
            room->users_in_room[i] = user_to_join;
            return USER_JOINED_ROOM;
        }
    }

    return ROOM_FULL;
}

int unjoin_user_room_t(Room_t* room, User_t* user_to_unjoin) {

    int i=0;
    for(;i<MAX_USERS_ROOM;i++) {
        if(room->users_in_room[i] != NULL && room->users_in_room[i]->user_id == user_to_unjoin->user_id)
        {
            room->number_users--;
            unjoin_user_t(user_to_unjoin);
            room->users_in_room[i] = NULL;
            return USER_UNJOINED_ROOM;
        }
    }

    return USER_NOT_FOUND;
}

int destroy_room_t(Room_t* room) {

    free(room);
    return ROOM_DESTROYED;
}
