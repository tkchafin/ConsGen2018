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
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QComboBox>
#include <QtGui>
#include <QLabel>
#include <QButtonGroup>
#include <QLineEdit>
#include <QSlider>
#include <QLCDNumber>
#include "LowScoringSegParams.h"
#include "clustalW/substitutionMatrix/globalmatrix.h"
#include "FileDialog.h"

LowScoringSegParams::LowScoringSegParams()
{
    proteinMatLoaded = false;
    dnaMatLoaded = false;
        
    sliderGridBox = new QGroupBox;
    proteinMatGridBox = new QGroupBox(tr("Protein Weight Matrix"));
    dnaMatGridBox = new QGroupBox(tr("DNA Weight Matrix"));
    
    mainLayout = new QVBoxLayout;
     
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    
    setUpLayout();
    
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(sliderGridBox);
    mainLayout->addWidget(proteinMatGridBox);
    mainLayout->addWidget(loadProteinGroup);
    mainLayout->addWidget(dnaMatGridBox);
    mainLayout->addWidget(loadDNAGroup);
    setLayout(mainLayout);
}

void LowScoringSegParams::showEvent(QShowEvent* event)
{
    setInitialProteinMat();
    setInitialParamValues();
    setInitialDNAMat();
}

void LowScoringSegParams::setUpLayout()
{
    setUpOtherParams();
    grid = new QGridLayout; 
    grid->addWidget(minLenSegsLabel, 0, 0);
    grid->addWidget(minLenSegsLCD, 0, 1);
    grid->addWidget(minLenSegs, 0, 2);
    grid->addWidget(dnaMarkingScaleLabel, 1, 0);
    grid->addWidget(dnaMarkingScaleLCD, 1, 1);  
    grid->addWidget(dnaMarkingScale, 1, 2); 
    sliderGridBox->setLayout(grid);
   
    setUpProteinMatRadioButtons();
    protMatLayout = new QGridLayout; 
    protMatLayout->addWidget(gonnetPAM80, 0, 0);
    protMatLayout->addWidget(gonnetPAM120, 0, 1);
    protMatLayout->addWidget(gonnetPAM250, 0, 2);
    protMatLayout->addWidget(gonnetPAM350, 1, 0);
    protMatLayout->addWidget(aaUserMat, 1, 1);
    proteinMatGridBox->setLayout(protMatLayout);
   
    setUpDNAMatRadioButtons();
    dnaMatLayout = new QGridLayout; 
    dnaMatLayout->addWidget(IUB, 0, 0);
    dnaMatLayout->addWidget(clustal, 0, 1);
    dnaMatLayout->addWidget(dnaUserMat, 1, 0);
    dnaMatGridBox->setLayout(dnaMatLayout);     
}

void LowScoringSegParams::userProteinClicked()
{
    if(!proteinMatLoaded)
    {
        browseProtein();
    }
}

void LowScoringSegParams::userDNAClicked()
{
    if(!dnaMatLoaded)
    {
        browseDNA();
    }
}

void LowScoringSegParams::setUpProteinMatRadioButtons()
{
    proteinWeightMat = new QButtonGroup;
    gonnetPAM80 = new QRadioButton("Gonnet PAM 80");
    gonnetPAM120 = new QRadioButton("Gonnet PAM 120");
    gonnetPAM250 = new QRadioButton("Gonnet PAM 250");
    gonnetPAM350 = new QRadioButton("Gonnet PAM 350");
    aaUserMat = new QRadioButton("User defined");
    
    connect(aaUserMat, SIGNAL(clicked()), this, SLOT(userProteinClicked())); 
     
    proteinWeightMat->addButton(gonnetPAM80);
    proteinWeightMat->addButton(gonnetPAM120);
    proteinWeightMat->addButton(gonnetPAM250);
    proteinWeightMat->addButton(gonnetPAM350);  
    proteinWeightMat->addButton(aaUserMat);                  
}

void LowScoringSegParams::setInitialProteinMat()
{
    if(clustalw::subMatrix->getQTsegmentAAMatNum() == clustalw::QTAASEGUSER)
    {
        aaUserMat->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTsegmentAAMatNum() == clustalw::QTAASEGGONNETPAM80)
    {
        gonnetPAM80->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTsegmentAAMatNum() == clustalw::QTAASEGGONNETPAM120)
    {
        gonnetPAM120->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTsegmentAAMatNum() == clustalw::QTAASEGGONNETPAM250)
    {
        gonnetPAM250->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTsegmentAAMatNum() == clustalw::QTAASEGGONNETPAM350)
    {
        gonnetPAM350->setChecked(true);
    }            
}

void LowScoringSegParams::setUpDNAMatRadioButtons()
{
    DNAWeightMat = new QButtonGroup;
    IUB = new QRadioButton("IUB");
    clustal = new QRadioButton("CLUSTALW(1.6)");
    dnaUserMat = new QRadioButton("User defined");
    connect(dnaUserMat, SIGNAL(clicked()), this, SLOT(userDNAClicked()));
    
    DNAWeightMat->addButton(IUB);
    DNAWeightMat->addButton(clustal);
    DNAWeightMat->addButton(dnaUserMat);
}

