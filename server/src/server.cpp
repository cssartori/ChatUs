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
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/server.h"
#include "../include/room_t.h"
#include "../include/user_t.h"
#include "../../protocol/include/message_t.h"
#include "../../protocol/include/constants.h"
#include "../../protocol/include/protocol.h"
#include <sys/timeb.h>

/*ALL users constant defined to use in broadcasts*/
const int ALL_USERS = MAX_USERS_CONNECTED+1;

/*Struct to pass as parameter to client threads*/
typedef struct Client_parameters
{
    int socket;
    int id;
} Client_parameters;

/*Keeps client threads structures*/
pthread_t *client_threads[MAX_USERS_CONNECTED] = {NULL};
/*Keeps user structures*/
User_t *users_connected[MAX_USERS_CONNECTED] = {NULL};
/*Keeps rooms structures*/
Room_t *rooms_connected[MAX_ROOMS_CONNECTED] = {NULL};

/*To protect the client socket variable*/
pthread_mutex_t mutex_client_socket = PTHREAD_MUTEX_INITIALIZER;

/*To protect room creation (only one user can create room at a time)*/
pthread_mutex_t mutex_create_room = PTHREAD_MUTEX_INITIALIZER;

/*To protect join room actions (two users joining at the same time could overflow room capacity)*/
pthread_mutex_t mutex_join_room[MAX_ROOMS_CONNECTED];

/*Two users changing nickname to the same name could be a problem*/
pthread_mutex_t mutex_change_nickname = PTHREAD_MUTEX_INITIALIZER;

/*To allow only one thread at a time modify the counter variables*/
pthread_mutex_t mutex_user_counter = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_room_counter = PTHREAD_MUTEX_INITIALIZER;

/*Keep track of number of users and rooms connected to the server: have no pratical use*/
int number_users_con = 0;
int number_rooms_con = 0;

/*pre definitions of server functions*/
int user_initialization(SOCKET client_socket, int user_id);
int user_create_room(const char* new_room_name);
int get_free_user_id();
int get_room_id(const char *room_name);
int user_change_nickname(int user_id, const char *new_user_nickname);
int user_join_room(int room_id, int user_id);
int user_leave_room(int room_id, int user_id);
int user_quit(int user_id);
int user_broadcast_message_room(int user_id, const char* message);
int broadcast_room_list(int user_id);
int broadcast_user_list_room(int room_id);

/*Functions to increment and decrement user/rooms counter*/
void increment_users_counter(){
    pthread_mutex_lock(&mutex_user_counter);
    number_users_con+=1;
    pthread_mutex_unlock(&mutex_user_counter);
}

void decrement_users_counter(){
    pthread_mutex_lock(&mutex_user_counter);
    number_users_con-=1;
    pthread_mutex_unlock(&mutex_user_counter);
}

void increment_rooms_counter(){
    pthread_mutex_lock(&mutex_room_counter);
    number_rooms_con+=1;
    pthread_mutex_unlock(&mutex_room_counter);
}

void decrement_rooms_counter(){
    pthread_mutex_lock(&mutex_room_counter);
    number_rooms_con-=1;
    pthread_mutex_unlock(&mutex_room_counter);
}

/*============================================================================================*/
/*The following functions create a response message to send to the user*/
Message_t make_room_created_response_message(int user_id) {
    Message_t room_created_response;
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "%s %s", user_created_room_ack_str, rooms_connected[users_connected[user_id]->room_id]->room_name);
    init_message_t(&room_created_response, USER_CREATED_ROOM_ACK, user_id, users_connected[user_id]->room_id, rooms_connected[users_connected[user_id]->room_id]->room_name, users_connected[user_id]->user_name, message_str);
    return room_created_response;
}

Message_t make_user_joined_response_message(int user_id) {
    Message_t user_joined_response;
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "%s %s", user_joined_ack_str, rooms_connected[users_connected[user_id]->room_id]->room_name);
    init_message_t(&user_joined_response, USER_JOINED_ACK, user_id, users_connected[user_id]->room_id, rooms_connected[users_connected[user_id]->room_id]->room_name, users_connected[user_id]->user_name, message_str);
    return user_joined_response;
}

