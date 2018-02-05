/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 *
 * Changes:
 *
 * 03-02-07,Nigel Brown(EMBL): removed 'globalLastDir.h' dependency
 * and 'lastDir' global initialization.
 *
 * 24-05-07,Nigel Brown(EMBL): changed saving of executablePath to use
 * Resources class instead of MainWindow.
 *
 * 2007-12-03, Andreas Wilm (UCD): added support for infile flag
 * (commandline)
 *
 * 2007-12-06, Andreas Wilm (UCD): added full clustalw command line support
 *
 ****************************************************************************/

#include <QApplication>
//#include <QTranslator>
#include "clustalW/substitutionMatrix/SubMatrix.h"
#include "QTUtility.h"
#include "clustalW/general/UserParameters.h"
#include "mainwindow.h"
#include <QString>
#include <QDir>
#include "version.h"
// - nige: Resources.h
#include "Resources.h"
#include "clustalW/interface/CommandLineParser.h"

namespace clustalw
{ 
    UserParameters* userParameters;
    Utility* utilityObject;
    SubMatrix *subMatrix;    
    Stats* statsObject;
}

/**
 * The main function. It creates a new MainWindow widget and displays it.
 */
using namespace clustalw; 
int main(int argc, char *argv[])
{
    //QTranslator translator;
    //translator.load("clustalx_la");
    
    // Note the path to the executable is usually passed in as argv[0]
    // This will be used if we cannot find the help or the xml files in current dir.
    Resources *resources = Resources::Instance();
    resources->setPathToExecutable(QString(argv[0]));
    //resources->dump(); //debug: nige

    userParameters = new UserParameters(false);
    subMatrix = new SubMatrix(); // Mark April 11th 2007
    userParameters->setInteractive(true);
    userParameters->setGui(true); // gui is not the same as interactive - text menus are interactive but not gui
    statsObject = new Stats();// actually only needed in clustalw

    QApplication app(argc, argv);
    //app.installTranslator(&translator);
    psPrintParams params;

    MainWindow window;
    utilityObject = new QTUtility(&window);
    window.show();

    // parse command line if we got some non-Qt args
    // must be done after we've setup utilityObject
    if (argc > 1)
    {
        vector<string> args;
        for (int i = 1; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
        userParameters->setMenuFlag(false);
        CommandLineParser cmdLineParser(&args, true);
    }

    // if profiles were given on the command line, we need to switch to two window mode
    if(clustalw::userParameters->getProfileNum() > 0) // profile mode
    {
        window.setProfileModeCmd();
        window.loadProfile1(clustalw::userParameters->getProfile1Name());
        window.loadProfile2(clustalw::userParameters->getProfile2Name());
    }
    else // multiple alignment mode
    {
        // infile parsed? then load
        string seqname = userParameters->getSeqName();
        if (seqname.length())
            window.loadSequences(QString(seqname.c_str()));
    } 

    // Andreas Wilm (UCD):
    // if help flag is given, show graphical clustax help on cmd line
    // flags, instead of trying to load clustalw help (as 1.83 did!)
    //
    if (userParameters->getHelpFlag() ||userParameters->getFullHelpFlag() )
        window.commandLineHelp();
    
    return app.exec();
    
    delete userParameters;
    delete utilityObject;
    delete subMatrix;
}
