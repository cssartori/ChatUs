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


#ifndef _SERVER_H
#define _SERVER_H

enum{ USER_CREATED,
      ROOM_CREATED,
      USER_JOIN_SUCC,
      USER_UNJOIN_SUCC,
      USER_NICKNAME_CHANGED,
      USER_MESSAGE_BROADCASTED,
      ROOMS_BROADCASTED,
      USER_LIST_ROOM_BROADCASTED,
      NICKNAME_EXISTS_ERROR,
      ROOM_NAME_EXISTS_ERROR,
      ROOM_SPACE_ERROR,
      ROOM_FULL_ERROR,
      USER_CREATION_ERROR,
      USER_JOIN_ERROR,
      USER_UNJOIN_ERROR,
      USER_ALREADY_JOINED_ERROR,
      USER_STILL_JOINED_ERROR,
      USER_QUIT_SUCC,
      ROOM_NOT_FOUND_ERROR
};

#endif
