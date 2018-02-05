#ifndef ALIGNMENTFORMATOPTIONS_H
#define ALIGNMENTFORMATOPTIONS_H

#include <QDialog>
class QPushButton;
class QGroupBox;
class QComboBox;
class QLabel;
class QCheckBox;

class AlignmentFormatOptions : public QDialog
{
    Q_OBJECT
public:
    AlignmentFormatOptions();

protected:


private slots:
    void accept();

private:
    void setUpLayout();
    void setUpRadioButtons(); 
    void setUpOtherParams();   
    QPushButton* okButton;
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
    
    QComboBox* gdeOutputCase;
    QComboBox* clustalSequenceNumbers;
    QComboBox* outputOrder;
    QComboBox* parameterOutput;
    
    QLabel* gdeOutputCaseLabel;
    QLabel* clustalSequenceNumbersLabel;
    QLabel* outputOrderLabel;
    QLabel* parameterOutputLabel;
};

#endif
