/**
 * Changes:
 * Mark: 18-01-2007. Changed from using QFileDialog to FileDialog.
 *
 * 15-03-07,Nigel Brown(EMBL): setSlowInitialTextParams() was calling
 * getGapOpen() and getGapExtend() instead of getPWGapOpen() and
 * getPWGapExtend(), so these user settings were being dropped.
 */
#include "PairwiseParams.h"
#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QButtonGroup>
#include <QRadioButton>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QtGui>
#include "clustalW/general/userparams.h"
#include "clustalW/substitutionMatrix/globalmatrix.h"
#include "FileDialog.h"

PairwiseParams::PairwiseParams()
{
    setWindowTitle("Pairwise Parameters");
    proteinMatLoaded = false;
    dnaMatLoaded = false;
    
    if(clustalw::userParameters->getDNAFlag())
    {
        clustalw::userParameters->setDNAParams();
    }
    else
    {
        clustalw::userParameters->setProtParams();
    }    

    slowOrQuick = new QComboBox;
    slowOrQuick->addItem("Slow-Accurate");    
    slowOrQuick->addItem("Fast-Approximate");
    if(clustalw::userParameters->getQuickPairAlign())
    {
        slowOrQuick->setCurrentIndex(1);
    }
    else
    {
        slowOrQuick->setCurrentIndex(0);
    }
                
    mainLayout = new QVBoxLayout;
    
    slowPairwiseTab = new QGroupBox;
    fastPairwiseTab = new QGroupBox;
    tabWidget = new QTabWidget;
    
    setUpSlowTab();
    tabWidget->addTab(slowPairwiseTab, "Slow/Accurate Pairwise Parameters");
    setUpFastTab();
    tabWidget->addTab(fastPairwiseTab, "Fast/Approx Pairwise Parameters");
    
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(slowOrQuick);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
}

void PairwiseParams::showEvent(QShowEvent* event)
{
    if(clustalw::userParameters->getQuickPairAlign())
    {
        slowOrQuick->setCurrentIndex(1);
    }
    else
    {
        slowOrQuick->setCurrentIndex(0);
    }    
    // Each time the dialog is shown reload the values from the userParameters object!!!
    setInitialProteinMat();
    setInitialDNAMat();
    setSlowInitialTextParams();
    setFastInitialTextParams();
}

void PairwiseParams::userProteinClicked()
{
    if(!proteinMatLoaded)
    {
        slowBrowseProtein();
    }
}

void PairwiseParams::userDNAClicked()
{
    if(!dnaMatLoaded)
    {
        slowBrowseDNA();
    }
}

void PairwiseParams::setUpSlowTab()
{
    setUpSlowTextAreas();
    setUpSlowProteinWeightMat();
    setUpSlowDNAWeightMat();

    loadProteinGroup = new QGroupBox;
    horizLayoutAA = new QHBoxLayout; 
    userProteinMatrix = new QLabel("");
    slowLoadProtMat = new QPushButton(tr("Load protein matrix:"));
    slowLoadProtMat->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);    
    horizLayoutAA->addWidget(slowLoadProtMat);
    horizLayoutAA->addWidget(userProteinMatrix);
    loadProteinGroup->setLayout(horizLayoutAA);
        
    connect(slowLoadProtMat, SIGNAL(clicked()), this, SLOT(slowBrowseProtein()));

    loadDNAGroup = new QGroupBox;
    horizLayoutDNA = new QHBoxLayout;
    userDNAMatrix = new QLabel("");
    slowLoadDNAMat = new QPushButton(tr("Load DNA matrix:"));
    slowLoadDNAMat->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);     
    horizLayoutDNA->addWidget(slowLoadDNAMat);
    horizLayoutDNA->addWidget(userDNAMatrix);
    loadDNAGroup->setLayout(horizLayoutDNA);       
    
    connect(slowLoadDNAMat, SIGNAL(clicked()), this, SLOT(slowBrowseDNA())); 
    
    slowTabLayout = new QVBoxLayout;
    slowTabLayout->addWidget(slowGapParams);
    slowTabLayout->addWidget(slowProtMatGroup);
    slowTabLayout->addWidget(loadProteinGroup);
    slowTabLayout->addWidget(slowDNAMatGroup);
    slowTabLayout->addWidget(loadDNAGroup);
    
    slowPairwiseTab->setLayout(slowTabLayout);   
}

void PairwiseParams::setUpSlowTextAreas()
{
    slowGapParams = new QGroupBox;
    slowGapOpenLabel = new QLabel(tr("Gap Opening [0-100]: ")); 
    slowGapExtendLabel = new QLabel(tr("Gap Extend [0-100]: "));
    
    slowGapOpen = new QLineEdit; 
    
    slowGapExtend = new QLineEdit;  
    
    slowGapLayout = new QGridLayout; 
    slowGapLayout->addWidget(slowGapOpenLabel, 0, 0);
    slowGapLayout->addWidget(slowGapOpen, 0, 1);
    slowGapLayout->addWidget(slowGapExtendLabel, 1, 0);
    slowGapLayout->addWidget(slowGapExtend, 1, 1);
    slowGapParams->setLayout(slowGapLayout);
}

