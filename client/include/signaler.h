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

/*
 * This module is the communication between the C code (client.cpp) and the C++ code (clientmainwindow.cpp),
 * making it possible for the server messages' to have effect on the GUI
*/

#ifndef SIGNALER_H
#define SIGNALER_H

#include <QObject>

class Signaler: public QObject{
    Q_OBJECT
public:
    Signaler(){

    }
    void send_gui_output_message(const char* output){
        emit output_message_ready(output);
    }
    void pop_user_from_room(){
        emit pop_user_room();
    }
    void change_user_nickname(const char* new_nickname){
        emit change_nick(new_nickname);
    }
    void set_user_in_room(const char* room){
        emit set_user_room(room);
    }
    void update_room_list(const char* room_list){
        emit set_room_list(room_list);
    }
    void update_user_list(const char* user_list){
        emit set_user_list(user_list);
    }
    void user_nickname_exists(){
        emit user_name_exists();
    }
    void room_full_error(){
        emit room_full();
    }
    void room_space_error(){
        emit room_space();
    }
    void room_name_exists_error(){
        emit room_name_exists();
    }
    void room_not_found_error(){
        emit room_not_found();
    }

signals:
    void output_message_ready(const char* output);
    void set_user_room(const char* room);
    void pop_user_room();
    void change_nick(const char* new_nickname);
    void set_room_list(const char* room_list);
    void set_user_list(const char* user_list);
    void user_name_exists();
    void room_full();
    void room_space();
    void room_name_exists();
    void room_not_found();

};

#endif // SIGNALER_H
