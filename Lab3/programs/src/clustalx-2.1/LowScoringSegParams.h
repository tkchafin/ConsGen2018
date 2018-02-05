#ifndef LOWSCORINGSEGPARAMS_H
#define LOWSCORINGSEGPARAMS_H

#include <QDialog>
class QPushButton;
class QGroupBox;
class QRadioButton;
class QComboBox;
class QLabel;
class QButtonGroup;
class QSlider;
class QLCDNumber;
class QGridLayout;
class QHBoxLayout;
class QVBoxLayout;

class LowScoringSegParams : public QDialog
{
    Q_OBJECT
public:
    LowScoringSegParams();

protected:
    void showEvent(QShowEvent* event);

private slots:
    void accept();
    void browseProtein();
    void browseDNA();
    void userProteinClicked();
    void userDNAClicked();     
private:
    void setUpLayout();
    void setUpProteinMatRadioButtons();
    void setInitialProteinMat();
    void setInitialDNAMat();
    void setInitialParamValues();
    void setUpDNAMatRadioButtons();  
    void setUpOtherParams();
    void radioSetUsingUserProtein();
    
    QVBoxLayout* mainLayout;
    QHBoxLayout* horizLayoutAA;
    QHBoxLayout* horizLayoutDNA;
    QGridLayout* grid;
    QGridLayout* protMatLayout;
    QGridLayout* dnaMatLayout;    
           
    QPushButton* okButton;
    QPushButton* loadProtMat;
    QPushButton* loadDNAMat;
    
    QGroupBox* sliderGridBox;
    QGroupBox* proteinMatGridBox;
    QGroupBox* dnaMatGridBox;
    QGroupBox* loadDNAGroup;
    QGroupBox* loadProteinGroup;
        
    QButtonGroup* proteinWeightMat;
    QRadioButton* gonnetPAM80;
    QRadioButton* gonnetPAM120;
    QRadioButton* gonnetPAM250;
    QRadioButton* gonnetPAM350;
    QRadioButton* aaUserMat;
    
    QButtonGroup* DNAWeightMat;
    QRadioButton* IUB;
    QRadioButton* clustal;
    QRadioButton* dnaUserMat;  
    
    QLabel* minLenSegsLabel;
    QLabel* dnaMarkingScaleLabel;
    
    QSlider* minLenSegs;
    QSlider* dnaMarkingScale;
    
    QLCDNumber* minLenSegsLCD;
    QLCDNumber* dnaMarkingScaleLCD;
    
    QLabel* userProteinMatrix;
    QLabel* userDNAMatrix;
        
    QString aaMatrixPath;
    QString dnaMatrixPath;
    
    std::string aaMatrixFile;
    std::string dnaMatrixFile;
        
    bool proteinMatLoaded;
    bool dnaMatLoaded;     
};

#endif
