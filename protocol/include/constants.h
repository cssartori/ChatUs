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

#ifndef _CONSTANT_H
#define _CONSTANT_H

#define MAX_ROOMNAME_SIZE 20
#define MAX_USERNAME_SIZE 20
#define MAX_MESSAGESTR_SIZE 1200

#define MAX_USERS_ROOM 20
#define MAX_USERS_CONNECTED 100
#define MAX_ROOMS_CONNECTED 50

#define	SOCKET  int
#define INVALID_SOCKET -1

#define BOOL char
#define TRUE 1
#define FALSE 0


enum{USERNAMESIZE_ERROR = 200,
     ROOMNAMESIZE_ERROR,
     MESSAGESIZE_ERROR
};


#endif

