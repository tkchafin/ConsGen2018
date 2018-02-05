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
#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QFileDialog>
#include <QString>

class FileDialog : public QFileDialog
{
    Q_OBJECT

public:


    QString getOpenDataFileName(QWidget *parent = 0, 
				const QString &caption = QString(), 
				const QString &filter = QString(), 
				QString *selectedFilter = 0);
    
    QString getOpenParamFileName(QWidget *parent = 0, 
				 const QString &caption = QString(), 
				 const QString &filter = QString(), 
				 QString *selectedFilter = 0);
                                    
    QString getSaveDataFileName(const QString &suffix = QString(),
				QWidget *parent = 0,
				const QString &caption = QString(), 
				const QString &filter = QString(), 
				QString *selectedFilter = 0);    
protected:

private slots:

private:
    static QString lastDataDir;
    static QString lastParamDir;
    static QString lastBaseName;
};

#endif //FILEDIALOG_H
