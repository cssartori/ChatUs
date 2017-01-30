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

#ifndef INFOMESSAGES_H
#define INFOMESSAGES_H

#include <QMessageBox>


class InfoMessages
{
public:
    InfoMessages(QWidget *parent);
    void requestTimeOutMsg();
    void noInputMsg();
    void invalidCharMsg();
    void nameExistsMsg();
    void roomFullMsg();
    void roomSpaceMsg();
    void roomNameExistsMsg();
    void roomNotFoundMsg();
    void changeNameMsg();
    void savedDialogMsg();
    void dialogFileErrorMsg();
    void createRoomErrorMsg();

private:
    QWidget *parent;

    /*
     *TITLES
     **/
    const QString REQUEST_TIME_OUT_ERROR_TITLE = "Request timed out!";
    const QString NO_INPUT_DATA_ERROR_TITLE = "No input!";
    const QString INVALID_CHARACTERS_ERROR_TITLE = "Invalid Character used!";
    const QString NAME_EXISTS_ERROR_TITLE = "Name already exists!";
    const QString ROOM_FULL_ERROR_TITLE = "Room full!";
    const QString ROOM_SPACE_ERROR_TITLE = "No room space!";
    const QString ROOM_NAME_EXISTS_ERROR_TITLE = "Room name exists!";
    const QString ROOM_NOT_FOUND_ERROR_TITLE = "Room not found!";
    const QString CHANGE_NAME_ERROR_TITLE = "Can not change name!";
    const QString DIALOG_SAVED_TITLE = "Dialog saved!";
    const QString DIALOG_FILE_ERROR_TITLE = "Error creating file!";
    const QString CREATE_ROOM_ERROR_TITLE = "Error creating room!";

    /*
     *TEXTS
     **/
    const QString REQUEST_TIME_OUT_ERROR_TXT = "Request timed out.\nPlease make sure you are connected to the internet and try again.";
    const QString NO_INPUT_DATA_ERROR_TXT = "No input string.\nPlease insert new name and try again.\n";
    const QString INVALID_CHARACTERS_ERROR_TXT = "Invalid character used.\nPlease make sure no forbbiden characters are used and try again.";
    const QString NAME_EXISTS_ERROR_TXT = "Name already exists. Please try another one.\n";
    const QString ROOM_FULL_ERROR_TXT = "Room is full. Wait untill someone leaves the room.\n";
    const QString ROOM_SPACE_ERROR_TXT = "There is no more space available for room creation.\n";
    const QString ROOM_NAME_EXISTS_ERROR_TXT = "Room name already in use. Please try another one.\n";
    const QString ROOM_NOT_FOUND_ERROR_TXT = "Room does not exist. Please try another room.\n";
    const QString CHANGE_NAME_ERROR_TXT = "You can't change your nickname while joined in a room.\nPlease leave and try again.";
    const QString DIALOG_SAVED_TXT = "Dialog messages saved successfully.";
    const QString DIALOG_FILE_ERROR_TXT = "Error trying to create Dialog Messages file.\nPlease try again.";
    const QString CREATE_ROOM_ERROR_TXT = "You can not create a room while in a room.\nPlease leave the room and try again.";
};

#endif // INFOMESSAGES_H