Message_t make_user_unjoined_response_message(int user_id) {
    Message_t user_unjoined_response;
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "%s", user_left_ack_str);
    init_message_t(&user_unjoined_response, USER_LEFT_ACK, user_id, ROOT_ROOM_ID, root_room_name, users_connected[user_id]->user_name, message_str);
    return user_unjoined_response;
}

Message_t make_nickname_changed_response_message(int user_id) {
    Message_t nickname_changed_response;
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "%s %s", user_changed_nickname_ack_str, users_connected[user_id]->user_name);
    init_message_t(&nickname_changed_response, USER_CHANGED_NICKNAME_ACK, user_id, users_connected[user_id]->room_id, root_room_name, users_connected[user_id]->user_name, message_str);
    return nickname_changed_response;
}

Message_t make_nickname_already_exists_response_message(int user_id) {
    Message_t nickname_error_response;
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "%s %s", change_nickname_command_str, users_connected[user_id]->user_name);
    init_message_t(&nickname_error_response, USER_NICKNAME_ALREADY_EXISTS_ERROR, user_id, users_connected[user_id]->room_id, rooms_connected[users_connected[user_id]->room_id]->room_name, users_connected[user_id]->user_name, message_str);
    return nickname_error_response;
}

Message_t make_room_name_already_exists_response_message(int user_id) {
    Message_t room_name_error_response;
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "%s %s", create_room_command_str, users_connected[user_id]->user_name);
    init_message_t(&room_name_error_response, USER_ROOM_NAME_ALREADY_EXISTS_ERROR, user_id, users_connected[user_id]->room_id, root_room_name, users_connected[user_id]->user_name, message_str);
    return room_name_error_response;
}

Message_t make_room_space_response_message(int user_id) {
    Message_t room_space_error_response;
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "%s %s", create_room_command_str, users_connected[user_id]->user_name);
    init_message_t(&room_space_error_response, USER_ROOM_SPACE_ERROR, user_id, users_connected[user_id]->room_id, root_room_name, users_connected[user_id]->user_name, message_str);
    return room_space_error_response;
}

Message_t make_room_full_response_message(int user_id) {
    Message_t room_full_error_response;
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "%s %s", join_room_command_str, users_connected[user_id]->user_name);
    init_message_t(&room_full_error_response, USER_ROOM_FULL_ERROR, user_id, users_connected[user_id]->room_id, root_room_name, users_connected[user_id]->user_name, message_str);
    return room_full_error_response;
}

Message_t make_room_not_found_response_message(int user_id) {
    Message_t room_not_found_error_response;
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "%s %s", join_room_command_str, users_connected[user_id]->user_name);
    init_message_t(&room_not_found_error_response, USER_ROOM_NOT_FOUND, user_id, users_connected[user_id]->room_id, root_room_name, users_connected[user_id]->user_name, message_str);
    return room_not_found_error_response;
}
/*End of response functions*/
/*============================================================================================*/

