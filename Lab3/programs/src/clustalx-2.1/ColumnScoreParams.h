#ifndef COLUMNSCOREPARAMS_H
#define COLUMNSCOREPARAMS_H

#include <QDialog>
class QPushButton;
class QGroupBox;
class QRadioButton;
class QComboBox;
class QLabel;
class QButtonGroup;
class QSlider;
class QLCDNumber;
class QShowEvent;
class QHBoxLayout;
class QGridLayout;

class ColumnScoreParams : public QDialog
{
    Q_OBJECT
public:
    ColumnScoreParams();

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
    void setUpDNAMatRadioButtons();  
    void setUpOtherParams();
    void setUpLoadButtons();
    void setInitialProteinMat();
    void setInitialDNAMat();
    void setInitialParamValues();
    void radioSetUsingUserProtein();
       
    QPushButton* okButton;
    QPushButton* loadProtMat;
    QPushButton* loadDNAMat;
    
    QGroupBox* sliderGridBox;
    QGroupBox* proteinMatGridBox;
    QGroupBox* dnaMatGridBox;
    QGroupBox* loadDNAGroup;
    QGroupBox* loadProteinGroup;
        
    QHBoxLayout* horizLayoutAA;
    QHBoxLayout* horizLayoutDNA;
    QGridLayout* grid;
    QGridLayout* protMatLayout;
    QGridLayout* dnaMatLayout;
        
    QButtonGroup* proteinWeightMat;
    QRadioButton* identity;
    QRadioButton* gonnetPAM80;
    QRadioButton* gonnetPAM120;
    QRadioButton* gonnetPAM250;
    QRadioButton* gonnetPAM350;
    QRadioButton* aaUserMat;
    
    QButtonGroup* DNAWeightMat;
    QRadioButton* IUB;
    QRadioButton* clustal;
    QRadioButton* dnaUserMat;  
    
    QLabel* scorePlotScale;
    QLabel* residueExceptionCutoff;
    
    QSlider* plotScale;
    QSlider* cutOff;
    
    QLCDNumber* plotValue;
    QLCDNumber* cutOffValue;
    
    QString aaMatrixPath;
    QString dnaMatrixPath;
    
    QLabel* userProteinMatrix;
    QLabel* userDNAMatrix;
        
    std::string aaMatrixFile;
    std::string dnaMatrixFile;    
    
    bool proteinMatLoaded;
    bool dnaMatLoaded;    
};

#endif
