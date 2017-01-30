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
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../protocol/include/protocol.h"
#include "../../protocol/include/message_t.h"
#include "../include/client.h"
#include <sys/timeb.h>
#include <fcntl.h>

/*User connected (client) state information*/
char user_name[MAX_USERNAME_SIZE];
int user_id;
int user_room_id;
BOOL user_is_joined;
char user_room_name[MAX_ROOMNAME_SIZE];
SOCKET client_socket;
BOOL user_is_connected;

/*Room/users list variables*/
char room_list_names[MAX_MESSAGESTR_SIZE];
char user_list_names[MAX_MESSAGESTR_SIZE];

/*Client-server communication variables*/
int command_sent;                   //last command sent from client to server
Message_t *message_to_send = NULL;  //send message buffer
BOOL message_waiting_send = FALSE;  //send message flag
pthread_t* client_server_com_thread;

/*To keep interaction between client and client-GUI*/
Signaler *cli_gui_signaler;

/*Mutex for the 'send message' buffer*/
pthread_mutex_t message_send_mutex = PTHREAD_MUTEX_INITIALIZER;

/*Client server communication thread*/
pthread_t* get_client_server_com_thread() {
    return client_server_com_thread;
}

/*Update user current list of rooms available*/
void set_room_list(const char* new_room_list) {
    strcpy(room_list_names, new_room_list);
}

/*Get current list of rooms available*/
char* get_room_list(){
    return room_list_names;
}

/*Clear current list of rooms available. Used when a new room is created, or the user has quit the room.*/
void clear_room_list(){
    strcpy(room_list_names, "\0");
}

/*Update the name list of users available in the current room.*/
void set_user_list(const char* new_user_list) {
    strcpy(user_list_names, new_user_list);
}

/*Get list of users in the current room.*/
char* get_user_list(){
    return user_list_names;
}

/*Clear list of users inside the current room.*/
void clear_user_list(){
    strcpy(user_list_names, "\0");
}

char* get_user_name() {
    return user_name;
}

void set_user_name(const char* new_user_name) {
    strcpy(user_name, new_user_name);
}

char* get_user_room_name() {
    return user_room_name;
}

void set_user_room_name(const char* new_user_room_name) {
    strcpy(user_room_name, new_user_room_name);
}

int get_user_id() {
    return user_id;
}

void set_user_id(int new_id) {
    user_id = new_id;
}

int get_user_room_id() {
    return user_room_id;
}

void set_user_room_id(int new_user_room_id) {
    user_room_id = new_user_room_id;
}

BOOL is_user_joined() {
    return user_is_joined;
}

void set_user_is_joined(BOOL is_joined){
    user_is_joined = is_joined;
}

int get_command_sent() {
    return command_sent;
}

void set_command_sent(int new_command_sent) {
    command_sent = new_command_sent;
}

void clear_command_sent() {
    command_sent = NO_COMMAND;
}

BOOL is_user_connected() {
    return user_is_connected;
}

void set_user_is_connected(BOOL user_connected) {
    user_is_connected = user_connected;
}

/*Inserts a new message to be sent in the buffer*/
void set_message_to_send(Message_t *new_message_to_send) {
    pthread_mutex_lock(&message_send_mutex);
    message_to_send = new_message_to_send;
    set_command_sent(new_message_to_send->command);
    message_waiting_send = TRUE;
    pthread_mutex_unlock(&message_send_mutex);
}

/*Clear the buffer of messages to send.*/
void free_message_to_send() {
    pthread_mutex_lock(&message_send_mutex);
    free(message_to_send);
    message_to_send = NULL;
    message_waiting_send = FALSE;
    pthread_mutex_unlock(&message_send_mutex);
}

Message_t* get_message_to_send() {
    return message_to_send;
}

/*Pre declaration of function*/
void init_user_information(SOCKET client_socket);