/*Process a given user request*/
int process_user_request(Message_t user_message_req) {
    int process_result;
    int room_id;
    char *parameter = NULL;

    switch (user_message_req.command)
    {
    case MESSAGE_COMMAND:
        /*Common message, must broadcast it to room where sender is connected*/
        pthread_mutex_lock(&mutex_join_room[user_message_req.room_id]);
        process_result = user_broadcast_message_room(user_message_req.user_id, user_message_req.message_string);
        pthread_mutex_unlock(&mutex_join_room[user_message_req.room_id]);
        break;
    case CREATE_ROOM_COMMAND:
        pthread_mutex_lock(&mutex_create_room);
        /*Get room_name from user_message*/
        parameter = extract_command_parameter(user_message_req.command, user_message_req.message_string);
        process_result = user_create_room(parameter);
        room_id = get_room_id(parameter);
        user_join_room(room_id, user_message_req.user_id);
        pthread_mutex_unlock(&mutex_create_room);
        break;
    case JOIN_ROOM_COMMAND:
        parameter = extract_command_parameter(user_message_req.command, user_message_req.message_string);
        room_id = get_room_id(parameter);
        if(room_id < 0){
            process_result = ROOM_NOT_FOUND_ERROR;
            break;
        }
        pthread_mutex_lock(&mutex_join_room[room_id]);
        process_result = user_join_room(room_id, user_message_req.user_id);
        pthread_mutex_unlock(&mutex_join_room[room_id]);
        break;
    case LEAVE_ROOM_COMMAND:
        pthread_mutex_lock(&mutex_join_room[user_message_req.room_id]);
        process_result=user_leave_room(users_connected[user_message_req.user_id]->room_id, user_message_req.user_id);
        pthread_mutex_unlock(&mutex_join_room[user_message_req.room_id]);
        break;
    case CHANGE_NICKNAME_COMMAND:
        pthread_mutex_lock(&mutex_change_nickname);
        parameter = extract_command_parameter(user_message_req.command, user_message_req.message_string);
        process_result = user_change_nickname(user_message_req.user_id, parameter);
        pthread_mutex_unlock(&mutex_change_nickname);
        break;
    case QUIT_CHAT_COMMAND:
        process_result = user_quit(user_message_req.user_id);
        break;
    default:
        /*Invalid command thing*/
        process_result = INVALID_COMMAND;
    }

    if(parameter != NULL) {
        free(parameter);
        parameter = NULL;
    }

    return process_result;

}

void send_user_response(int process_result, int user_id, int old_user_room) {
    /*Prepare response message to send*/
    Message_t response_message;
    BOOL response = TRUE;
    switch(process_result) {
    case ROOM_CREATED:
        response_message = make_room_created_response_message(user_id);
        break;
    case USER_JOIN_SUCC:
        response_message = make_user_joined_response_message(user_id);
        break;
    case USER_UNJOIN_SUCC:
        response_message = make_user_unjoined_response_message(user_id);
        break;
    case USER_NICKNAME_CHANGED:
        response_message = make_nickname_changed_response_message(user_id);
        break;
    case NICKNAME_EXISTS_ERROR:
        response_message = make_nickname_already_exists_response_message(user_id);
        break;
    case ROOM_NAME_EXISTS_ERROR:
        response_message = make_room_name_already_exists_response_message(user_id);
        break;
    case ROOM_SPACE_ERROR: /*cant create more rooms*/
        response_message = make_room_space_response_message(user_id);
        break;
    case ROOM_FULL_ERROR:
        response_message = make_room_full_response_message(user_id);
        break;
    case ROOM_NOT_FOUND_ERROR:
        response_message = make_room_not_found_response_message(user_id);
        break;
    default:
        response = FALSE;
    }

    if(response) {
        int bytes_sent = 0;
        marshalling_message(&response_message);
        while((bytes_sent += write(users_connected[user_id]->user_socket, (const char*)&response_message, MESSAGE_SIZE)) < MESSAGE_SIZE);
    }
    if(process_result == ROOM_CREATED) {
        broadcast_room_list(ALL_USERS);
        broadcast_user_list_room(users_connected[user_id]->room_id);
    }
    else if(process_result == USER_JOIN_SUCC) {
        broadcast_user_list_room(users_connected[user_id]->room_id);
    }
    else if(process_result == USER_UNJOIN_SUCC) {
        broadcast_user_list_room(old_user_room);
    }
    else if(process_result == ROOM_NOT_FOUND_ERROR) {
        broadcast_room_list(ALL_USERS);
    }

}

