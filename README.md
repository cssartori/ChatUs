# **ChatUs!: A chat client and server in C** #

*ChatUs!* is a simple chat program coded in C for UNIX systems. It has an easy-to-use Graphical User Interface (GUI), allowing the user to forget command lines and to focus on the messages. With *ChatUs!*, one can:

* Create and Join Rooms with multiple users;
* Exchange text messages with all the users in a room;
* Have a custom personal name;
* Freedom to connect to whatever server there is available (as long as one knows it's IP address and port).

## **Usage** ##
The program has two executables: the *client* and the *server* side. A common user would only run the Client side, connecting to an already running Server side. However, it is very simple to run the Server in a central computer on your LAN and then connect other devices all together through it. 

Considering that *client* is the name of the Client program, one can run it by typing:

```
$ client -h <server_ip> -s <server_port> -p <client_port> 
```
The server_ip and server_port are defined in the server initiation. The client_port is any valid port number > 1024.

Alternatively, one can run the Server by typing:

```
$ server -s <server_port> 
```
Where the server_port is also any valid port number > 1024.

## **Executables** ##
If you don't want to bother in compiling the code, there is a [link](https://bitbucket.org/cssartori/chatus/downloads/Exec.zip) with both Client and Server side compiled. The compilation was done in a Ubuntu 64-bits, 14.04.10.

## **Implementation** ##

The Client side is a project for Qt Creator, which allowed the easy creation of a GUI in C++ and the connection to pure C code for the thread and lock manipulations. Qt Creator with Major version 4 is needed for compilation.

The Server side is a simple C code, with no GUI as there is no need for it. No manipulation or commands can be given in the server, all it is supposed to do is manage the commands send by users and accept connection requests. One can compile the Server side with a Makefile located inside the corresponding folder.

Both Client and Server sides have a common *Protocol* to be followed, with a standard Message structure and command codes. All this information is defined under the folder *Protocol*.

## **About** ##
The project was done for the INF01151-Operating Systems II, 2015, at UFRGS.

The core was written in C because it was a special request in the project.