/*Function to initiate all client state: socket, threads and main loop.*/
int init_client(Signaler *client_gui_signaler,char server_ip[10], int client_port, int server_port)
{
    /*Sets user signaler to communicate with GUI*/
    cli_gui_signaler = client_gui_signaler;

    struct sockaddr_in  client_address, server_address;

    // Opens TCP socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0))==INVALID_SOCKET)
        errorTreatment(client_socket, OPENSOCK_ERROR);

    // Set client's information address
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = htonl(INADDR_ANY);
    client_address.sin_port = htonl(client_port);

    // Associates local informations with socket
    if ((bind(client_socket, (struct sockaddr *)&client_address, sizeof(client_address))) != 0)
        errorTreatment(client_socket, BIND_ERROR);

    // Set remote server's information address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port = htons(server_port);

    // Connects open client socket with server
    if(connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) != 0)
        errorTreatment(client_socket, CONNECT_ERROR);

    init_user_information(client_socket);

    return 0;
}

/*Receive message from socket and returns it*/
Message_t receive_message() {

    Message_t rcv_msg;
    int bytes_received = 0;
    struct timeb start, end;

    ftime(&start);
    while((bytes_received+=(read(client_socket, &rcv_msg, MESSAGE_SIZE))) < MESSAGE_SIZE)
    {
        ftime(&end);
        if((end.time - start.time) > 5*TIMEOUT)
        {
            /*If timeout has ocurred, probably connection was lost. Close connection and end program.*/
            close(client_socket);
            exit(-1);
        }
    }

    /*Unmarshall message to get the parameters in the right form*/
    unmarshalling_message(&rcv_msg);

    return rcv_msg;
}

/*Process the received message accordingly*/
void process_received_message(Message_t received_message) {

    switch(received_message.command) {
    case USER_CREATED_ROOM_ACK: /*If User created a room, the user will join it.*/
    case USER_JOINED_ACK:       /*If a user joined a room, the user will get here too.*/
        set_user_is_joined(TRUE);
        set_user_room_id(received_message.room_id);
        set_user_room_name(received_message.room_name);
        /*Signals GUI to change its look: now user is in a room!*/
        cli_gui_signaler->set_user_in_room(received_message.room_name);
        /*Clear the last command sent if it is the case*/
        if(get_command_sent() == CREATE_ROOM_COMMAND || get_command_sent() == JOIN_ROOM_COMMAND)
            clear_command_sent();
        break;
    case USER_LEFT_ACK:
        set_user_is_joined(FALSE);
        set_user_room_id(received_message.room_id);
        set_user_room_name(received_message.room_name);
        cli_gui_signaler->pop_user_from_room();
        cli_gui_signaler->update_room_list(get_room_list());
        clear_user_list();
        if(get_command_sent() == LEAVE_ROOM_COMMAND)
            clear_command_sent();
        break;
    case USER_CHANGED_NICKNAME_ACK:
        set_user_name(received_message.user_name);
        cli_gui_signaler->change_user_nickname(received_message.user_name);
        if(get_command_sent() == CHANGE_NICKNAME_COMMAND)
            clear_command_sent();
        break;
    case BROADCAST_MESSAGE_COMMAND:
        cli_gui_signaler->send_gui_output_message(received_message.message_string);
        if(get_command_sent() == MESSAGE_COMMAND && !strcmp(received_message.user_name, get_user_name()))
            clear_command_sent();
        break;
    case BROADCAST_ROOMS_COMMAND:
        if(! is_user_joined())
            cli_gui_signaler->update_room_list(received_message.message_string);
        set_room_list(received_message.message_string);
        break;
    case BROADCAST_USERS_COMMAND:
        if(is_user_joined())
            cli_gui_signaler->update_user_list(received_message.message_string);
        set_user_list(received_message.message_string);
        break;
    case USER_QUIT_ACK:
        set_user_is_connected(false);
        if(get_command_sent() == QUIT_CHAT_COMMAND){
            clear_command_sent();
        }
        break;
    case USER_NICKNAME_ALREADY_EXISTS_ERROR:
        cli_gui_signaler->user_nickname_exists();
        if(get_command_sent() == CHANGE_NICKNAME_COMMAND)
            clear_command_sent();
        break;
    case USER_ROOM_FULL_ERROR:
        cli_gui_signaler->room_full_error();
        if(get_command_sent() == JOIN_ROOM_COMMAND)
            clear_command_sent();
        break;
    case USER_ROOM_SPACE_ERROR:
        cli_gui_signaler->room_space_error();
        if(get_command_sent() == CREATE_ROOM_COMMAND)
            clear_command_sent();
        break;
    case USER_ROOM_NAME_ALREADY_EXISTS_ERROR:
        cli_gui_signaler->room_name_exists_error();
        if(get_command_sent() == CREATE_ROOM_COMMAND)
            clear_command_sent();
        break;
    case USER_ROOM_NOT_FOUND:
        cli_gui_signaler->room_not_found_error();
        if(get_command_sent() == JOIN_ROOM_COMMAND)
            clear_command_sent();
        break;
    }

}

