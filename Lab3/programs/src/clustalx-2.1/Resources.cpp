/**
 * Implements a singleton that maintains program resources.
 * The single instance is (re)instantiated on demand like:
 *     Resources *res = Resources::Instance();
 *
 * 24-05-07,Nigel Brown(EMBL): created.
 */

#include "Resources.h"
#include <QString>
#include <QDir>
#include <QFile>
// stdlib only for getenv
#include <cstdlib>
#include <iostream>

using namespace std;

//environment variables
static const char *CLU_INSTALL_DIR = "CLU_INSTALL_DIR";

//return the sole instance
Resources *Resources::Instance() {
    static Resources instance;
    return &instance;
}

Resources::Resources()
{
    //defaultPath
    defaultPath = ".";

    //executablePath
    executablePath = ".";
    
    //installPath
    installPath = ".";
    char *env;
    if ((env = getenv(CLU_INSTALL_DIR)) != 0) {
        installPath = QString(env);
    }

    //homePath
    homePath = QDir::homePath();
}

void Resources::setPathToExecutable(QString path) {
    executablePath = dirname(path);
}

QString Resources::dirname(QString path) {
    QString tempString;
    int size = path.size();
    tempString = path;
    //cout << "tempString is\n" << tempString.toStdString() << "\n";
    for (int i = size - 1; i > 0; i--) {
        if (tempString[i] == QDir::separator()) { 
            tempString.chop(size - i);
            break;
        }
    }
    //cout << "tempString is\n" << tempString.toStdString() << "\n";
    return tempString;
}

void Resources::dump() {
    printf("%s => %s [%s]\n%s => %s\n%s => %s\n",
           "installPath", installPath.toStdString().c_str(), CLU_INSTALL_DIR,
           "executablePath", executablePath.toStdString().c_str(),
           "homePath", homePath.toStdString().c_str()
           );
};

string Resources::findFile(const char *file, const ResourcePathType where) const {
    return findFile(QString(file), where);
}

string Resources::findFile(const string &file, const ResourcePathType where) const {
    return findFile(QString(file.c_str()), where);
}

string Resources::findFile(const QString &file, const ResourcePathType where) const {
    const QString *path;
    switch (where) {
    case InstallPath:
        path = &installPath;
        break;
    case ExecutablePath:
        path = &executablePath;
        break;
    case HomePath:
        path = &homePath;
        break;
    default:
        path = &defaultPath;
        break;
    }
    QString fileName = *path + QString(QDir::separator()) + file;
    // AW: replaced file with qfile, as ICC otherwise complains about redeclaration
    QFile qfile(fileName);
    qfile.open(QFile::ReadOnly);
    if (qfile.isOpen()) {
        qfile.close();
        return fileName.toStdString();
    }
    return string(); //not found/readable
}

/* Search for a (QString) file in a succession of likely locations and
 * return the full path as (QString).
 */
QString Resources::searchPathsForFile(const QString &fileName) const {
    string file = searchPathsForFile(fileName.toStdString());
    return QString(file.c_str());
}

/* Search for a (string) file in a succession of likely locations and
 * return the full path as (string).
 */
string Resources::searchPathsForFile(const string &fileName) const {
    string file;
    while (1) {
        file = findFile(fileName, InstallPath);
        if (file != "") break;
        
        file = findFile(fileName, ExecutablePath);
        if (file != "") break;
        
        file = findFile(fileName, HomePath);
        if (file != "") break;
        
        file = findFile(fileName);
        if (file != "") break;
        
        file = fileName; // give up
        break;
    }
    return file;
}