void PairwiseParams::setSlowInitialTextParams()
{
    QString num;
    num = num.setNum(clustalw::userParameters->getPWGapOpen());
    slowGapOpen->setText(num); 
    
    num = num.setNum(clustalw::userParameters->getPWGapExtend());
    slowGapExtend->setText(num);       
}

void PairwiseParams::setUpSlowProteinWeightMat()
{
    slowProtMatGroup = new QGroupBox(tr("Protein Weight Matrix")); 
    slowProteinWeightMat = new QButtonGroup;
    slowProteinWeightMat->setExclusive(true);
    slowProtBlosum30 = new QRadioButton("BLOSUM 30");
    slowProtPAM350 = new QRadioButton("PAM 350");
    slowProtGonnet250 = new QRadioButton("Gonnet 250");
    slowProtIdentity = new QRadioButton("Identity matrix");
    slowProtAAUserMat = new QRadioButton("User defined");

    connect(slowProtAAUserMat, SIGNAL(clicked()), this, SLOT(userProteinClicked()));    
        
    slowProteinWeightMat->addButton(slowProtBlosum30);
    slowProteinWeightMat->addButton(slowProtPAM350);
    slowProteinWeightMat->addButton(slowProtGonnet250);
    slowProteinWeightMat->addButton(slowProtIdentity);
    slowProteinWeightMat->addButton(slowProtAAUserMat);
    
    protMatLayout = new QGridLayout; 
    protMatLayout->addWidget(slowProtBlosum30, 0, 0);
    protMatLayout->addWidget(slowProtPAM350, 0, 1);
    protMatLayout->addWidget(slowProtGonnet250, 0, 2);
    protMatLayout->addWidget(slowProtIdentity, 1, 0);
    protMatLayout->addWidget(slowProtAAUserMat, 1, 1);
    slowProtMatGroup->setLayout(protMatLayout);    
}

void PairwiseParams::setInitialProteinMat()
{
    if((clustalw::subMatrix->getPWMatrixNum() - 1) == clustalw::PWAABLOSUM)
    {
        slowProtBlosum30->setChecked(true);
    }
    else if((clustalw::subMatrix->getPWMatrixNum() - 1) == clustalw::PWAAPAM)
    {
        slowProtPAM350->setChecked(true);
    }
    else if((clustalw::subMatrix->getPWMatrixNum() - 1) == clustalw::PWAAGONNET)
    {
        slowProtGonnet250->setChecked(true);
    }
    else if((clustalw::subMatrix->getPWMatrixNum() - 1) == clustalw::PWAAIDENTITY)
    {
        slowProtIdentity->setChecked(true);
    }
    else if((clustalw::subMatrix->getPWMatrixNum() - 1) == clustalw::PWAAUSER)
    {
        slowProtAAUserMat->setChecked(true);
    } 
}

void PairwiseParams::setUpSlowDNAWeightMat()
{
    slowDNAMatGroup = new QGroupBox(tr("DNA Weight Matrix")); 
    slowDNAWeightMat = new QButtonGroup;
    slowDNAWeightMat->setExclusive(true);
    slowDNAIUB = new QRadioButton("IUB");
    slowDNAClustalw = new QRadioButton("CLUSTALW(1.6)");
    slowDNAUser = new QRadioButton("User defined");
    connect(slowDNAUser, SIGNAL(clicked()), this, SLOT(userDNAClicked()));    
    
    slowDNAWeightMat->addButton(slowDNAIUB);
    slowDNAWeightMat->addButton(slowDNAClustalw);
    slowDNAWeightMat->addButton(slowDNAUser);
    
    dnaMatLayout = new QGridLayout; 
    dnaMatLayout->addWidget(slowDNAIUB, 0, 0);
    dnaMatLayout->addWidget(slowDNAClustalw, 0, 1);
    dnaMatLayout->addWidget(slowDNAUser, 0, 2);
    slowDNAMatGroup->setLayout(dnaMatLayout);
}

void PairwiseParams::setInitialDNAMat()
{
    if((clustalw::subMatrix->getPWDNAMatrixNum() - 1) == clustalw::DNAIUB)
    {
        slowDNAIUB->setChecked(true);
    }
    else if((clustalw::subMatrix->getPWDNAMatrixNum() - 1) == clustalw::DNACLUSTALW)
    {
        slowDNAClustalw->setChecked(true);
    }
    else if((clustalw::subMatrix->getPWDNAMatrixNum() - 1) == clustalw::DNAUSERDEFINED)
    {
        slowDNAUser->setChecked(true);
    }
}

