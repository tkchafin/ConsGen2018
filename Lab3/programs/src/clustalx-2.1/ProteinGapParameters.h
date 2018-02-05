#ifndef PROTEINGAPPARAMETERS_H
#define PROTEINGAPPARAMETERS_H

#include <QDialog>
class QPushButton;
class QGroupBox;
class QRadioButton;
class QComboBox;
class QLabel;
class QLineEdit;

class ProteinGapParameters : public QDialog
{
    Q_OBJECT
public:
    ProteinGapParameters();

protected:


private slots:
    void accept();

private:
    void setUpLayout();
    void setUpOtherParams(); 
    void setInitialParameterValues();  
    
    QPushButton* okButton;
    QGroupBox* otherParams;
    
    QComboBox* resSpecPen;
    QComboBox* hydSpecPen;
    QComboBox* endGapSeparation;
    
    QLineEdit* hydroPhilicRes;
    QLineEdit* gapSeparationDist;
    
    QLabel* resSpecPenLabel;
    QLabel* hydSpecPenLabel;
    QLabel* endGapSeparationLabel;
    QLabel* hydroPhilicResLabel;
    QLabel* gapSeparationDistLabel;
    
    int gapDist;
};

#endif
