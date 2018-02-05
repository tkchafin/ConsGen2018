/**
 * Implements a singleton that maintains program resources.
 * The single instance is (re)instantiated on demand like:
 *     Resources *res = Resources::Instance();
 *
 * 24-05-07,Nigel Brown(EMBL): created.
 */

#ifndef RESOURCES_H
#define RESOURCES_H

#include <QString>
#include <string>

enum ResourcePathType {
    DefaultPath,
    InstallPath,
    ExecutablePath,
    HomePath,
};

class Resources {

public:
    /* return the Resources singleton */
    static Resources *Instance();

    /* setters */
    void setPathToExecutable(QString pathToFiles);
    
    /* getters */
    QString getDefaultPath()    { return defaultPath; }
    QString getInstallPath()    { return installPath; }
    QString getExecutablePath() { return executablePath; }
    QString getHomePath()       { return homePath; }

    std::string findFile(const char *file, const ResourcePathType where = DefaultPath) const;
    std::string findFile(const std::string &file, const ResourcePathType where = DefaultPath) const;
    std::string findFile(const QString &file, const ResourcePathType where = DefaultPath) const;

    QString searchPathsForFile(const QString &fileName) const;
    std::string searchPathsForFile(const std::string &fileName) const;

    /* debug */
    void dump();

protected:
    /* hide the constructors */
    Resources();
    Resources(const Resources&);
    Resources& operator= (const Resources&);
    
    QString dirname(QString path);    

private:
    QString defaultPath;
    QString installPath;
    QString executablePath;
    QString homePath;
};

#endif //RESOURCES_H
