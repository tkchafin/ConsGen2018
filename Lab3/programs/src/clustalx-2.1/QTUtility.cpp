#include <iostream>
#include <QMessageBox>
#include <QString>
#include <QLabel>
#include <QApplication>

#include "QTUtility.h"
namespace clustalw
{

QTUtility::QTUtility(QWidget* main)
{
    mainwidget = main;
    infoLabel = 0;
}

void QTUtility::setInfoLabelPtr(QLabel* ptrToLabelObj)
{
    infoLabel = ptrToLabelObj;
}

/**
 * 
 * @param msg 
 */
void QTUtility::error( char *msg,...)
{
    va_list ap;
    char myMessage[1000];
    va_start(ap, msg);
    vsprintf(myMessage, msg, ap);
    QString myString(myMessage);
    QString errorString("ERROR: ");
    myString = errorString + myString;
    QMessageBox::information(0, "", myString, QMessageBox::Ok);    
    va_end(ap);
}

void QTUtility::error(const char *msg,...)
{
    va_list ap;
    char myMessage[1000];
    va_start(ap, msg);
    vsprintf(myMessage, msg, ap);
    QString myString(myMessage);
    QString errorString("ERROR: ");
    myString = errorString + myString;
    QMessageBox::information(0, "", myString, QMessageBox::Ok);    
    va_end(ap);
}

/**
 * 
 * @param msg 
 */
void QTUtility::warning( char *msg,...)
{
    va_list ap;
    char myMessage[1000];
    va_start(ap, msg);
    vsprintf(myMessage, msg, ap);
    QString myString(myMessage);    
    QString warningString("WARNING: "); 
    myString = warningString + myString; 
    QMessageBox::information(0, "", myString, QMessageBox::Ok);          
    va_end(ap);
}

void QTUtility::warning(const char *msg,...)
{
    va_list ap;
    char myMessage[1000];
    va_start(ap, msg);
    vsprintf(myMessage, msg, ap);
    QString myString(myMessage);    
    QString warningString("WARNING: "); 
    myString = warningString + myString; 
    QMessageBox::information(0, "", myString, QMessageBox::Ok);          
    va_end(ap);
}

void QTUtility::info( char *msg,...)
{
    if(infoLabel)
    {
        va_list ap;
        char myMessage[1000];
        va_start(ap, msg);
        fprintf(stdout, "\n");
        vfprintf(stdout, msg, ap);
        vsprintf(myMessage, msg, ap);
        QString myString(myMessage);
        infoLabel->setText(myString);
        
        QApplication::processEvents();// increases responsiveness

        va_end(ap);
    }
}

void QTUtility::info(const char *msg,...)
{
    if(infoLabel)
    {
        va_list ap;
        char myMessage[1000];
        va_start(ap, msg);
        fprintf(stdout, "\n");
//        vfprintf(stdout, msg, ap);
        vsprintf(myMessage, msg, ap);
        cout << myMessage;
        QString myString(myMessage);
        infoLabel->setText(myString);
        
        QApplication::processEvents();// increases responsiveness
        va_end(ap);   
    }
}



       
/**
 * 
 */
void QTUtility::myname( char *myname)
{
    strcpy(myname, "clustalx\0");
}


char QTUtility::promptForYesNo(char *title, char *prompt)
{
    QString promptMessage = QString(title) + "\n" + QString(prompt) + "?";
    
    int answer;
    answer = QMessageBox::question(mainwidget, "", promptMessage, QMessageBox::Yes,
                                      QMessageBox::No);
    if(answer == QMessageBox::Yes)
    {
        return ('y');
    }
    else
    {
        return ('n');
    }
}

char QTUtility::promptForYesNo(const char *title, char *prompt)
{
    QString promptMessage = QString(title) + "\n" + QString(prompt) + "?";
    
    int answer;
    answer = QMessageBox::question(mainwidget, "", promptMessage, QMessageBox::Yes,
                                      QMessageBox::No);
    if(answer == QMessageBox::Yes)
    {
        return ('y');
    }
    else
    {
        return ('n');
    }
    return 'y';  
}

void QTUtility::setMainWindowAddress(QWidget* main)
{
    mainwidget = main;
}
}
