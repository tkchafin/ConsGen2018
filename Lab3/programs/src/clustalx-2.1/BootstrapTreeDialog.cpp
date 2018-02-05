/**
 * Changes:
 * Mark: 18-01-2007. Changed from using QFileDialog to FileDialog.
 *
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. Also removed
 * setAllPtrsToNull function. 
 */
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QComboBox>
#include <QtGui>
#include <QLabel>
#include <iostream>
#include "BootstrapTreeDialog.h"
#include "clustalW/general/userparams.h"
#include "clustalW/general/utils.h"
#include "ClustalQtParams.h"
#include "FileDialog.h"

BootstrapTreeDialog::BootstrapTreeDialog(QString seqFileName)
{    
    setAllPtrsToNull();
    
    title = "Draw Tree";
        
    setWindowTitle(title);
    
    filePath = getPathFromFileName(seqFileName);
    
    mainLayout = new QVBoxLayout;
     
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    bootstrapCancelButton = new QPushButton(tr("Cancel"));
    bootstrapCancelButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(bootstrapCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));        
    
    fileBox = new QGroupBox();
    
    setUpLayout();
    mainLayout->addWidget(fileBox);
    setLayout(mainLayout);
        
}

QString BootstrapTreeDialog::getPathFromFileName(QString fileName)
{
    QString filePath;
    if(fileName.size() > 0)
    {
        std::string path = "";
        clustalw::utilityObject->getPath(fileName.toStdString(), &path);
        
        if(path.size() > 0)
        {
            filePath = QString(path.c_str());
        }
        else
        {
           filePath = QDir::currentPath() + QString(QDir::separator()) + "temp";
        }
    }
    return filePath;
}

void BootstrapTreeDialog::setUpLayout()
{
    int row = 0;
    fileLayout = new QGridLayout;
    QString numStr;
    int num;
    
    randomNumberGenSeedLabel = new QLabel(tr("Random number generator seed[1-1000]: "));
    randomNumberGenSeed = new QLineEdit();
    num = clustalw::userParameters->getBootRanSeed();
    numStr.setNum(num);
    randomNumberGenSeed->setText(numStr);
    fileLayout->addWidget(randomNumberGenSeedLabel, row, 0);
    fileLayout->addWidget(randomNumberGenSeed, row, 1);    
    row++;
    
    numBootstrapTrialsLabel = new QLabel(tr("Number of bootstrap trials[1-10000]: "));
    numBootstrapTrials = new QLineEdit();
    num = clustalw::userParameters->getBootNumTrials();
    numStr.setNum(num);
    numBootstrapTrials->setText(numStr);
    fileLayout->addWidget(numBootstrapTrialsLabel, row, 0);
    fileLayout->addWidget(numBootstrapTrials, row, 1);    
    row++;
       
    if(clustalw::userParameters->getOutputTreeClustal())
    {
        clustalTreeNameLabel = new QLabel(tr("Save Clustal tree as: "));
        clustalTreeName = new QLineEdit();
        clustalTreeName->setText(filePath + "njb");
        clustalTreeName->setMinimumWidth(MinLineEditWidth);
        clustalTreeBrowseButton = new QPushButton("Browse");
        connect(clustalTreeBrowseButton, SIGNAL(clicked()), this, SLOT(browseClustal()));
        fileLayout->addWidget(clustalTreeNameLabel, row, 0);
        fileLayout->addWidget(clustalTreeName, row, 1);
        fileLayout->addWidget(clustalTreeBrowseButton, row, 2);
        row++;
    }
    if(clustalw::userParameters->getOutputTreePhylip())
    {
        phylipTreeNameLabel = new QLabel(tr("Save Phylip tree as: "));
        phylipTreeName = new QLineEdit();
        phylipTreeName->setText(filePath + "phb");
        phylipTreeName->setMinimumWidth(MinLineEditWidth);
        phylipTreeBrowseButton = new QPushButton("Browse");
        connect(phylipTreeBrowseButton, SIGNAL(clicked()), this, SLOT(browsePhylip()));
        fileLayout->addWidget(phylipTreeNameLabel, row, 0);
        fileLayout->addWidget(phylipTreeName, row, 1);
        fileLayout->addWidget(phylipTreeBrowseButton, row, 2);
        row++;    
    }
    if(clustalw::userParameters->getOutputTreeNexus())
    {
        nexusTreeNameLabel = new QLabel(tr("Save Nexus tree as: "));
        nexusTreeName = new QLineEdit();
        nexusTreeName->setText(filePath + "treb");
        nexusTreeName->setMinimumWidth(MinLineEditWidth);
        nexusTreeBrowseButton = new QPushButton("Browse");
        connect(nexusTreeBrowseButton, SIGNAL(clicked()), this, SLOT(browseNexus()));
        fileLayout->addWidget(nexusTreeNameLabel, row, 0);
        fileLayout->addWidget(nexusTreeName, row, 1);
        fileLayout->addWidget(nexusTreeBrowseButton, row, 2);
        row++;     
    }
    fileLayout->addWidget(okButton, row, 0);
    fileLayout->addWidget(bootstrapCancelButton, row, 1);
    fileBox->setLayout(fileLayout);
}

