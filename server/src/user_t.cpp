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


#include "../include/user_t.h"
#include <stdlib.h>
#include <string.h>

int init_user_t(User_t* user, int user_id, SOCKET user_socket, const char *user_name) {

     if(user == NULL)
        return USERALLOC_ERROR;

     if(strlen(user_name) > MAX_USERNAME_SIZE) {
        return USERNAMESIZE_ERROR;
    }
    else if(user_socket == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }
    else if(user_id < 0) {
        return USERID_ERROR;
    }

    user->user_id = user_id;
    user->user_socket = user_socket;
    strcpy(user->user_name, user_name);
    user->is_connected = FALSE;
    user->is_joined = FALSE;
    user->room_id = -1;

    return USER_INITIALIZED;
}

void connect_user_t(User_t* user) {
    user->is_connected = TRUE;
}

void disconnect_user_t(User_t* user) {
    user->is_connected = FALSE;
}

void join_user_t(User_t* user, int room_id) {
    user->is_joined = TRUE;
    user->room_id = room_id;
}

void unjoin_user_t(User_t* user) {
    user->is_joined = FALSE;
    user->room_id=-1;
}

int set_name_user_t(User_t* user, const char *new_user_name) {

    if(strlen(new_user_name) > MAX_USERNAME_SIZE) {
        return USERNAMESIZE_ERROR;
    }

    strcpy(user->user_name, new_user_name);

    return USERNAME_SET;
}


