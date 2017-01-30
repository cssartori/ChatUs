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

#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../include/clientmainwindow.h"
#include "../../protocol/include/message_t.h"
#include "../../protocol/include/constants.h"
#include "../../protocol/include/protocol.h"
#include "../include/client.h"
#include "../include/signaler.h"


/*Main function of the client chat*/
int main(int argc, char **argv)
{

    char server_ip[10];
    int client_port = -1;
    int server_port = -1;

    if(argc < 6)
    {/*Server IP, Client Port and Server Port must be informed*/
        printf("\tUsage:\n\t%s -h <server_ip> -p <client_port> -s <server_port>\n", argv[0]);
        exit(1);
    }

    int i=0;

    // Get Parameters
    for(i=1; i<argc; i++)
    {
        if(argv[i][0]=='-')
        {
            switch(argv[i][1])
            {
            case 'h': // Server IP number
                i++;
                strcpy(server_ip, argv[i]);
                break;
            case 'p': // Client Port
                i++;
                client_port = atoi(argv[i]);
                if(client_port < 1024) //Port numbers smaller than 1024 are reserved to special usage by definition
                {
                    printf("Invalid client port value. Must be higher than 1023.\n");
                    exit(1);
                }
                break;
            case 's': //Server Port
                i++;
                server_port = atoi(argv[i]);
                if(server_port < 1024)
                {
                    printf("Invalid server port value. Must be higher than 1023.\n");
                    exit(1);
                }
                break;
            default:
                printf("Invalid parameter %i: %s\n",i, argv[i]);
                exit(1);
            }
        }
        else
        {
            printf("Invalid parameter %i: %s\n",i, argv[1]);
            exit(1);
        }
    }


    /*Creates GUI variables*/
    QApplication a(argc, argv);
    ClientMainWindow w;

    /*Signaler to allow client - client GUI communication (throughout signals)*/
    Signaler client_gui_signaler;

    /*Initialize client informations - socket, name, and server communication*/
    init_client(&client_gui_signaler, server_ip, client_port, server_port);

    /*Sets user name on GUI*/
    w.set_user_name(get_user_name());

    /*Connects slots in GUI with the signaler to allow client - clientGUI (two different threads) communication */
    QObject::connect(&client_gui_signaler, SIGNAL(output_message_ready(const char*)), &w, SLOT(append_output_message(const char*)));
    QObject::connect(&client_gui_signaler, SIGNAL(set_user_room(const char*)), &w, SLOT(set_user_in_room(const char*)));
    QObject::connect(&client_gui_signaler, SIGNAL(pop_user_room()), &w, SLOT(pop_user_from_room()));
    QObject::connect(&client_gui_signaler, SIGNAL(change_nick(const char*)), &w, SLOT(set_user_name(const char*)));
    QObject::connect(&client_gui_signaler, SIGNAL(set_room_list(const char*)), &w, SLOT(update_room_list(const char*)));
    QObject::connect(&client_gui_signaler, SIGNAL(set_user_list(const char*)), &w, SLOT(update_room_list(const char*)));
    QObject::connect(&client_gui_signaler, SIGNAL(user_name_exists()), &w, SLOT(user_name_exists_error()));
    QObject::connect(&client_gui_signaler, SIGNAL(room_full()), &w, SLOT(room_full_error()));
    QObject::connect(&client_gui_signaler, SIGNAL(room_space()), &w, SLOT(room_space_error()));
    QObject::connect(&client_gui_signaler, SIGNAL(room_name_exists()), &w, SLOT(room_name_exists_error()));
    QObject::connect(&client_gui_signaler, SIGNAL(room_not_found()), &w, SLOT(room_not_found_error()));

    /*Thread for client-server communication*/
    pthread_t client_server_com_thread;
    w.show();

    /*Creates thread that will keep communication between server and client*/
    pthread_create(&client_server_com_thread, NULL, client_server_communication, NULL);

    /*Start executing the GUI*/
    return a.exec();
}

