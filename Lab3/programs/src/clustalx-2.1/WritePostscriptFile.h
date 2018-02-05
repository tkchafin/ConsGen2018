/**
 * Changes: 
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. 
 */
#ifndef WRITEPOSTSCRIPTFILE_H
#define WRITEPOSTSCRIPTFILE_H

#include <QDialog>
#include <memory>
#include "ClustalQtParams.h"
#include "PostscriptFileParams.h"

class QPushButton;
class QGroupBox;
class QComboBox;
class QLabel;
class QCheckBox;
class QVBoxLayout;
class QGridLayout;
class QLineEdit;

class WritePostscriptFile : public QDialog
{
    Q_OBJECT
public:
    WritePostscriptFile(QString inputFileName, int type, int lengthLongest);
    QString getFileName(){return fileName;}
    PostscriptFileParams getParams();
protected:


private slots:
    void accept();
    void cancel();
    void browse();
    void changeBlockSize();
private:
    void setUpLayout();
    void setFileBox(); 
    void setUpOtherParams();   
    int getSuggestedBlockSize();
    QVBoxLayout* mainLayout;
    QGridLayout* fileLayout;
    QGridLayout* otherParamLayout;
    
    QPushButton* okButton;
    QPushButton* cancelButton;
    QPushButton* browseButton;
    
    QGroupBox* otherParams;
    QGroupBox* fileBox;
    
    QComboBox* pageSize;
    QComboBox* orientation;
    QComboBox* printHeader;
    QComboBox* printQualityCurve;
    QComboBox* printRuler;
    QComboBox* printResidueNumbers;
    QComboBox* resizeToFit;
    
    QLabel* writeSeqsToLabel;
    QLabel* psColorsFileLabel;
    QLabel* pageSizeLabel;
    QLabel* orientationLabel;
    QLabel* printHeaderLabel;
    QLabel* printQualityCurveLabel;
    QLabel* printRulerLabel;
    QLabel* printResidueNumbersLabel;
    QLabel* resizeToFitLabel;
    QLabel* printFromPosLabel;
    QLabel* printToPosLabel;
    QLabel* useBlockLengthLabel;
    
    QLineEdit* writeSeqsTo;
    QLineEdit* psColorsFile;
    QLineEdit* printFromPos;
    QLineEdit* printToPos;
    QLineEdit* useBlockLength;
    
    int type;
    QString title;
    QString label;
    
    QString fileName;
    
    int blockLength;
    int lengthLongestSeq;
    auto_ptr<PostscriptFileParams> params;
    
};

#endif
