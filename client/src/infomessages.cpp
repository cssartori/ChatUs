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

#include "../include/infomessages.h"

InfoMessages::InfoMessages(QWidget *parent)
{
    this->parent=parent;
}

/*
 * Request timed out message.
*/
void InfoMessages::requestTimeOutMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, REQUEST_TIME_OUT_ERROR_TITLE, REQUEST_TIME_OUT_ERROR_TXT);
}

/*
 * No message input trying to be sent.
*/
void InfoMessages::noInputMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, NO_INPUT_DATA_ERROR_TITLE, NO_INPUT_DATA_ERROR_TXT);
}

/*
 * Message contains invalid characters.
*/
void InfoMessages::invalidCharMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, INVALID_CHARACTERS_ERROR_TITLE, INVALID_CHARACTERS_ERROR_TXT);
}

/*
 * User name already exists.
*/
void InfoMessages::nameExistsMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, NAME_EXISTS_ERROR_TITLE, NAME_EXISTS_ERROR_TXT);
}

/*
 * Room is full.
*/
void InfoMessages::roomFullMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, ROOM_FULL_ERROR_TITLE, ROOM_FULL_ERROR_TXT);
}

/*
 * Room has no space (similar to run is full).
*/
void InfoMessages::roomSpaceMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, ROOM_SPACE_ERROR_TITLE, ROOM_SPACE_ERROR_TXT);
}

/*
 * Room name already exists.
*/
void InfoMessages::roomNameExistsMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, ROOM_NAME_EXISTS_ERROR_TITLE, ROOM_NAME_EXISTS_ERROR_TXT);
}

/*
 * Room could not be found.
*/
void InfoMessages::roomNotFoundMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, ROOM_NOT_FOUND_ERROR_TITLE, ROOM_NOT_FOUND_ERROR_TXT);
}

/*
 * Name could not be changed.
*/
void InfoMessages::changeNameMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, CHANGE_NAME_ERROR_TITLE, CHANGE_NAME_ERROR_TXT);
}

/*
 * Dialog saved successfully.
*/
void InfoMessages::savedDialogMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, DIALOG_SAVED_TITLE, DIALOG_SAVED_TXT);
}

/*
 * Dialog was not saved successfully
*/
void InfoMessages::dialogFileErrorMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, DIALOG_FILE_ERROR_TITLE, DIALOG_FILE_ERROR_TXT);
}

/*
 * Room could not be created.
*/
void InfoMessages::createRoomErrorMsg()
{
    QMessageBox msgBox;
    msgBox.critical(this->parent, CREATE_ROOM_ERROR_TITLE, CREATE_ROOM_ERROR_TXT);
}
