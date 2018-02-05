/**
 * Changes: 
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. Also removed
 * setAllPtrsToNull function.  
 *
 * 24-05-07,Nigel Brown(EMBL): simplified file searching mechanism; removed
 * filePath, executablePath members; made helpFileName static; removed
 * executablePath argument to constructor.
 *
 * 06-06-07,Nigel Brown(EMBL): loadHelpInformation() now returns string *.
 */
#ifndef HELPDISPLAYWIDGET_H
#define HELPDISPLAYWIDGET_H

#include <QDialog>
#include <string>
#include "clustalW/general/clustalw.h"
#include "ClustalQtParams.h"

class QPushButton;
class QTextEdit;
class QVBoxLayout;

class HelpDisplayWidget : public QDialog
{
    Q_OBJECT

public:
    HelpDisplayWidget(char helpPointer); 

protected:

private slots:
    void accept();

private:
    string *loadHelpInformation(char helpPointer);  
    
    QVBoxLayout* mainLayout;
    QTextEdit *helpText;
    QPushButton* okButton;
    QString title;

    static const std::string helpFileName;
    static const int MinLineEditWidth = 250;
};

#endif
//HELPDISPLAYWIDGET_H
