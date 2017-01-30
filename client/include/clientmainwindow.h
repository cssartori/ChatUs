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

#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>

namespace Ui {
class ClientMainWindow;
}


class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientMainWindow(QWidget *parent = 0);
    ~ClientMainWindow();


public slots:
    void append_output_message(const char *message);
    void set_user_name(const char *userName);
    void set_user_in_room(const char* room_name);
    void pop_user_from_room();
    void update_room_list(const char* room_list);
    void user_name_exists_error();
    void room_full_error();
    void room_space_error();
    void room_name_exists_error();
    void room_not_found_error();
    void save_room_dialog();




private slots:
    void on_createRoomBtn_clicked();

    void on_leaveBtn_clicked();

    void on_editNameBtn_clicked();

    void on_sendBtn_clicked();

    void set_room_name(const char *room_name);
    void clear_room_user_list();
    void inputEnabled(bool state);
    void exit_event();
    void on_roomList_doubleClicked(const QModelIndex &index);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::ClientMainWindow *ui;

    const QString LEAVE_ROOM_BTN_STRING = "Leave Room";
    const QString LEAVE_CHAT_BTN_STRING = "Leave Chat";
    const QString ROOT_ROOM_BASE_STRING = "<p style=\"text-align: center;\"><strong><span style=\"color:#0000cd;\">Welcome to ChatUs!\nEnter a room from the list beside, or create a new room to start chating!</span></strong></p>";
    const QString CHAT_WINDOW_TITLE = "ChatUs";

};

#endif // CLIENTMAINWINDOW_H
