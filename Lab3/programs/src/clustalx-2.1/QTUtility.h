#ifndef QTUTILITY_H
#define QTUTILITY_H

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <QWidget>
#include "clustalW/general/clustalw.h"
#include "clustalW/general/Utility.h"
class QLabel;

using namespace std;

namespace clustalw
{

class QTUtility : public Utility
{
    public:
        QTUtility(QWidget* main);
        virtual void error( char *msg,...);
        virtual void error( const char *msg,...);
        virtual void warning( char *msg,...);
        virtual void warning( const char *msg,...);
        virtual char promptForYesNo(char *title,char *prompt);
        virtual char promptForYesNo(const char *title,char *prompt);
        virtual void setMainWindowAddress(QWidget* main);
        virtual void info( char *msg,...);
        virtual void info( const char *msg,...);
        virtual void myname( char *myname );
        virtual void setInfoLabelPtr(QLabel* ptrToLabelObj);
        /* Attributes */

    private:
        /* Functions */


        /* Attributes */
        QWidget* mainwidget;
        QLabel* infoLabel;
};
}
#endif
