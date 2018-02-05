/**
 * Changes:
 * Mark: 18-01-2007. Changed from using QFileDialog to FileDialog.
 *
 *
 */
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QComboBox>
#include <QtGui>
#include <QLabel>
#include <QButtonGroup>
#include <QLineEdit>
#include <QHBoxLayout>
#include <cstring>
#include "AlignmentParameters.h"
#include "clustalW/general/userparams.h"
#include "clustalW/substitutionMatrix/globalmatrix.h"
#include "FileDialog.h"

AlignmentParameters::AlignmentParameters()
{   
    setWindowTitle("Alignment Parameters");
    proteinMatLoaded = false;
    dnaMatLoaded = false;
    
    paramGridBox = new QGroupBox;
    proteinMatGridBox = new QGroupBox(tr("Protein Weight Matrix"));
    dnaMatGridBox = new QGroupBox(tr("DNA Weight Matrix"));
    verticalBox = new QGroupBox(tr("Multiple Parameters"));
    mainLayout = new QVBoxLayout;
     
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    
    setUpLayout();
    
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(verticalBox);
    setLayout(mainLayout);
}

void AlignmentParameters::showEvent(QShowEvent* event)
{
    setInitialProteinMat();
    setInitialDNAMat();
    setInitialParams();
}

void AlignmentParameters::setUpLayout()
{
    setUpOtherParams();
    grid = new QGridLayout; 
    grid->addWidget(gapOpenLabel, 0, 0);
    grid->addWidget(gapOpen, 0, 1);
    grid->addWidget(gapExtendLabel, 1, 0);
    grid->addWidget(gapExtend, 1, 1);
    grid->addWidget(delayDivergentLabel, 2, 0);
    grid->addWidget(delayDivergent, 2, 1);
    grid->addWidget(dnaTranWeightLabel, 3, 0);
    grid->addWidget(dnaTranWeight, 3, 1);
    grid->addWidget(useNegMatLabel, 4, 0);
    grid->addWidget(useNegMat, 4, 1);   
    paramGridBox->setLayout(grid);
   
    setUpProteinMatRadioButtons();
    protMatLayout = new QGridLayout; 
    protMatLayout->addWidget(blosum, 0, 0);
    protMatLayout->addWidget(pam, 0, 1);
    protMatLayout->addWidget(gonnet, 1, 0);
    protMatLayout->addWidget(identity, 1, 1);
    protMatLayout->addWidget(userProt, 0, 2);
    proteinMatGridBox->setLayout(protMatLayout);
   
    setUpDNAMatRadioButtons();
    dnaMatLayout = new QGridLayout; 
    dnaMatLayout->addWidget(IUB, 0, 0);
    dnaMatLayout->addWidget(clustal, 0, 1);
    dnaMatLayout->addWidget(userDNA, 0, 2);
    dnaMatGridBox->setLayout(dnaMatLayout); 

    setUpLoadButtons();                   
    multipleParamLayout = new QVBoxLayout;
    multipleParamLayout->addWidget(paramGridBox);
    multipleParamLayout->addWidget(proteinMatGridBox);
    multipleParamLayout->addWidget(loadProteinGroup);
    multipleParamLayout->addWidget(dnaMatGridBox);
    multipleParamLayout->addWidget(loadDNAGroup);
    verticalBox->setLayout(multipleParamLayout);     
}

void AlignmentParameters::setUpProteinMatRadioButtons()
{
    proteinMat = new QButtonGroup;
    proteinMat->setExclusive(true);
    blosum = new QRadioButton("BLOSUM series");
    pam = new QRadioButton("PAM series");
    gonnet = new QRadioButton("Gonnet series");
    identity = new QRadioButton("Identity matrix");
    userProt = new QRadioButton("User defined");

    connect(userProt, SIGNAL(clicked()), this, SLOT(userProteinClicked()));        
     
    proteinMat->addButton(blosum);
    proteinMat->addButton(pam);
    proteinMat->addButton(gonnet);
    proteinMat->addButton(identity);
    proteinMat->addButton(userProt);                   
}

void AlignmentParameters::setInitialProteinMat()
{
    if((clustalw::subMatrix->getMatrixNum() - 1) == clustalw::AABLOSUM)
    {
        blosum->setChecked(true);
    }
    else if((clustalw::subMatrix->getMatrixNum() - 1) == clustalw::AAPAM)
    {
        pam->setChecked(true);
    }
    else if((clustalw::subMatrix->getMatrixNum() - 1) == clustalw::AAGONNET)
    {
        gonnet->setChecked(true);
    }
    else if((clustalw::subMatrix->getMatrixNum() - 1) == clustalw::AAIDENTITY)
    {
        identity->setChecked(true);
    }
    else if((clustalw::subMatrix->getMatrixNum() - 1) == clustalw::AAUSERDEFINED)
    {
        userProt->setChecked(true);
    }
}

