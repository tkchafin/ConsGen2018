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
#include <QSlider>
#include <QLCDNumber>
#include <QHBoxLayout>
#include "ColumnScoreParams.h"
#include "clustalW/general/userparams.h"
#include "clustalW/substitutionMatrix/globalmatrix.h"
#include "FileDialog.h"

ColumnScoreParams::ColumnScoreParams()
{
    proteinMatLoaded = false;
    dnaMatLoaded = false;
    
    sliderGridBox = new QGroupBox;
    proteinMatGridBox = new QGroupBox(tr("Protein Weight Matrix"));
    dnaMatGridBox = new QGroupBox(tr("DNA Weight Matrix"));
    QVBoxLayout* mainLayout = new QVBoxLayout;
     
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

void ColumnScoreParams::showEvent(QShowEvent* event)
{
    setInitialProteinMat();
    setInitialParamValues();
    setInitialDNAMat();
}

void ColumnScoreParams::setUpLayout()
{
    setUpLoadButtons();     
    setUpOtherParams();
    grid = new QGridLayout; 
    grid->addWidget(scorePlotScale, 0, 0);
    grid->addWidget(plotValue, 0, 1);
    grid->addWidget(plotScale, 0, 2);
    grid->addWidget(residueExceptionCutoff, 1, 0);
    grid->addWidget(cutOffValue, 1, 1);  
    grid->addWidget(cutOff, 1, 2); 
    sliderGridBox->setLayout(grid);
   
    setUpProteinMatRadioButtons();
    protMatLayout = new QGridLayout; 
    protMatLayout->addWidget(identity, 0, 0);
    protMatLayout->addWidget(gonnetPAM80, 0, 1);
    protMatLayout->addWidget(gonnetPAM120, 0, 2);
    protMatLayout->addWidget(gonnetPAM250, 1, 0);
    protMatLayout->addWidget(gonnetPAM350, 1, 1);
    protMatLayout->addWidget(aaUserMat, 1, 2);
    proteinMatGridBox->setLayout(protMatLayout);
   
    setUpDNAMatRadioButtons();
    dnaMatLayout = new QGridLayout; 
    dnaMatLayout->addWidget(IUB, 0, 0);
    dnaMatLayout->addWidget(clustal, 0, 1);
    dnaMatLayout->addWidget(dnaUserMat, 1, 0);
    dnaMatGridBox->setLayout(dnaMatLayout);  
      
}



void ColumnScoreParams::setUpProteinMatRadioButtons()
{
    proteinWeightMat = new QButtonGroup;
    identity = new QRadioButton("Identity");
    gonnetPAM80 = new QRadioButton("Gonnet PAM 80");
    gonnetPAM120 = new QRadioButton("Gonnet PAM 120");
    gonnetPAM250 = new QRadioButton("Gonnet PAM 250");
    gonnetPAM350 = new QRadioButton("Gonnet PAM 350");
    aaUserMat = new QRadioButton("User defined");
    
    connect(aaUserMat, SIGNAL(clicked()), this, SLOT(userProteinClicked()));     

    proteinWeightMat->addButton(identity);
    proteinWeightMat->addButton(gonnetPAM80);
    proteinWeightMat->addButton(gonnetPAM120);
    proteinWeightMat->addButton(gonnetPAM250);
    proteinWeightMat->addButton(gonnetPAM350);  
    proteinWeightMat->addButton(aaUserMat);                  
}

void ColumnScoreParams::userProteinClicked()
{
    if(!proteinMatLoaded)
    {
        browseProtein();
    }
}

void ColumnScoreParams::setInitialProteinMat()
{
    if(clustalw::subMatrix->getQTAAHistMatNum() == clustalw::AAHISTIDENTITY)
    {
        identity->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTAAHistMatNum() == clustalw::AAHISTGONNETPAM80)
    {
        gonnetPAM80->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTAAHistMatNum() == clustalw::AAHISTGONNETPAM120)
    {
        gonnetPAM120->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTAAHistMatNum() == clustalw::AAHISTGONNETPAM250)
    {
        gonnetPAM250->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTAAHistMatNum() == clustalw::AAHISTGONNETPAM350)
    {
        gonnetPAM350->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTAAHistMatNum() == clustalw::AAHISTUSER)
    {
        aaUserMat->setChecked(true);
    }                
}

void ColumnScoreParams::setUpDNAMatRadioButtons()
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

void ColumnScoreParams::userDNAClicked()
{
    if(!dnaMatLoaded)
    {
        browseDNA();
    }
}

void ColumnScoreParams::setInitialDNAMat()
{
    if(clustalw::subMatrix->getQTDNAHistMatNum() == clustalw::DNAIUB)
    {
        IUB->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTDNAHistMatNum() == clustalw::DNACLUSTALW)
    {
        clustal->setChecked(true);
    }
    else if(clustalw::subMatrix->getQTDNAHistMatNum() == clustalw::DNAUSERDEFINED)
    {
        dnaUserMat->setChecked(true);
    }    
}

void ColumnScoreParams::setUpOtherParams()
{
    plotScale = new QSlider(Qt::Horizontal);
    plotScale->setRange(1, 10);  
    scorePlotScale = new QLabel(tr("Score Plot Scale ")); 
    plotValue = new QLCDNumber;
    connect(plotScale, SIGNAL(valueChanged(int)), plotValue, SLOT(display(int)));
                
    cutOff = new QSlider(Qt::Horizontal);
    cutOff->setRange(1, 10);   
    residueExceptionCutoff = new QLabel(tr("Residue Exception Cutoff "));                     
    cutOffValue = new QLCDNumber;
    connect(cutOff, SIGNAL(valueChanged(int)), cutOffValue, SLOT(display(int)));
}

void ColumnScoreParams::setUpLoadButtons()
{
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

void ColumnScoreParams::setInitialParamValues()
{
    plotScale->setValue(clustalw::userParameters->getQTScorePlotScale());
    plotValue->display(plotScale->value());
    
    cutOff->setValue(clustalw::userParameters->getQTResExceptionCutOff());
    cutOffValue->display(cutOff->value());
}

void ColumnScoreParams::browseProtein()
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
        
        if (clustalw::subMatrix->getAAScoreMatFromFile(userFile))
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

void ColumnScoreParams::browseDNA()
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
        
        if (clustalw::subMatrix->getDNAScoreMatFromFile(userFile))
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

void ColumnScoreParams::radioSetUsingUserProtein()
{
    aaUserMat->setChecked(true);
}

void ColumnScoreParams::accept()
{
    clustalw::userParameters->setQTScorePlotScale(plotScale->value());
    
    clustalw::userParameters->setQTResExceptionCutOff(cutOff->value());
    
    //set amino acid histogram matrix
    if(identity->isChecked())
    {
        clustalw::subMatrix->setQTAAHistMatNum(clustalw::AAHISTIDENTITY);
    }
    else if(gonnetPAM80->isChecked())
    {
        clustalw::subMatrix->setQTAAHistMatNum(clustalw::AAHISTGONNETPAM80);
    }
    else if(gonnetPAM120->isChecked())
    {
        clustalw::subMatrix->setQTAAHistMatNum(clustalw::AAHISTGONNETPAM120);
    }
    else if(gonnetPAM250->isChecked())
    {
        clustalw::subMatrix->setQTAAHistMatNum(clustalw::AAHISTGONNETPAM250);
    }
    else if(gonnetPAM350->isChecked())
    {
        clustalw::subMatrix->setQTAAHistMatNum(clustalw::AAHISTGONNETPAM350);
    }
    else if(aaUserMat->isChecked())
    {
        clustalw::subMatrix->setQTAAHistMatNum(clustalw::AAHISTUSER);
    }     
    
    // set DNA histogram matrix
    if(IUB->isChecked())
    {
        clustalw::subMatrix->setQTDNAHistMatNum(clustalw::DNAIUB);
    }
    else if(clustal->isChecked())
    {
        clustalw::subMatrix->setQTDNAHistMatNum(clustalw::DNACLUSTALW);
    }
    else if(dnaUserMat->isChecked())
    {
        clustalw::subMatrix->setQTDNAHistMatNum(clustalw::DNAUSERDEFINED);
    }
        
    setResult(QDialog::Accepted);
    hide();
}
