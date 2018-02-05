/**
 * Changes: 
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. 
 */
#ifndef SAVESEQFILE_H
#define SAVESEQFILE_H

#include <QDialog>
#include "ClustalQtParams.h"

class QPushButton;
class QGroupBox;
class QComboBox;
class QLabel;
class QCheckBox;
class QVBoxLayout;
class QGridLayout;
class QLineEdit;

class SaveSeqFile : public QDialog
{
    Q_OBJECT
public:
    SaveSeqFile(QString inputFileName, int type, rangeParams params);
    QString getFileName(){return fileName;}
protected:


private slots:
    void accept();
    void cancel();
    void browse();
    void changeUseRange();
private:
    void setUpLayout();
    void setUpRadioButtons(); 
    void setUpOtherParams();   
    
    QVBoxLayout* mainLayout;
    QGridLayout* grid;
    QGridLayout* otherParamLayout;
    
    QPushButton* okButton;
    QPushButton* cancelButton;
    QPushButton* browseButton;
    QGroupBox* fileGridBox;
    QGroupBox* otherParams;
    QGroupBox* verticalBox;
    
    QCheckBox* clustal;
    QCheckBox* gcg;
    QCheckBox* gde;
    QCheckBox* fasta;
    QCheckBox* nbrf;
    QCheckBox* phylip;
    QCheckBox* nexus;
    QCheckBox* useRangeInfo; 
    
    QComboBox* gdeOutputCase;
    QComboBox* clustalSequenceNumbers;
    QComboBox* rangeNumbers;
    
    QLabel* gdeOutputCaseLabel;
    QLabel* clustalSequenceNumbersLabel;
    QLabel* rangeFromLabel;
    QLabel* rangeToLabel;
    QLabel* rangeNumbersLabel;
    QLabel* saveAsLabel;
    
    QLineEdit* rangeFrom;
    QLineEdit* rangeTo;
    QLineEdit* saveAs;
    
    int type;
    QString title;
    QString label;
    
    QString fileName;
    
    int firstSeq, lastSeq, firstRes, lastRes;
};

#endif
