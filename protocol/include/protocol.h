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

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "constants.h"
/*Protocol definitions*/

/*Enum of commands available to exchange between client and server*/
enum{NO_COMMAND = -2,
     INVALID_COMMAND,
     MESSAGE_COMMAND,
     CREATE_ROOM_COMMAND,
     JOIN_ROOM_COMMAND,
     LEAVE_ROOM_COMMAND,
     CHANGE_NICKNAME_COMMAND,
     QUIT_CHAT_COMMAND,
     INIT_USER_COMMAND,
     BROADCAST_MESSAGE_COMMAND,
     BROADCAST_ROOMS_COMMAND,
     BROADCAST_USERS_COMMAND
};

/*ACKS and return messages available from server to client*/
enum{USER_CREATED_ROOM_ACK=500,
     USER_JOINED_ACK,
     USER_LEFT_ACK,
     USER_CHANGED_NICKNAME_ACK,
     USER_QUIT_ACK,
     USER_NICKNAME_ALREADY_EXISTS_ERROR,
     USER_ROOM_NAME_ALREADY_EXISTS_ERROR,
     USER_ROOM_SPACE_ERROR,
     USER_ROOM_FULL_ERROR,
     USER_ROOM_NOT_FOUND

};

/*General error flags for socket and connection initialization*/
enum {OPENSOCK_ERROR=1000,
      BIND_ERROR,
      ACCEPT_ERROR,
      LISTEN_ERROR,
      RECEIVE_ERROR,
      CONNECT_ERROR
};

/*Timeout for receive/send message*/
#define TIMEOUT 10

/*Root room definitions (room where all users start at)*/
#define ROOT_ROOM_ID -1
static const char root_room_name[] = "root";

/*Available user commands. All must begin with \\ and be 6 chars long*/
static const char create_room_command_str[] = "\\\\crterm";
static const char join_room_command_str[] = "\\\\joinrm";
static const char leave_room_command_str[] = "\\\\leavrm";
static const char change_nickname_command_str[] = "\\\\chngnm";
static const char quit_chat_command_str[] = "\\\\quitct";

/*ACK strings to respond to users command*/
static const char user_connection_confirmed_ack_str[] = "\\\\hello!";
static const char user_created_room_ack_str[] = "\\\\rmcrtd";
static const char user_joined_ack_str[] = "\\\\joined";
static const char user_left_ack_str[] = "\\\\leftrm";
static const char user_changed_nickname_ack_str[] = "\\\\chnged";
static const char user_quit_ack_str[] = "\\\\quited";

/*Special characters not allowed in user names or room names*/
static const char not_allowed_chars_name[4] = {'$', '%', '&', '#'};

/*Basic functions necessary in both client and server*/
/*error treatment for connection initialization*/
void errorTreatment(SOCKET socket, int error);
/*Extract command parameter from command string*/
char* extract_command_parameter(int command, const char* command_str);

#endif