/*Server-client communication main function*/
void* client_communication(void *arg)
{
    SOCKET client_socket = ((Client_parameters*)arg)->socket;
    int user_id = ((Client_parameters*)arg)->id;

    pthread_mutex_unlock(&mutex_client_socket);

    user_initialization(client_socket, user_id);
    /*When a users enters the chat, broadcast room list to him, so he knows all rooms available*/
    broadcast_room_list(user_id);

    Message_t user_message;
    BOOL quit = FALSE;

    /*While user is still connected to server*/
    while (! quit)
    {
        int bytes_received =0;
        /*Wait for incoming message from user*/
        while ((bytes_received +=(read (client_socket, &user_message, MESSAGE_SIZE))) < MESSAGE_SIZE);

        /*Unmarshall message to process it accordingly*/
        unmarshalling_message(&user_message);

        int process_result;
        /*Keeps user old room in case it is leaving the room, so that server can broadcast the updated
         * user list for that room*/
        int old_user_room = users_connected[user_message.user_id]->room_id;
        /*Process user request*/
        process_result = process_user_request(user_message);

        /*If process result is for user quit, break loop*/
        if(process_result == USER_QUIT_SUCC)
            break;
        /*Send any response message according to the process result*/
        send_user_response(process_result, user_message.user_id, old_user_room);

    }

    /*Closes client socket and finishes any connection remaining with that user*/
    close(client_socket);

}


/*Server main Function*/
int main(int argc, char **argv)
{

    //Must enter port number parameter
    if(argc < 3)
    {
        printf("\tUse:\n\t%s -s <port_number>\n", argv[0]);
        exit(1);
    }

    int server_port;

    //Check parameters
    if(argv[1][0]=='-')
    {
        switch(argv[1][1])
        {
        case 's': // gets server port
            server_port = atoi(argv[2]);
            if(server_port < 1024)
            {
                printf("Invalid server port value. Must be higher than 1023.\n");
                exit(1);
            }
            break;
        default:
            printf("Invalid parameter: %s\n", argv[1]);
            exit(1);
        }
    }
    else
    {
        printf("Invalid parameter: %s\n", argv[1]);
        exit(1);
    }


    SOCKET client_socket;
    SOCKET server_socket;

    //Creates socket of AF_INET family (Internet) using TCP protocol (SOCK_STREAM)
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0))==INVALID_SOCKET)
        errorTreatment(server_socket, OPENSOCK_ERROR);

    struct sockaddr_in  server_address, client_address;
    unsigned int client_address_size = sizeof(client_address);

    // Defines domain, IP and port to receive data
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // can receive from any IP
    server_address.sin_port = htons(server_port);

    // Associates Server Socket with Server address
    if ((bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))) != 0)
        errorTreatment(server_socket, BIND_ERROR);

    // Server's Socket listen to external connections
    if((listen(server_socket, MAX_USERS_CONNECTED)) != 0) // allows up to MAX_USERS_CONNECTED simultaneous connections
        errorTreatment(server_socket, LISTEN_ERROR);

    //initializes mutex variable to protect client socket variable
    pthread_mutex_init(&mutex_client_socket, NULL);
    int i=0;
    for(;i<MAX_ROOMS_CONNECTED;i++) {
        pthread_mutex_init(&mutex_join_room[i], NULL);
    }
    int new_user_id;
    Client_parameters c_param;

    printf("Server Initialized successfully.......100\nRunning...\n");
    while(TRUE)
    {
        //Allows clients to connect with server
        if( ( client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size) ) < 0)
            errorTreatment(server_socket, ACCEPT_ERROR);

        /*Only one client can be modifiyng the c_param variable at a time*/
        pthread_mutex_lock(&mutex_client_socket);

        new_user_id = get_free_user_id();

        c_param.id = new_user_id;
        c_param.socket = client_socket;
        users_connected[new_user_id] = (User_t*)malloc(USER_SIZE);

        if(pthread_create(client_threads[new_user_id], NULL, client_communication, &c_param) != 0)
        {
            /*In case of error on thread creation,
             * free user space and unlock mutex (which otherwise would only be freed on thread*/
            pthread_mutex_unlock(&mutex_client_socket);
            free(users_connected[new_user_id]);
            users_connected[new_user_id] = NULL;
        }
    }


    close(server_socket);
    exit(1);

}