void AlignmentParameters::radioSetUsingUserProtein()
{
    userProt->setChecked(true);
    blosum->setChecked(false);
    pam->setChecked(false);
    gonnet->setChecked(false);
    identity->setChecked(false);
}

void AlignmentParameters::userProteinClicked()
{   
    if(!proteinMatLoaded)
    {
        browseProtein();
    }
}

void AlignmentParameters::setUpDNAMatRadioButtons()
{
    DNAMat = new QButtonGroup;
    DNAMat->setExclusive(true);
    IUB = new QRadioButton("IUB");
    clustal = new QRadioButton("CLUSTALW(1.6)");
    userDNA = new QRadioButton("User defined");

    connect(userDNA, SIGNAL(clicked()), this, SLOT(userDNAClicked()));
            
    DNAMat->addButton(IUB);
    DNAMat->addButton(clustal);
    DNAMat->addButton(userDNA);
}

void AlignmentParameters::setInitialDNAMat()
{
    if((clustalw::subMatrix->getDNAMatrixNum() - 1) == clustalw::DNAIUB)
    {
        IUB->setChecked(true);
    }
    else if((clustalw::subMatrix->getDNAMatrixNum() - 1) == clustalw::DNACLUSTALW)
    {
        clustal->setChecked(true);
    }
    else if((clustalw::subMatrix->getDNAMatrixNum() - 1) == clustalw::DNAUSERDEFINED)
    {
        userDNA->setChecked(true);
    }
}

void AlignmentParameters::userDNAClicked()
{
    if(!dnaMatLoaded)
    {
        browseDNA();
    }
}

void AlignmentParameters::setUpOtherParams()
{
    // Need to intialy set the parameters to be either DNA or protein.
    // Could be setting DNA if the alignment we read in is DNA.
    
    gapOpen = new QLineEdit;
    gapOpenLabel = new QLabel(tr("Gap Opening [0-100]: "));
    
    gapExtend = new QLineEdit;
    gapExtendLabel = new QLabel(tr("Gap Extension [0-100]: "));
    
    delayDivergent = new QLineEdit;    
    delayDivergentLabel = new QLabel(tr("Delay Divergent Sequences(%): "));
    
    dnaTranWeight = new QLineEdit;    
    dnaTranWeightLabel = new QLabel(tr("DNA Transition Weight[0-1] "));                 
    
    useNegMat = new QComboBox();
    useNegMat->addItem("Off");    
    useNegMat->addItem("On");
    useNegMatLabel = new QLabel(tr("Use Negative Matrix "));
    
    loadProtMat = new QPushButton(tr("Load protein matrix:"));
    loadProtMat->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(loadProtMat, SIGNAL(clicked()), this, SLOT(browseProtein()));
    
    loadDNAMat = new QPushButton(tr("Load DNA matrix:"));
    loadDNAMat->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);    
    connect(loadDNAMat, SIGNAL(clicked()), this, SLOT(browseDNA()));
}


void AlignmentParameters::setInitialParams()
{
    if(clustalw::userParameters->getDNAFlag())
    {
        clustalw::userParameters->setDNAParams();
    }
    else
    {
        clustalw::userParameters->setProtParams();
    }
    
    QString num;
    num = num.setNum(clustalw::userParameters->getGapOpen());
    gapOpen->setText(num);
    
    num = num.setNum(clustalw::userParameters->getGapExtend());    
    gapExtend->setText(num);
    
    num = num.setNum(clustalw::userParameters->getDivergenceCutoff());    
    delayDivergent->setText(num);
    
    num = num.setNum(clustalw::userParameters->getTransitionWeight());    
    dnaTranWeight->setText(num);
    
    if(clustalw::userParameters->getUseNegMatrix())
    {
        useNegMat->setCurrentIndex(1);
    }
    else
    {
        useNegMat->setCurrentIndex(0);
    }
}

void AlignmentParameters::setUpLoadButtons()
{
    loadProteinGroup = new QGroupBox;
    horizLayoutAA = new QHBoxLayout; 
    userProteinMatrix = new QLabel("");
    horizLayoutAA->addWidget(loadProtMat);
    horizLayoutAA->addWidget(userProteinMatrix);
    loadProteinGroup->setLayout(horizLayoutAA); 

    loadDNAGroup = new QGroupBox;
    horizLayoutDNA = new QHBoxLayout;
    userDNAMatrix = new QLabel("");
    horizLayoutDNA->addWidget(loadDNAMat);
    horizLayoutDNA->addWidget(userDNAMatrix);
    loadDNAGroup->setLayout(horizLayoutDNA);
}

