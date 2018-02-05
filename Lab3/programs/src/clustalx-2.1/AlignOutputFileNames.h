/**
 * Changes:
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects.
 */
#ifndef ALIGNOUTPUTFILENAMES_H
#define ALIGNOUTPUTFILENAMES_H

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

class AlignOutputFileNames : public QDialog
{
    Q_OBJECT
public:
    AlignOutputFileNames(QString inputFileName, FileNameDialogType _type, 
                         AlignType _alignType, QString profileName = ""); 
    clustalw::AlignmentFileNames getNames();
    bool getRealignEndGapPen(){return realignEndGapPen;}
protected:


private slots:
    void accept();
    void cancel();
    void browseGuideTreeSeq();
    void browseGuideTreeProfile();
    void browseInputTreeSeq();
    void browseInputTreeProfile();
    void browseClustal();
    void browseNBRF();
    void browseGCG();
    void browseNexus();
    void browseFasta();
    void browsePhylip();
    void browseGDE();
private:
    void browse(QLineEdit* lineEditName);
    void browseInputTree(QLineEdit* lineEditName);
    void setUpLayout();
    void setUpTopBox(); 
    void setUpOptional();
    void setAllPtrsToNull(); 
    QString getPathFromFileName(QString fileName);  
    
    QVBoxLayout* mainLayout;
    QGridLayout* topBoxLayout;
    QGridLayout* fileLayout;
    QGridLayout* otherParamLayout;
    
    QPushButton* okButton;
    QPushButton* alignOutCancelButton;
    QPushButton* guideTreeSeqBrowseButton;
    QPushButton* guideTreeProfile2BrowseButton;
    QPushButton* clustalBrowseButton;
    QPushButton* nbrfBrowseButton;
    QPushButton* gcgBrowseButton;
    QPushButton* phylipBrowseButton;
    QPushButton* gdeBrowseButton;
    QPushButton* nexusBrowseButton;
    QPushButton* fastaBrowseButton;
    
    QGroupBox* fileBox;
    QGroupBox* topBox;
    
    QComboBox* realignSegEndGapPenalties;
    
    
    QLabel* guideTreeNameLabelSeq;
    QLabel* outputFilesLabel;
    QLabel* clustalNameLabel;
    QLabel* nbrfNameLabel;
    QLabel* gcgNameLabel;
    QLabel* phylipNameLabel;
    QLabel* gdeNameLabel;
    QLabel* nexusNameLabel;
    QLabel* fastaNameLabel;
    QLabel* realignSegEndGapPenaltiesLabel;
    
    QLineEdit* guideTreeNameSeq;
    QLineEdit* guideTreeNameProfile2;
    QLineEdit* clustalName;
    QLineEdit* nbrfName;
    QLineEdit* gcgName;
    QLineEdit* phylipName;
    QLineEdit* gdeName;
    QLineEdit* nexusName;
    QLineEdit* fastaName;
    
    FileNameDialogType type;
    AlignType alignType;
    QString title;
    
    QString filePath;
    QString profile2Path;
    
    auto_ptr<clustalw::AlignmentFileNames> outFileNames; 
    static const int MinLineEditWidth = 250;
    bool realignEndGapPen;  
};

#endif