/*Send message to server*/
void send_message() {
    int bytes_sent=0;
    struct timeb start, end;

    /*First must execute the marshalling of the message to avoid problems*/
    marshalling_message(get_message_to_send());

    ftime(&start);
    while((bytes_sent+=(write(client_socket, (const char*)get_message_to_send(), MESSAGE_SIZE))) < MESSAGE_SIZE)
    {
        ftime(&end);
        if((end.time - start.time) > TIMEOUT)
        {
            /*If timeout has ocurred probably connection was lost with the server. Better close socket and end program*/
            printf("Request timed out!\n");
            close(client_socket);
            exit(-1);
        }
    }

    /*Free message buffer because it was just sent*/
    free_message_to_send();
}

/*Main client-server communication function*/
void* client_server_communication(void *args) {
    (void)((void*)args); //suppress unused variable warning (useless line)

    /*Set of file descriptors to read*/
    fd_set read_set;
    /*max value among the read file descriptors*/
    int max_fd;
    /*Initially clear the read set*/
    FD_ZERO(&read_set);

    struct timeval timeout;

    /*While user is still connected to server*/
    while( is_user_connected() ) {
        FD_SET( client_socket, &read_set );
        /*Sets select timeout as 50ms*/
        timeout.tv_sec=0;timeout.tv_usec=50;

        /*Gets highest file descriptor value*/
        max_fd = client_socket;

        /*Checks if there is anything to receive from socket*/
        select( max_fd+1, &read_set,  NULL,  NULL,  &timeout);

        /* If there is anything on the client socket to receive*/
        if( FD_ISSET(client_socket, &read_set) ) {
            Message_t received_message = receive_message();
            process_received_message(received_message);
        }

        /*If 'send message' buffer is full*/
        if(message_waiting_send && is_user_connected()) {
            send_message();
        }
    }
    return NULL;
}


void init_user_information(SOCKET client_socket)
{
    int bytes_received = 0;
    Message_t first_control_message;
    struct timeb start, end;

    ftime(&start);
    while((bytes_received+=(read(client_socket, &first_control_message, MESSAGE_SIZE))) < MESSAGE_SIZE)
    {
        ftime(&end);
        if((end.time - start.time) > TIMEOUT)
        {
            printf("Request timed out!\n");
            close(client_socket);
            exit(-1);
        }
    }

    /*unmarshal message*/
    unmarshalling_message(&first_control_message);

    if(first_control_message.command != INIT_USER_COMMAND)
    {
        printf("Error at initialization step.\n");
        close_client();
        exit(-1);
    }

    set_user_id(first_control_message.user_id);
    set_user_name(first_control_message.user_name);
    set_user_room_id(first_control_message.room_id);
    set_user_room_name(first_control_message.room_name);
    set_user_is_joined(FALSE);
    set_user_is_connected(TRUE);
    clear_room_list();
    clear_room_list();
    clear_command_sent();

    return;
}