void AlignmentParameters::browseProtein()
{   
    // Mark Jan 18th 2007: changes to remember working Dir
    FileDialog fd;
    aaMatrixPath = fd.getOpenFileName(this, tr("Find Protein Matrix"));
                    
    string path = aaMatrixPath.toStdString();
                                   
    if(path != "")
    {
        int lenOfString = path.length();
        
        char* userFile = new char[lenOfString + 1];
        strcpy(userFile, path.c_str());
        
        if (clustalw::subMatrix->getUserMatFromFile(userFile, clustalw::Protein,
                                                    clustalw::MultipleAlign))
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
    else // we pressed cancel
    {
        if(!proteinMatLoaded)
        {
            setInitialProteinMat();
        }
    }                                  
}

void AlignmentParameters::browseDNA()
{
    // Mark Jan 18th 2007: changes to remember working Dir
    FileDialog fd;
    dnaMatrixPath = fd.getOpenFileName(this, tr("Find DNA Matrix"));
               
    string path = dnaMatrixPath.toStdString();
                                   
    if(path != "")
    {
        int lenOfString = path.length();
        
        char* userFile = new char[lenOfString + 1];
        strcpy(userFile, path.c_str());
        
        if (clustalw::subMatrix->getUserMatFromFile(userFile, clustalw::DNA,
                                                    clustalw::MultipleAlign))
        {
            dnaMatrixFile = string(userFile);
            dnaMatLoaded = true;
            userDNAMatrix->setText(dnaMatrixFile.c_str());
            userDNA->setChecked(true);
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
    else // we pressed cancel
    {
        if(!dnaMatLoaded)
        {
            setInitialDNAMat();
        }
    }                                       
}

void AlignmentParameters::accept()
{
    bool ok;
    // check _gapExtention
    QString gExtend = gapExtend->text();
    float extend;
    extend = gExtend.toFloat(&ok);   

    if(ok && extend >= 0.0 && extend <= 100.0)
    {
        if(clustalw::userParameters->getDNAFlag())
        {
            clustalw::userParameters->setDNAGapExtend(extend);
        }
        else
        {
            clustalw::userParameters->setAAGapExtend(extend);
        }
    }  
      
    // check _gapOpen
    QString gOpen = gapOpen->text();
    float open;
    open = gOpen.toFloat(&ok);   

    if(ok && open >= 0.0 && open <= 100.0)
    {
        if(clustalw::userParameters->getDNAFlag())
        {
            clustalw::userParameters->setDNAGapOpen(open);
        }
        else
        {
            clustalw::userParameters->setAAGapOpen(open);
        }
    }
        
    // check _delayDivSeq
    QString divPc = delayDivergent->text();
    int delay;
    delay = divPc.toInt(&ok);   

    if(ok && delay >= 0 && delay <= 100)
    {
        clustalw::userParameters->setDivergenceCutoff(delay);
    }
        
    // check _DNATranWeight
    QString tWeight = dnaTranWeight->text();
    float tranWeight;
    tranWeight = tWeight.toFloat(&ok);
    
    if(ok && tranWeight >= 0.0 && tranWeight <= 1.0)
    {
        clustalw::userParameters->setTransitionWeight(tranWeight);
    }

    // Protein Weight Matrix
    if(blosum->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("blosum", 1, 
                                            clustalw::Protein, clustalw::MultipleAlign); 
    }
    else if(pam->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("pam", 2, 
                                            clustalw::Protein, clustalw::MultipleAlign); 
    }
    else if(gonnet->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("gonnet", 3, 
                                            clustalw::Protein, clustalw::MultipleAlign); 
    }
    else if(identity->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("id", 4, 
                                            clustalw::Protein, clustalw::MultipleAlign); 
    }
    else if(userProt->isChecked())
    {
        if(aaMatrixFile != "")
        {
            clustalw::subMatrix->setCurrentNameAndNum(aaMatrixFile, 5, 
                                            clustalw::Protein, clustalw::MultipleAlign);
        }
        else // Set to a default if a problem with the 
        {
            clustalw::subMatrix->setCurrentNameAndNum("gonnet", 3, 
                                            clustalw::Protein, clustalw::MultipleAlign);   
        }    
    }

    // DNA matrix
    if(IUB->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("iub", 1, clustalw::DNA,
                                                  clustalw::MultipleAlign);
    }
    else if(clustal->isChecked())
    {
        clustalw::subMatrix->setCurrentNameAndNum("clustalw", 2, clustalw::DNA,
                                                  clustalw::MultipleAlign);
    }
    else if(userDNA->isChecked())
    {
        if(dnaMatrixFile != "")
        {
            clustalw::subMatrix->setCurrentNameAndNum(dnaMatrixFile, 3, 
                                                    clustalw::DNA, clustalw::MultipleAlign);
        }
        else // Set it to the default
        {
            clustalw::subMatrix->setCurrentNameAndNum("iub", 1, clustalw::DNA,
                                                  clustalw::MultipleAlign);
        }
    }
    if(useNegMat->currentIndex() == 1)
    {
        clustalw::userParameters->setUseNegMatrix(true);   
    }
    else
    {
        clustalw::userParameters->setUseNegMatrix(false);    
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

