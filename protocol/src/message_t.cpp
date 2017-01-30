/*
 * Developed by:
 * Carlo Sulzbach Sartori
 * Project done for "INF01151-Operating Systems II" class
 * 2015 - UFRGS
 *
 * The project is a Chat Client and Server in order to exercise the knowledge of
 * mutex/lock, semaphores, threads, concurrency and interprocess comunication.
 *
 * Originally, the request was for a program done in pure C, but to implement a Graphical User Interface the professor
 * allowed the implamentation to be done in C++ (strictly for the GUI).
 * That's the reason most of the core code is done using C notation and only the GUI uses OO. Also
 * some trickies had to be done in order to communicate between GUI and C pure code.
*/

#include "../include/message_t.h"
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int init_message_t(Message_t* message, int command, int user_id, int room_id, const char *room_name, const char *user_name, const char *message_str) {

	if(message == NULL)
		return MESSAGEALLOC_ERROR;

    if(strlen(room_name) > MAX_ROOMNAME_SIZE)
        return ROOMNAMESIZE_ERROR;
    if(strlen(user_name) > MAX_USERNAME_SIZE)
        return USERNAMESIZE_ERROR;
    if(strlen(message_str) > MAX_MESSAGESTR_SIZE)
        return MESSAGESIZE_ERROR;

    message->command = command;
    message->user_id = user_id;
    message->room_id = room_id;
    strcpy(message->user_name, user_name);
    strcpy(message->room_name, room_name);
    strcpy(message->message_string, message_str);

    return MESSAGE_INITIALIZED;
}

/*Marshalling of message; integer parameters must go through it*/
void marshalling_message(Message_t* message){
    message->command = htonl(message->command);
    message->user_id = htonl(message->user_id);
    message->room_id = htonl(message->room_id);
}

/*Unmarshalling of message*/
void unmarshalling_message(Message_t* message){
    message->command = ntohl(message->command);
    message->user_id = ntohl(message->user_id);
    message->room_id = ntohl(message->room_id);
}