void PairwiseParams::setUpFastTab()
{   
    fastGapPenaltyLabel = new QLabel(tr("Gap Penalty [1-500]: "));
    fastGapPenalty = new QLineEdit;
    
    fastKTupSizeLabel = new QLabel(tr("K-Tuple Size [1-2]: "));
    fastKTupSize = new QLineEdit;
    
    fastTopDiagonalsLabel = new QLabel(tr("Top Diagonals [1-50]: "));
    fastTopDiagonals = new QLineEdit;
    
    fastWindowSizeLabel = new QLabel(tr("Window Size [1-50]: "));
    fastWindowSize = new QLineEdit;  
    
    fastTabLayout = new QGridLayout; 
    fastTabLayout->addWidget(fastGapPenaltyLabel, 0, 0);
    fastTabLayout->addWidget(fastGapPenalty, 0, 1);
    fastTabLayout->addWidget(fastKTupSizeLabel, 1, 0);
    fastTabLayout->addWidget(fastKTupSize, 1, 1);  
    fastTabLayout->addWidget(fastTopDiagonalsLabel, 2, 0);
    fastTabLayout->addWidget(fastTopDiagonals, 2, 1);
    fastTabLayout->addWidget(fastWindowSizeLabel, 3, 0);
    fastTabLayout->addWidget(fastWindowSize, 3, 1);
    
    fastPairwiseTab->setLayout(fastTabLayout);     
}

void PairwiseParams::setFastInitialTextParams()
{
    QString num;
    num = num.setNum(clustalw::userParameters->getWindowGap());
    fastGapPenalty->setText(num);

    num = num.setNum(clustalw::userParameters->getKtup());
    fastKTupSize->setText(num);
    
    num = num.setNum(clustalw::userParameters->getSignif());
    fastTopDiagonals->setText(num);

    num = num.setNum(clustalw::userParameters->getWindow());
    fastWindowSize->setText(num);                
}

void PairwiseParams::radioSetUsingUserProtein()
{
    slowProtAAUserMat->setChecked(true);
}

void PairwiseParams::slowBrowseProtein()
{
    // Mark Jan 18th 2007: changes to remember working Dir
    FileDialog fd;
    slowAAMatrixPath = fd.getOpenFileName(this, tr("Find Protein Matrix"));
                          
    string path = slowAAMatrixPath.toStdString();
    if(path != "")
    {
        int lenOfString = path.length();
        
        char* userFile = new char[lenOfString + 1];
        strcpy(userFile, path.c_str());
        
        if (clustalw::subMatrix->getUserMatFromFile(userFile, clustalw::Protein,
                                                    clustalw::Pairwise))
        {
            aaMatrixFile = string(userFile);
            proteinMatLoaded = true;
            userProteinMatrix->setText(aaMatrixFile.c_str());
            radioSetUsingUserProtein();           
        }
        else
        {
            aaMatrixFile = "";
            proteinMatLoaded = false;
            userProteinMatrix->setText(aaMatrixFile.c_str());
            setInitialProteinMat();                         
        }
        delete []userFile;
    }
    else // pressed cancel
    {
        if(!proteinMatLoaded)
        {
            setInitialProteinMat();
        }
    }                                   
}

void PairwiseParams::slowBrowseDNA()
{
     // Mark Jan 18th 2007: changes to remember working Dir
    FileDialog fd;
    slowDNAMatrixPath = fd.getOpenFileName(this, tr("Find DNA Matrix"));
    
    string path = slowDNAMatrixPath.toStdString();
                                   
    if(path != "")
    {
        int lenOfString = path.length();
        
        char* userFile = new char[lenOfString + 1];
        strcpy(userFile, path.c_str());
        
        if (clustalw::subMatrix->getUserMatFromFile(userFile, clustalw::DNA,
                                                    clustalw::Pairwise))
        {
            dnaMatrixFile = string(userFile);
            dnaMatLoaded = true;
            userDNAMatrix->setText(dnaMatrixFile.c_str());
            slowDNAUser->setChecked(true);
        }
        else
        {
            dnaMatrixFile = "";
            dnaMatLoaded = false; 
            setInitialDNAMat();
            userDNAMatrix->setText(dnaMatrixFile.c_str());                        
        }
        delete []userFile;
    }
    else // pressed cancel
    {
        if(!dnaMatLoaded)
        {
            setInitialDNAMat();
        }
    }                                   
}

