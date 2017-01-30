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

#ifndef _CLIENT_H
#define _CLIENT_H

#include <pthread.h>
#include "../../protocol/include/constants.h" //define BOOL type
#include "signaler.h"

int init_client(Signaler *client_gui_signaler, char server_ip[10], int client_port, int server_port);
void user_create_room_request(const char *new_room_name);
void user_join_room_request(const char* room);
void user_leave_room_request();
void user_change_nickname_request(const char *new_user_nickname);
void user_quit_request();
void user_message_request(const char* message);
void* client_server_communication(void* arg) ;
int get_command_sent();
BOOL is_user_joined();
BOOL is_user_connected();
char* get_user_name();
char* get_user_room_name();
void close_client();

#endif // _CLIENT_H
