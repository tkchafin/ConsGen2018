#ifndef ALIGNMENTPARAMETERS_H
#define ALIGNMENTPARAMETERS_H

#include <QDialog>
class QPushButton;
class QGroupBox;
class QRadioButton;
class QComboBox;
class QLabel;
class QButtonGroup;
class QLineEdit;
class QVBoxLayout;
class QGridLayout;
class QHBoxLayout;
class QHBoxLayout;
class QShowEvent;

class AlignmentParameters : public QDialog
{
    Q_OBJECT
public:
    AlignmentParameters();

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
    void setInitialParams();
    void radioSetUsingUserProtein();
       
    QPushButton* okButton;
    QPushButton* loadProtMat;
    QPushButton* loadDNAMat;
    
    QGroupBox* paramGridBox;
    QGroupBox* proteinMatGridBox;
    QGroupBox* dnaMatGridBox;
    QGroupBox* verticalBox;
    QGroupBox* loadDNAGroup;
    QGroupBox* loadProteinGroup;
    
    QVBoxLayout* mainLayout;
    QGridLayout* grid;
    QGridLayout* protMatLayout;
    QGridLayout* dnaMatLayout;
    QVBoxLayout* multipleParamLayout;
    QHBoxLayout* horizLayoutAA;
    QHBoxLayout* horizLayoutDNA;
    
    QButtonGroup* proteinMat;
    QRadioButton* blosum;
    QRadioButton* pam;
    QRadioButton* gonnet;
    QRadioButton* identity;
    QRadioButton* userProt;
    
    QButtonGroup* DNAMat;
    QRadioButton* IUB;
    QRadioButton* clustal;
    QRadioButton* userDNA;  
    
    QComboBox* useNegMat;
    
    QLineEdit* gapOpen;
    QLineEdit* gapExtend;
    QLineEdit* delayDivergent;
    QLineEdit* dnaTranWeight;
    
    QLabel* useNegMatLabel;
    QLabel* gapOpenLabel;
    QLabel* gapExtendLabel;
    QLabel* delayDivergentLabel;
    QLabel* dnaTranWeightLabel;
    
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
