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

#ifndef _MESSAGE_T_H
#define _MESSAGE_T_H
#include "constants.h"

/*Keeps message information for exchange between server and clients*/
typedef struct Message_t
{
    /*Command sent*/
    int command;
    /*User ID from sender*/
    int user_id;
    /*Room ID from where message was sent*/
    int room_id;
    /*Room name from where message was sent*/
    char room_name[MAX_ROOMNAME_SIZE];
    /*User name from who sent*/
    char user_name[MAX_USERNAME_SIZE];
    /*The message string as it is*/
    char message_string[MAX_MESSAGESTR_SIZE];
} Message_t;

#define MESSAGE_SIZE (int)sizeof(Message_t)

enum {MESSAGEALLOC_ERROR
     };

enum {MESSAGE_INITIALIZED = MESSAGEALLOC_ERROR+1
     };

int init_message_t(Message_t* message, int command, int user_id, int room_id, const char *room_name, const char *user_name, const char *message_str) ;
/*Functions of marshalling and unmarshalling integer attributes from message*/
void marshalling_message(Message_t* message);
void unmarshalling_message(Message_t* message);
#endif