/*
 * Send server message requesting to create a room.
 * Server answers postive OR negative (must wait response)
 */
void user_create_room_request(const char *new_room_name) {

    Message_t *create_room_message_req = (Message_t*)malloc(MESSAGE_SIZE);
    char message[MAX_MESSAGESTR_SIZE];
    sprintf(message, "%s %s", create_room_command_str, new_room_name);

    init_message_t(create_room_message_req, CREATE_ROOM_COMMAND, get_user_id(), get_user_room_id(), get_user_room_name(),
                   get_user_name(), message);

    /*fills 'send message' buffer with prepared message*/
    set_message_to_send(create_room_message_req);

}

/*Make user join room request message*/
void user_join_room_request(const char* room) {
    Message_t *join_room_message_req = (Message_t*)malloc(MESSAGE_SIZE);
    char message[MAX_MESSAGESTR_SIZE];
    sprintf(message, "%s %s", join_room_command_str, room);

    init_message_t(join_room_message_req, JOIN_ROOM_COMMAND, get_user_id(), get_user_room_id(), get_user_room_name(),
                   get_user_name(), message);

    /*fills 'send message' buffer with prepared message*/
    set_message_to_send(join_room_message_req);
}

/*Make user leave room request message*/
void user_leave_room_request() {

    Message_t *leave_room_message_req = (Message_t*)malloc(MESSAGE_SIZE);
    char message[MAX_MESSAGESTR_SIZE];
    sprintf(message, "%s %s", leave_room_command_str, get_user_room_name());

    init_message_t(leave_room_message_req, LEAVE_ROOM_COMMAND, get_user_id(), get_user_room_id(), get_user_room_name(),
                   get_user_name(), message);

    /*fills 'send message' buffer with prepared message*/
    set_message_to_send(leave_room_message_req);
}

/*Make user change nickname request message*/
void user_change_nickname_request(const char *new_user_nickname) {

    Message_t *change_nickname_message_req = (Message_t*)malloc(MESSAGE_SIZE);
    char message[MAX_MESSAGESTR_SIZE];
    sprintf(message, "%s %s", change_nickname_command_str, new_user_nickname);

    init_message_t(change_nickname_message_req, CHANGE_NICKNAME_COMMAND, get_user_id(), get_user_room_id(), get_user_room_name(),
                   get_user_name(), message);

    /*fills 'send message' buffer with prepared message*/
    set_message_to_send(change_nickname_message_req);
}

/*Make user quit chat request message*/
void user_quit_request() {
    Message_t *quit_chat_message_req = (Message_t*)malloc(MESSAGE_SIZE);
    char message[MAX_MESSAGESTR_SIZE];
    sprintf(message, "%s", quit_chat_command_str);

    init_message_t(quit_chat_message_req, QUIT_CHAT_COMMAND, get_user_id(), get_user_room_id(), get_user_room_name(),
                   get_user_name(), message);

    /*fills 'send message' buffer with prepared message*/
    set_message_to_send(quit_chat_message_req);
}

/*Make user send message request*/
void user_message_request(const char* message) {
    Message_t *message_req = (Message_t*)malloc(MESSAGE_SIZE);

    init_message_t(message_req, MESSAGE_COMMAND, get_user_id(), get_user_room_id(), get_user_room_name(),
                   get_user_name(), message);

    /*fills 'send message' buffer with prepared message*/
    set_message_to_send(message_req);
}

/*close client socket, making sure no information is left*/
void close_client() {
    close(client_socket);
    strcpy(user_name, "");
    strcpy(user_room_name, "");
    user_room_id = -1;
    user_is_joined = FALSE;
    user_is_connected = FALSE;
    user_id = -1;
}

