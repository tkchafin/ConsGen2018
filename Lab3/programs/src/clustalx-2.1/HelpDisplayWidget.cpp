/**
 * Changes: 
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. Also removed
 * setAllPtrsToNull function.  
 *
 * 24-05-07,Nigel Brown(EMBL): simplified file searching mechanism; removed
 * filePath, executablePath members; made helpFileName static; removed
 * executablePath argument to constructor. Added Resources class to handle
 * file search.
 *
 * 06-06-07,Nigel Brown(EMBL): loadHelpInformation() now returns 'string *'
 * instead of 'void'; constructor tests this value to see if a help string was
 * successfully loaded then typesets it as HTML and scrolls to the top.
 */
#include "HelpDisplayWidget.h"
#include <QTextEdit>
#include <iostream>
#include <fstream>
#include <QtGui>
#include "Resources.h"

const string HelpDisplayWidget::helpFileName = "clustalx.hlp";

HelpDisplayWidget::HelpDisplayWidget(char helpPointer)
{
    setWindowTitle("Help");
    
    helpText = new QTextEdit;
    helpText->setReadOnly(true);
    helpText->setMinimumWidth(500);
    helpText->setMinimumHeight(400);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout = new QVBoxLayout;
    
    okButton = new QPushButton(tr("Ok"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));    

    mainLayout->addWidget(helpText);
    mainLayout->addWidget(okButton);
    setLayout(mainLayout);

    /* nige */
    string *buff = loadHelpInformation(helpPointer);

    if (buff) {
        //cout << buff->c_str() << endl;
        helpText->setHtml(QString(buff->c_str()));

        //nige: scroll to top of text
        QTextCursor tc = helpText->textCursor();
        tc.movePosition(QTextCursor::Start);
        helpText->setTextCursor(tc);
        delete buff;
    }
}

string *HelpDisplayWidget::loadHelpInformation(char helpPointer)
{
    ifstream helpFile;
    int  i, number, nlines;
    bool foundHelp;
    char templine[MAXLINE + 1]; // To be used with get!
    char token = '\0';
    string digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string helpMarker    = ">>HELP";
    
    Resources *res = Resources::Instance();
    string file = res->searchPathsForFile(helpFileName);
    //cout << "helpfile: " << file.c_str() << endl;

    //nige
    string *buff = new string("<BODY>");

    try
    {
        helpFile.open(file.c_str(), ifstream::in);

        if(!helpFile.is_open())
        {
            clustalw::utilityObject->error("Cannot open File file [%s]\n",
                                           helpFileName.c_str());
            return 0;
        }

        nlines = 0;
        number = -1;
        foundHelp = false;
        string tempS;

        while(!helpFile.eof())
        {
            if(!helpFile.getline(templine, MAXLINE + 1))
            {
                if(!foundHelp)
                {
                    clustalw::utilityObject->error("No help found in help file\n");
                }
                helpFile.close();
                return 0;
            }
            tempS = string(templine); 
            
            if(tempS.find(helpMarker, 0) != string::npos) // If found helpMarker! 
            {
                token = ' ';
                for(i = helpMarker.length(); i < 8; i++) // MAGIC 8??????
                {
                    if(digits.find(tempS[i], 0)  != string::npos)
                    {
                        token = tempS[i];
                        break;
                    }
                }
            }
            if(token == helpPointer) 
            {
                foundHelp = true;
                while(helpFile.getline(templine, MAXLINE + 1)) 
                {
                    tempS = string(templine);

                    if(tempS.find(helpMarker, 0) != string::npos) // If found another help
                    {
                        //nige: section terminated by next section anchor
                        helpFile.close();
                        *buff += "</BODY>\n";
                        return buff;
                    }
                    
                    //replace trailing newline on non-html
                    if (tempS[0] != '<') {
                        tempS += '\n';
                    }

                    //cout << tempS.c_str() << endl;
                    *buff += tempS;
                    ++nlines;
                }
                //nige: last section terminated by EOF
                helpFile.close();
                *buff += "</BODY>\n";
                return buff;
            }
        }
    }
    catch(ifstream::failure e)
    {
        return 0;
    }
    if(helpFile.is_open())
    {
        helpFile.close();
    }
    return 0;
}

void HelpDisplayWidget::accept()
{
    setResult(QDialog::Accepted);
    hide();
}
