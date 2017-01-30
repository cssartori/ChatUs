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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/protocol.h"

/*Given a connection initialization error, prints the referred error message*/
void errorTreatment(SOCKET socket, int error)
{
    char type[20];

    switch(error)
    {
    case OPENSOCK_ERROR:
        strcpy(type, "Open Socket.");
        break;
    case BIND_ERROR:
        strcpy(type, "Bind");
        break;
    case ACCEPT_ERROR:
        strcpy(type, "Accept");
        break;
    case LISTEN_ERROR:
        strcpy(type, "Listen");
        break;
    case RECEIVE_ERROR:
        strcpy(type, "Receive");
        break;
    case CONNECT_ERROR:
        strcpy(type, "Connect");
        break;
    default:
        strcpy(type, "undefined.");
        break;
    }
    printf("\nError at %s\n", type);
    close(socket);
    exit(1);
}

/*Given a command and its string, extracts the parameter from command (e.g., new room name)*/
char* extract_command_parameter(int command, const char* command_str) {

    switch(command) {
    case CREATE_ROOM_COMMAND:
    case CHANGE_NICKNAME_COMMAND:
    case JOIN_ROOM_COMMAND:
        char* parameter = (char*)malloc(MAX_USERNAME_SIZE);
        /*Starts command string counter at length of command +1 (skipping white space)*/
        int j = strlen(change_nickname_command_str)+1;
        int i=0;

        /*gets parameter*/
        while(command_str[j] != '\0' && i<MAX_USERNAME_SIZE) {
            parameter[i] = command_str[j];
            i++;
            j++;
        }
        parameter[i] = '\0';
        return parameter;
        break;
    }

    return NULL;
}

