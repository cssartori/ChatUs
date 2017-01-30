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

#ifndef NAMEDIALOG_H
#define NAMEDIALOG_H

#include <QDialog>

namespace Ui {
class nameDIalog;
}

class nameDIalog : public QDialog
{
    Q_OBJECT

public:
    explicit nameDIalog(QWidget *parent, const char* insertLabel);
    ~nameDIalog();
    const char* get_input_name();

private slots:
    void on_buttonBox_accepted();
    bool isValidString(const char* name);
    void on_buttonBox_rejected();

private:
    Ui::nameDIalog *ui;
    QString inputName;
};

#endif // NAMEDIALOG_H
