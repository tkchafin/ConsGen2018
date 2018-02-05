#ifndef PAIRWISEPARAMS_H
#define PAIRWISEPARAMS_H

#include <QDialog>
#include <QComboBox>
class QPushButton;
class QGroupBox;
class QRadioButton;
class QLabel;
class QButtonGroup;
class QTabWidget;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QShowEvent;

class PairwiseParams : public QDialog
{
    Q_OBJECT
public:
    PairwiseParams();
protected:
    void showEvent(QShowEvent* event);

private slots:
    void accept();
    void slowBrowseProtein();
    void slowBrowseDNA();
    void userProteinClicked();
    void userDNAClicked();    
private:
    void setUpSlowTab();
    void setUpSlowTextAreas();
    void setUpSlowProteinWeightMat();
    void setUpSlowDNAWeightMat();
    void setUpFastTab();
    void setInitialProteinMat();
    void setInitialDNAMat(); 
    void radioSetUsingUserProtein(); 
    void setSlowInitialTextParams();
    void setFastInitialTextParams();
    
    QTabWidget* tabWidget;   
    QPushButton* okButton;
    
    QComboBox* slowOrQuick;
        
    QGroupBox* loadDNAGroup;
    QGroupBox* loadProteinGroup;
    QGroupBox* slowPairwiseTab;
    QGroupBox* fastPairwiseTab;
    
    QVBoxLayout* mainLayout;
    QVBoxLayout* slowTabLayout;
    QGridLayout* slowGapLayout;
    QGridLayout* protMatLayout;
    QGridLayout* dnaMatLayout;
    QGridLayout* fastTabLayout;
    QHBoxLayout* horizLayoutAA;
    QHBoxLayout* horizLayoutDNA;
        
    // Slow first
    QGroupBox* slowGapParams;
    QLabel* slowGapOpenLabel;
    QLabel* slowGapExtendLabel;
    
    QLineEdit* slowGapOpen;
    QLineEdit* slowGapExtend;
    
    QString slowAAMatrixPath;
    QString slowDNAMatrixPath;
    
    std::string aaMatrixFile;
    std::string dnaMatrixFile;    
    
    QPushButton* slowLoadProtMat;
    QPushButton* slowLoadDNAMat;
    
    QGroupBox* slowProtMatGroup;
    QButtonGroup* slowProteinWeightMat;
    QRadioButton* slowProtBlosum30;
    QRadioButton* slowProtPAM350;
    QRadioButton* slowProtGonnet250;
    QRadioButton* slowProtIdentity;
    QRadioButton* slowProtAAUserMat; 
    
    QGroupBox* slowDNAMatGroup;
    QButtonGroup* slowDNAWeightMat;
    QRadioButton* slowDNAIUB;
    QRadioButton* slowDNAClustalw;
    QRadioButton* slowDNAUser;  
    
    // Fast
    QGroupBox* fastParams;
    
    QLineEdit* fastGapPenalty;
    QLineEdit* fastKTupSize;
    QLineEdit* fastTopDiagonals;
    QLineEdit* fastWindowSize;
    
    QLabel* fastGapPenaltyLabel;
    QLabel* fastKTupSizeLabel;
    QLabel* fastTopDiagonalsLabel;
    QLabel* fastWindowSizeLabel;
    
    QLabel* userProteinMatrix;
    QLabel* userDNAMatrix;
    
    bool proteinMatLoaded;
    bool dnaMatLoaded;           
};

#endif