void PairwiseParams::accept()
{
    
    if(slowOrQuick->currentIndex() == 1)
    {
        clustalw::userParameters->setQuickPairAlign(true);
    }
    else
    {
        clustalw::userParameters->setQuickPairAlign(false);
    }
    
    // Protein matrices
    if(slowProtBlosum30->isChecked())
    {
        //slowProtBlosum30->setChecked(true);
        clustalw::subMatrix->setCurrentNameAndNum("blosum", 1, clustalw::Protein,
                                                  clustalw::Pairwise);
    }
    else if(slowProtPAM350->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("pam", 2, clustalw::Protein,
                                                  clustalw::Pairwise);
    }
    else if(slowProtGonnet250->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("gonnet", 3, clustalw::Protein,
                                                  clustalw::Pairwise);
    }
    else if(slowProtIdentity->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("id", 4, clustalw::Protein,
                                                  clustalw::Pairwise);
    }
    else if(slowProtAAUserMat->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum(aaMatrixFile, 5, clustalw::Protein,
                                                  clustalw::Pairwise);
    }
    
    // DNA Matrix
    if(slowDNAIUB->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("iub", 1, clustalw::DNA,
                                                  clustalw::Pairwise);
    }
    else if(slowDNAClustalw->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("clustalw", 2, clustalw::DNA,
                                                  clustalw::Pairwise);
    }
    else if(slowDNAUser->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum(dnaMatrixFile, 3, clustalw::DNA,
                                                  clustalw::Pairwise);
    }    
    // Check slowGapOpen 0-100
    bool ok;
    QString sGapOpen = slowGapOpen->text();
    float sgapOpen;
    sgapOpen = sGapOpen.toFloat(&ok);   
 
    if(ok && sgapOpen >= 0.0 && sgapOpen <= 100.0)
    {
        if(clustalw::userParameters->getDNAFlag())
        {
            clustalw::userParameters->setDNAPWGapOpenPenalty(sgapOpen);
        }
        else
        {
            clustalw::userParameters->setAAPWGapOpen(sgapOpen);
        }
    }       
    
    // Check slowGapExtend 0-100
    QString sGapExtend = slowGapExtend->text();
    float sgapExtend;
    sgapExtend = sGapExtend.toFloat(&ok);   

    if(ok && sgapExtend >= 0.0 && sgapExtend <= 100.0)
    {
        if(clustalw::userParameters->getDNAFlag())
        {
            clustalw::userParameters->setDNAPWGapExtendPenalty(sgapExtend);
        }
        else
        {
            clustalw::userParameters->setAAPWGapExtend(sgapExtend);
        }
    }
            
    // Check fastGapPenalty 1-500
    QString fastGapPenString = fastGapPenalty->text();
    int fastGPen;
    fastGPen = fastGapPenString.toInt(&ok);
       
    if(ok && fastGPen >= 1 && fastGPen <= 500)
    {
        if(clustalw::userParameters->getDNAFlag())
        {
            clustalw::userParameters->setDNAWindowGap(fastGPen);
        }
        else
        {
            clustalw::userParameters->setAAWindowGap(fastGPen);
        }
    }

        
    // Check fastKTupSize 1-2
    QString fastKTupString = fastKTupSize->text();
    int fastKTup;
    fastKTup = fastKTupString.toInt(&ok);
       
    if(ok && fastKTup >= 1 && fastKTup <= 2)
    {
        if(clustalw::userParameters->getDNAFlag())
        {
            clustalw::userParameters->setDNAKtup(fastKTup);
        }
        else
        {
            clustalw::userParameters->setAAKtup(fastKTup);
        }
    }
        
    // Check fastTopDiagonals 1-50
    QString fastTopDiagonalsString = fastTopDiagonals->text();
    int fastTopDiag;
    fastTopDiag = fastTopDiagonalsString.toInt(&ok);
       
    if(ok && fastTopDiag >= 1 && fastTopDiag <= 50)
    {
        if(clustalw::userParameters->getDNAFlag())
        {
            clustalw::userParameters->setDNASignif(fastTopDiag);
        }
        else
        {
            clustalw::userParameters->setAASignif(fastTopDiag);
        }
    }
        
    // Check fastWindowSize 1-50
    QString fastWindowSizeString = fastWindowSize->text();
    int fastWinSize;
    fastWinSize = fastWindowSizeString.toInt(&ok);
       
    if(ok && fastWinSize >= 1 && fastWinSize <= 50)
    {
        if(clustalw::userParameters->getDNAFlag())
        {
            clustalw::userParameters->setDNAWindow(fastWinSize);
        }
        else
        {
            clustalw::userParameters->setAAWindow(fastWinSize);
        }
    }
    
    // NOTE it is important to set the parameters here.
    if(clustalw::userParameters->getDNAFlag())
    {
        clustalw::userParameters->setDNAParams();
    }
    else
    {
        clustalw::userParameters->setProtParams();
    }        
    setResult(QDialog::Accepted);
    hide();
}