/*
   Try to find a free space to connect client.
   If the server is full (number_users_connected == MAX_USERS_CONNECTED) this functions returns -1
*/
int get_free_user_id()
{
    int i;
    for(i=0; i<MAX_USERS_CONNECTED; i++)
    {
        if(client_threads[i] == NULL && users_connected[i] == NULL)
        {
            client_threads[i] = (pthread_t*)malloc(sizeof(pthread_t));
            return i;
        }
    }

    return -1;
}

/*Tries to get a free room id: used in create room commands*/
int get_free_room_id()
{

    int i;
    for(i=0; i<MAX_ROOMS_CONNECTED; i++)
    {
        if(rooms_connected[i] == NULL)
        {
            rooms_connected[i] = (Room_t*)malloc(sizeof(Room_t));
            return i;
        }
    }

    return -1;
}

/*Checks if a given room name already exists in server*/
BOOL room_name_exist(const char* room_name)
{
    int i=0;
    for(; i<MAX_ROOMS_CONNECTED; i++)
    {
        if(rooms_connected[i] != NULL)
        {
            if(strcmp(room_name, rooms_connected[i]->room_name) == 0)
                return TRUE;
        }
    }

    return FALSE;
}

/*CHecks if a given user name already exists in server*/
BOOL user_name_exist(const char* user_name)
{
    int i=0;
    for(; i<MAX_USERS_CONNECTED; i++)
    {
        if(users_connected[i] != NULL)
        {
            if(strcmp(user_name, users_connected[i]->user_name) == 0)
                return TRUE;
        }
    }

    return FALSE;
}

/*Given a room name, returns its ID in server: used for join operations and such*/
int get_room_id(const char* room_name)
{
    int i=0;
    for(;i<MAX_ROOMS_CONNECTED;i++)
    {
        if(rooms_connected[i] != NULL){
            if(strcmp(rooms_connected[i]->room_name, room_name) == 0)
                return rooms_connected[i]->room_id;
        }
    }

    return ROOT_ROOM_ID;
}



/*
    Send basic information to client. Makes sure everything is working correctly.
*/
int user_initialization(SOCKET client_socket, int user_id)
{
    /*A user can't change his nickname while the server searches for a free name to the new incoming user
     * both names could be the same*/
    pthread_mutex_lock(&mutex_change_nickname);

    char user_name[MAX_USERNAME_SIZE];
    /*Basic starting name is 'Guest_<user_id>'*/
    sprintf(user_name, "Guest_%i", user_id);
    int name_modifier = MAX_USERS_CONNECTED;

    /*If, for some reason, the basic starting name is already taken, the server will keep trying until find
     * another name that is free*/
    while(user_name_exist(user_name)) {
        sprintf(user_name, "Guest_%i", user_id+name_modifier);
        name_modifier++;
    }

    init_user_t(users_connected[user_id], user_id, client_socket, user_name);

    /*After user is correctly initialized in server (and given an available name) can unlock mutex*/
    pthread_mutex_unlock(&mutex_change_nickname);

    /*Exchange first messages between server and client to confirm connection and give client his basic
     * informations (such as ID, name, and room ID)*/
    Message_t first_control_message;
    char first_message_str[MAX_MESSAGESTR_SIZE];
    sprintf(first_message_str, "%s", user_connection_confirmed_ack_str);

    init_message_t(&first_control_message, INIT_USER_COMMAND, user_id, ROOT_ROOM_ID, root_room_name, user_name, first_message_str);

    marshalling_message(&first_control_message);

    int bytes_sent = 0;
    struct timeb start, end;
    ftime(&start);

    /*Sends user first message*/
    while((bytes_sent +=(write(client_socket, (const char *)&first_control_message, MESSAGE_SIZE))) < MESSAGE_SIZE)
    {
        ftime(&end);
        if((end.time - start.time) > TIMEOUT)
        {
            printf("Request timed out!\n");
            close(client_socket);
            free(users_connected[user_id]);
            users_connected[user_id] = NULL;
            return USER_CREATION_ERROR;
        }
    }

    /*Set connect to server user flag*/
    connect_user_t(users_connected[user_id]);

    /*Increment number of users connected to server*/
    increment_users_counter();

    return USER_CREATED;
}

