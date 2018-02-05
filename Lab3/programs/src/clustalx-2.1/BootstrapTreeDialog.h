/**
 * Changes:
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. Also removed
 * setAllPtrsToNull function. 
 */
#ifndef BOOTSTRAPTREEDIALOG_H
#define BOOTSTRAPTREEDIALOG_H

#include <QDialog>
#include <memory>
#include "ClustalQtParams.h"

class QPushButton;
class QGroupBox;
class QComboBox;
class QLabel;
class QCheckBox;
class QVBoxLayout;
class QGridLayout;
class QLineEdit;

class BootstrapTreeDialog : public QDialog
{
    Q_OBJECT
public:
    BootstrapTreeDialog(QString seqFileName); 
    clustalw::TreeNames getNames();
protected:


private slots:
    void accept();
    void cancel();
    void browseClustal();
    void browseNexus();
    void browsePhylip();
private:
    void browse(QLineEdit* lineEditName);
    void setUpLayout();
    void setAllPtrsToNull(); 
    QString getPathFromFileName(QString fileName);  
    
    QVBoxLayout* mainLayout;
    QGridLayout* fileLayout;
    
    QPushButton* okButton;
    QPushButton* bootstrapCancelButton;
    QPushButton* clustalTreeBrowseButton;
    QPushButton* phylipTreeBrowseButton;
    QPushButton* nexusTreeBrowseButton;
    
    QGroupBox* fileBox;
    
    QLabel* randomNumberGenSeedLabel;
    QLabel* numBootstrapTrialsLabel;
    QLabel* clustalTreeNameLabel;
    QLabel* phylipTreeNameLabel;
    QLabel* nexusTreeNameLabel;
        
    QLineEdit* randomNumberGenSeed;
    QLineEdit* numBootstrapTrials;
    QLineEdit* clustalTreeName;
    QLineEdit* phylipTreeName;
    QLineEdit* nexusTreeName;
    
    QString title;
    
    QString filePath;
    
    auto_ptr<clustalw::TreeNames> treeFileNames; 
    static const int MinLineEditWidth = 250; 
};

#endif
