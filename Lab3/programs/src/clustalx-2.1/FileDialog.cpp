/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 * 
 * This widget was created to get around the problem that the QFileDialog cannot
 * remember the last directory visited. The QFileDialog doesnt know the last
 * directory it has been to. So I developed a wrapper class to make it easier.
 * It uses a global string defined in globalLastDir.h
 *
 * Changes:
 *
 * 03-02-07,Nigel Brown(EMBL): copied getOpenFileName() into two versions
 * getOpenDataFileName() and getOpenParamFileName() and changed lastDir to
 * lastDataDir and lastParamDir. These maintain two different working
 * directories for data files and for parameter files, respectively.
 *
 * 03-02-07,Nigel Brown(EMBL): changed getSaveFileName() to
 * getSaveDataFileName() which now takes a file suffix as a hint to the file
 * to be opened. Added lastBaseName to hold last data file basename (ie., no
 * suffix). Removed 'globalLastDir.h'dependency by making lastDataDir,
 * lastParamDir, lastBaseName static members.
 ****************************************************************************/
#include "FileDialog.h"
#include <QFileInfo>
#include <QDir>
//#include <iostream>
//using namespace std;

/* static initializations */
QString FileDialog::lastDataDir = QString(QDir::currentPath());
QString FileDialog::lastParamDir = QString(QDir::currentPath());
QString FileDialog::lastBaseName;
/* static initializations end */

QString FileDialog::getOpenDataFileName(QWidget *parent, 
					const QString &caption,
					const QString &filter, 
					QString *selectedFilter)
{
    if (lastDataDir == "")
        lastDataDir = QDir::currentPath();
    
    QString filePath = QFileDialog::getOpenFileName(parent, caption,
						    lastDataDir, filter,
						    selectedFilter);
    if (filePath != "") {
	QFileInfo info(filePath);
	lastDataDir  = info.absolutePath();
	lastBaseName = info.completeBaseName();
	//cout << "lastDataDir: " << lastDataDir.toStdString() <<
	//	" lastBaseName: " << lastBaseName.toStdString() << endl;
    }
    return filePath;
}

QString FileDialog::getOpenParamFileName(QWidget *parent, 
					 const QString &caption,
					 const QString &filter, 
					 QString *selectedFilter)
{
    if (lastParamDir == "")
        lastParamDir = QDir::currentPath();
    
    QString filePath = QFileDialog::getOpenFileName(parent, caption,
						    lastParamDir, filter,
						    selectedFilter);
    if (filePath != "") {
	QFileInfo info(filePath);
	lastParamDir = info.absolutePath();
	//cout << "lastParamDir: " << lastParamDir.toStdString() << endl;
    }
    return filePath;
}

QString FileDialog::getSaveDataFileName(const QString &suffix,
					QWidget *parent,
					const QString &caption, 
					const QString &filter, 
					QString *selectedFilter)
{
    QString filePath;
    
    if (lastDataDir == "")
        lastDataDir = QDir::currentPath();
    filePath.append(lastDataDir);
    if (lastBaseName != "") {
	filePath.append("/");
	filePath.append(lastBaseName);
    }
    if (suffix != "") {
	filePath.append(".");
	filePath.append(suffix);
    }

    filePath = QFileDialog::getSaveFileName(parent, caption, filePath, filter,
                                            selectedFilter);
    if(filePath != "") {
	QFileInfo info(filePath);
	lastDataDir  = info.absolutePath();
	lastBaseName = info.completeBaseName();
	//cout << "lastDataDir: " << lastDataDir.toStdString() <<
	//	" lastBaseName: " << lastBaseName.toStdString() << endl;
    }
    return filePath;
}