void BootstrapTreeDialog::browseClustal()
{
    browse(clustalTreeName);
}

void BootstrapTreeDialog::browseNexus()
{
    browse(nexusTreeName);
}

void BootstrapTreeDialog::browsePhylip()
{
    browse(phylipTreeName);
}

void BootstrapTreeDialog::accept()
{
    bool ok;
    int num;
    QString randomNumSeed = randomNumberGenSeed->text();
    num = randomNumSeed.toInt(&ok);
    if(ok)
    {
        if(num >= 1 && num <= 1000)
        {
            clustalw::userParameters->setBootRanSeed(num);
        }
    }
    
    QString numTrials = numBootstrapTrials->text();
    num = numTrials.toInt(&ok);
    if(ok)
    {
        if(num >= 1 && num <= 10000)
        {
            clustalw::userParameters->setBootNumTrials(num);
        }
    }
        
    treeFileNames.reset(new clustalw::TreeNames);
    if(clustalTreeName != 0)
    {
        if(clustalTreeName->text() == "")
        {
            treeFileNames->clustalName = filePath.toStdString() + "njb";
        }
        else
        {
            QString file = clustalTreeName->text(); 
            treeFileNames->clustalName = file.toStdString();
        }    
    }
    if(phylipTreeName != 0)
    {
        if(phylipTreeName->text() == "")
        {
            treeFileNames->phylipName = filePath.toStdString() + "phb";
        }
        else
        {
            QString file = phylipTreeName->text(); 
            treeFileNames->phylipName = file.toStdString();
        }    
    }    

    if(nexusTreeName != 0)
    {
        if(nexusTreeName->text() == "")
        {
            treeFileNames->nexusName = filePath.toStdString() + "treb";
        }
        else
        {
            QString file = nexusTreeName->text(); 
            treeFileNames->nexusName = file.toStdString();
        }    
    }  
                       
    setResult(QDialog::Accepted);
    hide();
}

void BootstrapTreeDialog::cancel()
{
    setResult(QDialog::Rejected);
    hide();
}

void BootstrapTreeDialog::browse(QLineEdit* lineEditName)
{
    QString myFile;
    // Mark Jan 18th 2007: changes to remember working Dir    
    FileDialog fd;
    myFile = fd.getSaveFileName(this, tr("Save as"));
    
    if(myFile != "")
    {
        lineEditName->setText(myFile);
    }
}

clustalw::TreeNames BootstrapTreeDialog::getNames()
{
    return *(treeFileNames.get());
}

void BootstrapTreeDialog::setAllPtrsToNull()
{
    nexusTreeBrowseButton = 0;
    nexusTreeName = 0;
    nexusTreeNameLabel = 0;
    phylipTreeBrowseButton = 0;
    phylipTreeName = 0;
    phylipTreeNameLabel = 0;
    clustalTreeBrowseButton = 0;
    clustalTreeName = 0;
    clustalTreeNameLabel = 0;
    numBootstrapTrials = 0;
    numBootstrapTrialsLabel = 0;
    randomNumberGenSeed = 0;
    randomNumberGenSeedLabel = 0;
    bootstrapCancelButton = 0;
    okButton = 0;
    fileLayout = 0;
    fileBox = 0;
    mainLayout = 0;
}
