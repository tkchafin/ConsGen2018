/**
 * Changes: 
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects.
 */
#ifndef TREEOUTPUTFILENAMES_H
#define TREEOUTPUTFILENAMES_H

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

class TreeOutputFileNames : public QDialog
{
    Q_OBJECT
public:
    TreeOutputFileNames(QString seqFileName); 
    clustalw::TreeNames getNames();
protected:


private slots:
    void accept();
    void cancel();
    void browseClustal();
    void browseNexus();
    void browsePhylip();
    void browseDistances();
    void browsePim();
private:
    void browse(QLineEdit* lineEditName);
    void setUpLayout();
    void setAllPtrsToNull(); 
    QString getPathFromFileName(QString fileName);  
    
    QVBoxLayout* mainLayout;
    QGridLayout* fileLayout;
    
    QPushButton* okButton;
    QPushButton* treeOutCancelButton;
    QPushButton* clustalTreeBrowseButton;
    QPushButton* phylipTreeBrowseButton;
    QPushButton* distancesBrowseButton;
    QPushButton* nexusTreeBrowseButton;
    QPushButton* pimNameBrowseButton;
    
    QGroupBox* fileBox;
    
    QLabel* clustalTreeNameLabel;
    QLabel* phylipTreeNameLabel;
    QLabel* distancesNameLabel;
    QLabel* nexusTreeNameLabel;
    QLabel* pimNameLabel;
        
    QLineEdit* clustalTreeName;
    QLineEdit* phylipTreeName;
    QLineEdit* distancesName;
    QLineEdit* nexusTreeName;
    QLineEdit* pimName;
    
    QString title;
    
    QString filePath;
    
    auto_ptr<clustalw::TreeNames> treeFileNames; 
    static const int MinLineEditWidth = 250; 
};

#endif
