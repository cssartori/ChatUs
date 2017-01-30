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

#ifndef _USER_T_H
#define _USER_T_H

#define USER_SIZE sizeof(User_t)

/*Keeps user information*/
typedef struct User_t
{
    /*Keeps user socket for communication purposes*/
    SOCKET user_socket;
    /*Indiates if a user is in a room or not*/
    BOOL is_joined;
    /*If a user is connected to the server or not (should always be TRUE)*/
    BOOL is_connected;
    /*User name on the server*/
    char user_name[MAX_USERNAME_SIZE];
    /*User ID on the server*/
    int user_id;
    /*Room ID to which user is joined on the server*/
    int room_id;
} User_t;


/*Enums with error indicators and possible returns for user_t functions*/
enum {USERALLOC_ERROR,
      USERID_ERROR,
      INVALIDUSER_ERROR
     };

enum {USER_INITIALIZED = INVALIDUSER_ERROR+1,
      USER_CONNECTED,
      USER_JOINED,
      USER_DISCONNECTED,
      USER_UNJOINED,
      USERNAME_SET
     };

/*User_t functions*/
int init_user_t(User_t* user, int user_id, SOCKET user_socket, const char *user_name);
void connect_user_t(User_t* user);
void disconnect_user_t(User_t* user);
void join_user_t(User_t* user, int room_id);
void unjoin_user_t(User_t* user);
int set_name_user_t(User_t* user, const char *new_user_name);


#endif