void LowScoringSegParams::setInitialDNAMat()
{
    if(clustalw::subMatrix->getQTsegmentDNAMatNum() == clustalw::DNAIUB)
    {
        IUB->setChecked(true); 
    }
    else if(clustalw::subMatrix->getQTsegmentDNAMatNum() == clustalw::DNACLUSTALW)
    {
        clustal->setChecked(true); 
    }
    else if(clustalw::subMatrix->getQTsegmentDNAMatNum() == clustalw::DNAUSERDEFINED)
    {
        dnaUserMat->setChecked(true); 
    }    
}

void LowScoringSegParams::setUpOtherParams()
{
    minLenSegs = new QSlider(Qt::Horizontal);
    minLenSegs->setRange(1, 20);  
    minLenSegsLabel = new QLabel(tr("Minimum Length of Segments ")); 
    minLenSegsLCD = new QLCDNumber;
    connect(minLenSegs, SIGNAL(valueChanged(int)), minLenSegsLCD, SLOT(display(int)));
               
    dnaMarkingScale = new QSlider(Qt::Horizontal);
    dnaMarkingScale->setRange(1, 10);  
    dnaMarkingScaleLabel = new QLabel(tr("DNA Marking Scale "));                     
    dnaMarkingScaleLCD = new QLCDNumber;
    connect(dnaMarkingScale, SIGNAL(valueChanged(int)), dnaMarkingScaleLCD,
             SLOT(display(int)));
    
                             
    loadProtMat = new QPushButton(tr("Load protein matrix:"));
    loadProtMat->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    connect(loadProtMat, SIGNAL(clicked()), this, SLOT(browseProtein()));
    
    loadProteinGroup = new QGroupBox;
    horizLayoutAA = new QHBoxLayout; 
    userProteinMatrix = new QLabel("");
    horizLayoutAA->addWidget(loadProtMat);
    horizLayoutAA->addWidget(userProteinMatrix);
    loadProteinGroup->setLayout(horizLayoutAA);
        
    loadDNAMat = new QPushButton(tr("Load DNA matrix:"));
    loadDNAMat->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);    
    connect(loadDNAMat, SIGNAL(clicked()), this, SLOT(browseDNA()));
    
    loadDNAGroup = new QGroupBox;
    horizLayoutDNA = new QHBoxLayout;
    userDNAMatrix = new QLabel("");
    horizLayoutDNA->addWidget(loadDNAMat);
    horizLayoutDNA->addWidget(userDNAMatrix);
    loadDNAGroup->setLayout(horizLayoutDNA);    
}

void LowScoringSegParams::setInitialParamValues()
{
    minLenSegs->setValue(clustalw::userParameters->getQTminLenLowScoreSegment());
    minLenSegsLCD->display(minLenSegs->value()); 
    
    dnaMarkingScale->setValue(clustalw::userParameters->getQTlowScoreDNAMarkingScale());
    dnaMarkingScaleLCD->display(dnaMarkingScale->value()); 
}

void LowScoringSegParams::radioSetUsingUserProtein()
{
    aaUserMat->setChecked(true);
}

void LowScoringSegParams::browseProtein()
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
        
        if (clustalw::subMatrix->getQTLowScoreMatFromFile(userFile, false))
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

void LowScoringSegParams::browseDNA()
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
        
        if (clustalw::subMatrix->getQTLowScoreMatFromFile(userFile, true))
        {
            dnaMatrixFile = string(userFile);
            dnaMatLoaded = true;
            userDNAMatrix->setText(dnaMatrixFile.c_str());
            dnaUserMat->setChecked(true);
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

void LowScoringSegParams::accept()
{
    clustalw::userParameters->setQTminLenLowScoreSegment(minLenSegs->value());
    
    clustalw::userParameters->setQTlowScoreDNAMarkingScale(dnaMarkingScale->value());
    
    if(aaUserMat->isChecked())
    {
        clustalw::subMatrix->setQTsegmentAAMatNum(clustalw::QTAASEGUSER);
    }
    else if(gonnetPAM80->isChecked())
    {
        clustalw::subMatrix->setQTsegmentAAMatNum(clustalw::QTAASEGGONNETPAM80);
    }
    else if(gonnetPAM120->isChecked())
    {
        clustalw::subMatrix->setQTsegmentAAMatNum(clustalw::QTAASEGGONNETPAM120);
    }
    else if(gonnetPAM250->isChecked())
    {
        clustalw::subMatrix->setQTsegmentAAMatNum(clustalw::QTAASEGGONNETPAM250);
    }
    else if(gonnetPAM350->isChecked())
    {
        clustalw::subMatrix->setQTsegmentAAMatNum(clustalw::QTAASEGGONNETPAM350);
    }  
    
    if(IUB->isChecked())
    { 
        clustalw::subMatrix->setQTsegmentDNAMatNum(clustalw::DNAIUB);
    }
    else if(clustal->isChecked())
    { 
        clustalw::subMatrix->setQTsegmentDNAMatNum(clustalw::DNACLUSTALW);
    }
    else if(dnaUserMat->isChecked())
    {
        clustalw::subMatrix->setQTsegmentDNAMatNum(clustalw::DNAUSERDEFINED); 
    }
            
    setResult(QDialog::Accepted);
    hide();
}
