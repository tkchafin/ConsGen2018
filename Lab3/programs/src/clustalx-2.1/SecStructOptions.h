#ifndef SECSTRUCTOPTIONS_H
#define SECSTRUCTOPTIONS_H

#include <QDialog>
class QPushButton;
class QGroupBox;
class QComboBox;
class QLabel;
class QCheckBox;
class QLineEdit;

class SecStructOptions : public QDialog
{
    Q_OBJECT
public:
    SecStructOptions();

protected:


private slots:
    void accept();

private:
    void setUpCombo();
    void setUpOutput(); 
    void setUpOtherParams();   
    
    QPushButton* okButton;
    
    QComboBox* useProfile1SS;
    QComboBox* useProfile2SS;
    
    QLabel* useProfile1SSLabel;
    QLabel* useProfile2SSLabel;
    
    QGroupBox* outputGridBox;
    QGroupBox* ssGridBox;
    QGroupBox* ssParamsGridBox;
    QGroupBox* verticalBox;
    
    QCheckBox* secStruct;
    QCheckBox* gapMask;
    
    QLineEdit* helixGapPen;
    QLineEdit* strandGapPen;
    QLineEdit* loopGapPen;
    QLineEdit* ssTerminalPen;
    QLineEdit* helixTerminalPosWithin;
    QLineEdit* helixTerminalPosOutside;
    QLineEdit* strandTerminalPosWithin;
    QLineEdit* strandTerminalPosOutside;
    
    QLabel* helixGapPenLabel;
    QLabel* strandGapPenLabel;
    QLabel* loopGapPenLabel;
    QLabel* ssTerminalPenLabel;
    QLabel* helixTerminalPosWithinLabel;
    QLabel* helixTerminalPosOutsideLabel;
    QLabel* strandTerminalPosWithinLabel;
    QLabel* strandTerminalPosOutsideLabel;
};

#endif
