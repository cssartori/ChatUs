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

#include "../include/namedialog.h"
#include "ui_namedialog.h"
#include "../../protocol/include/protocol.h"
#include "../include/infomessages.h"

nameDIalog::nameDIalog(QWidget *parent, const char* insertLabel) :
    QDialog(parent),
    ui(new Ui::nameDIalog)
{
    ui->setupUi(this);
    ui->insertLabel->setText(insertLabel);
    ui->insertNameBox->setMaxLength(MAX_ROOMNAME_SIZE-1);
}

nameDIalog::~nameDIalog()
{
    delete ui;
}

/*
 *Check if name has invalid characters, in which case is NOT a valid string
 **/
bool nameDIalog::isValidString(const char* name)
{
    int i;

    for(i=0;i<(int)strlen(name);i++){
        if(name[i] == not_allowed_chars_name[0])
            return false;
        else if(name[i] == not_allowed_chars_name[1])
            return false;
        else if(name[i] == not_allowed_chars_name[2])
            return false;
        else if(name[i] == not_allowed_chars_name[3])
            return false;
    }

    return true;
}

void nameDIalog::on_buttonBox_accepted()
{
    if(ui->insertNameBox->text().isEmpty())
    {
        /*No input text, nothing to do*/
        InfoMessages *msgBox = new InfoMessages(this);
        msgBox->noInputMsg();
        return;
    }

    if(!isValidString(ui->insertNameBox->text().toStdString().c_str()))
    {
        /*Name has invalid characters, communicate error*/
        InfoMessages *msgBox = new InfoMessages(this);
        msgBox->invalidCharMsg();
        return;
    }

    this->inputName = ui->insertNameBox->text();

    this->done(0);
}

const char* nameDIalog::get_input_name(){
    return this->inputName.toStdString().c_str();
}

void nameDIalog::on_buttonBox_rejected()
{
    this->done(-1);
}