/*Function to create a room in the server*/
int user_create_room(const char* new_room_name)
{
    if(room_name_exist(new_room_name))
        /*Can't create room with same name as another*/
        return ROOM_NAME_EXISTS_ERROR;

    int new_room_id = get_free_room_id();

    if(new_room_id < 0)
    {
        /*No more space to create room. Must be informed.*/
        pthread_mutex_unlock(&mutex_create_room);
        return ROOM_SPACE_ERROR;
    }

    init_room_t(rooms_connected[new_room_id], new_room_name, new_room_id);

    increment_rooms_counter();

    return ROOM_CREATED;
}

/*Make a user join the room*/
int user_join_room(int room_id, int user_id)
{
    if(users_connected[user_id]->is_joined)
        return USER_JOIN_ERROR;

    if(join_user_room_t(rooms_connected[room_id], users_connected[user_id])==ROOM_FULL)
        return ROOM_FULL_ERROR;

    return USER_JOIN_SUCC;
}

/*Broadcast a message in a room*/
int user_broadcast_message_room(int user_id, const char* message)
{
    char message_str[MAX_MESSAGESTR_SIZE];
    sprintf(message_str, "[ %s ]: %s", users_connected[user_id]->user_name, message);
    Message_t broadcast_message;
    init_message_t(&broadcast_message, BROADCAST_MESSAGE_COMMAND, user_id, users_connected[user_id]->room_id, rooms_connected[users_connected[user_id]->room_id]->room_name, users_connected[user_id]->user_name, message_str);
    int i=0;

    marshalling_message(&broadcast_message);
    for(;i<MAX_USERS_ROOM;i++)
    {
        if(rooms_connected[users_connected[user_id]->room_id]->users_in_room[i] != NULL)
        {
            int bytes_sent=0;
            while((bytes_sent+=(write(rooms_connected[users_connected[user_id]->room_id]->users_in_room[i]->user_socket, (const char*)&broadcast_message, MESSAGE_SIZE))) < MESSAGE_SIZE);
        }
    }

    return USER_MESSAGE_BROADCASTED;
}

/*User leaves room*/
int user_leave_room(int room_id, int user_id)
{
    /*First get him out of the room*/
    if(unjoin_user_room_t(rooms_connected[room_id], users_connected[user_id]) == USER_NOT_FOUND)
    {
        /*In case user was not found in that room, something went wrong*/
        return USER_UNJOIN_ERROR;
    }

    /*If the number of users in the room from where the user left is equals 0
     * the room must be destroyed (rooms will only keep in the server while having 1 or more users joined)*/
    if(rooms_connected[room_id]->number_users == 0){
        /*Destroy room*/
       destroy_room_t(rooms_connected[room_id]);
       decrement_rooms_counter();
       rooms_connected[room_id] = NULL;
       /*Broadcast the updated list of rooms to all users in the server (without the destroyed room)*/
       broadcast_room_list(ALL_USERS);
    }

    return USER_UNJOIN_SUCC;
}

int user_change_nickname(int user_id, const char* new_user_nickname)
{
    if(!user_name_exist(new_user_nickname))
    {
        /*if no other user is using the name, it can be changed*/
        set_name_user_t(users_connected[user_id], new_user_nickname);
        return USER_NICKNAME_CHANGED;
    }

    return NICKNAME_EXISTS_ERROR;
}

/*Function that broadcasts room list to a specific user, or all users if ALL_USERS is passed as parameter*/
int broadcast_room_list(int user_id) {
    char room_list[MAX_MESSAGESTR_SIZE]="$";
    int i=0;

    for(;i<MAX_ROOMS_CONNECTED;i++) {
        if(rooms_connected[i] != NULL)
            sprintf(room_list, "%s%s$", room_list, rooms_connected[i]->room_name);
    }

    Message_t broadcast_rooms_msg;
    init_message_t(&broadcast_rooms_msg, BROADCAST_ROOMS_COMMAND, user_id, ROOT_ROOM_ID, root_room_name, "", room_list);

    marshalling_message(&broadcast_rooms_msg);

    if(user_id == ALL_USERS) {
        /*broadcast to all users connected*/
        i=0;
        for(;i<MAX_USERS_CONNECTED;i++) {
            if(users_connected[i] != NULL) {
                int bytes_sent = 0;
                while((bytes_sent+=(write(users_connected[i]->user_socket, (const char*)&broadcast_rooms_msg, MESSAGE_SIZE))) < MESSAGE_SIZE);
            }
        }

    }
    else {
        /*Broadcast to single user*/
        int bytes_sent=0;
        while((bytes_sent+=(write(users_connected[user_id]->user_socket, (const char*)&broadcast_rooms_msg, MESSAGE_SIZE))) < MESSAGE_SIZE);

    }

    return ROOMS_BROADCASTED;

}

/*Function that broadcasts the list of users joined in a room to all the users in that room
 * so that they all know who is in that room*/
int broadcast_user_list_room(int room_id) {

    if(room_id < 0 || rooms_connected[room_id] == NULL)
        return USER_LIST_ROOM_BROADCASTED;

    char user_list_room[MAX_MESSAGESTR_SIZE]="$";

    int i=0;

    for(;i<MAX_USERS_ROOM;i++) {
        if(rooms_connected[room_id]->users_in_room[i] != NULL)
            sprintf(user_list_room, "%s%s$", user_list_room, rooms_connected[room_id]->users_in_room[i]->user_name);
    }


    Message_t broadcast_user_list_rooms_msg;
    init_message_t(&broadcast_user_list_rooms_msg, BROADCAST_USERS_COMMAND, ALL_USERS, room_id, rooms_connected[room_id]->room_name, "", user_list_room);

    marshalling_message(&broadcast_user_list_rooms_msg);

    /*broadcast to all users joined in the room*/
    i=0;
    for(;i<MAX_USERS_CONNECTED;i++) {
        if(users_connected[i] != NULL && users_connected[i]->room_id == room_id) {
            int bytes_sent = 0;
            while((bytes_sent+=(write(users_connected[i]->user_socket, (const char*)&broadcast_user_list_rooms_msg, MESSAGE_SIZE))) < MESSAGE_SIZE);
        }
    }

    return USER_LIST_ROOM_BROADCASTED;
}

/*A user is requesting to quit the server*/
int user_quit(int user_id)
{
    /*If a user is joined to a given room, he will leave it, so that no traces of him are kept by the server*/
    if(users_connected[user_id]->is_joined) {
    	pthread_mutex_lock(&mutex_join_room[users_connected[user_id]->room_id]);
        user_leave_room(users_connected[user_id]->room_id, user_id);
        pthread_mutex_unlock(&mutex_join_room[users_connected[user_id]->room_id]);
    }

    Message_t quit_m;
    char message[MAX_MESSAGESTR_SIZE];
    sprintf(message, "%s", user_quit_ack_str);
    init_message_t(&quit_m, USER_QUIT_ACK, user_id, ROOT_ROOM_ID, root_room_name, users_connected[user_id]->user_name, message);
    int bytes_sent=0;
    struct timeb start, end;
    ftime(&start);
    marshalling_message(&quit_m);

    /*Sends user an ACK message of his quit request, confirming everything is okay for him to quit*/
    while((bytes_sent+=(write(users_connected[user_id]->user_socket, (const char*)&quit_m, MESSAGE_SIZE))) < MESSAGE_SIZE)
    {
        ftime(&end);
        if((end.time - start.time) > TIMEOUT)
        {
           printf("Requeste timeout to user %i. Closing connection.\n", user_id);
           break;
        }
    }

    /*Free user variables*/
    free(users_connected[user_id]);
    users_connected[user_id] = NULL;
    client_threads[user_id] = NULL;

    decrement_users_counter();

    return USER_QUIT_SUCC;
}


