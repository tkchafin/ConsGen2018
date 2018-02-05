/**
 * Changes:
 * Mark: 18-01-2007. Changed from using QFileDialog to FileDialog.
 *
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects.
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
#include "TreeOutputFileNames.h"
#include "clustalW/general/userparams.h"
#include "clustalW/general/utils.h"
#include "ClustalQtParams.h"
#include "FileDialog.h"

TreeOutputFileNames::TreeOutputFileNames(QString seqFileName)
{    
    setAllPtrsToNull();
    
    title = "Draw Tree";
        
    setWindowTitle(title);
    
    filePath = getPathFromFileName(seqFileName);
    
    mainLayout = new QVBoxLayout;
     
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    treeOutCancelButton = new QPushButton(tr("Cancel"));
    treeOutCancelButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(treeOutCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));        
    
    fileBox = new QGroupBox();
    
    setUpLayout();
    mainLayout->addWidget(fileBox);
    setLayout(mainLayout);
        
}

QString TreeOutputFileNames::getPathFromFileName(QString fileName)
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

void TreeOutputFileNames::setUpLayout()
{
    int row = 0;
    fileLayout = new QGridLayout;
    if(clustalw::userParameters->getOutputTreeClustal())
    {
        clustalTreeNameLabel = new QLabel(tr("Save Clustal Tree As: "));
        clustalTreeName = new QLineEdit();
        clustalTreeName->setText(filePath + "nj");
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
        phylipTreeNameLabel = new QLabel(tr("Save Phylip Tree As: "));
        phylipTreeName = new QLineEdit();
        phylipTreeName->setText(filePath + "ph");
        phylipTreeName->setMinimumWidth(MinLineEditWidth);
        phylipTreeBrowseButton = new QPushButton("Browse");
        connect(phylipTreeBrowseButton, SIGNAL(clicked()), this, SLOT(browsePhylip()));
        fileLayout->addWidget(phylipTreeNameLabel, row, 0);
        fileLayout->addWidget(phylipTreeName, row, 1);
        fileLayout->addWidget(phylipTreeBrowseButton, row, 2);
        row++;    
    }
    if(clustalw::userParameters->getOutputTreeDistances())
    {
        distancesNameLabel = new QLabel(tr("Save Distance Matrix As: "));
        distancesName = new QLineEdit();
        distancesName->setText(filePath + "dst");
        distancesName->setMinimumWidth(MinLineEditWidth);
        distancesBrowseButton = new QPushButton("Browse");
        connect(distancesBrowseButton, SIGNAL(clicked()), this, SLOT(browseDistances()));
        fileLayout->addWidget(distancesNameLabel, row, 0);
        fileLayout->addWidget(distancesName, row, 1);
        fileLayout->addWidget(distancesBrowseButton, row, 2);
        row++;     
    }
    if(clustalw::userParameters->getOutputPim())
    {
        pimNameLabel = new QLabel(tr("Save % Identity Matrix As: "));
        pimName = new QLineEdit();
        pimName->setText(filePath + "pim");
        pimName->setMinimumWidth(MinLineEditWidth);
        pimNameBrowseButton = new QPushButton("Browse");
        connect(pimNameBrowseButton, SIGNAL(clicked()), this, SLOT(browsePim()));
        fileLayout->addWidget(pimNameLabel, row, 0);
        fileLayout->addWidget(pimName, row, 1);
        fileLayout->addWidget(pimNameBrowseButton, row, 2);
        row++;     
    }
    if(clustalw::userParameters->getOutputTreeNexus())
    {
        nexusTreeNameLabel = new QLabel(tr("Save Nexus Tree As: "));
        nexusTreeName = new QLineEdit();
        nexusTreeName->setText(filePath + "nxs");
        nexusTreeName->setMinimumWidth(MinLineEditWidth);
        nexusTreeBrowseButton = new QPushButton("Browse");
        connect(nexusTreeBrowseButton, SIGNAL(clicked()), this, SLOT(browseNexus()));
        fileLayout->addWidget(nexusTreeNameLabel, row, 0);
        fileLayout->addWidget(nexusTreeName, row, 1);
        fileLayout->addWidget(nexusTreeBrowseButton, row, 2);
        row++;     
    }
    fileLayout->addWidget(okButton, row, 0);
    fileLayout->addWidget(treeOutCancelButton, row, 1);
    fileBox->setLayout(fileLayout);
}

void TreeOutputFileNames::browseClustal()
{
    browse(clustalTreeName);
}

void TreeOutputFileNames::browseNexus()
{
    browse(nexusTreeName);
}

void TreeOutputFileNames::browsePhylip()
{
    browse(phylipTreeName);
}

void TreeOutputFileNames::browseDistances()
{
    browse(distancesName);
}

void TreeOutputFileNames::browsePim()
{
    browse(pimName);
}

void TreeOutputFileNames::accept()
{
    treeFileNames.reset(new clustalw::TreeNames);
    if(clustalTreeName != 0)
    {
        if(clustalTreeName->text() == "")
        {
            treeFileNames->clustalName = filePath.toStdString() + "nj";
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
            treeFileNames->phylipName = filePath.toStdString() + "ph";
        }
        else
        {
            QString file = phylipTreeName->text(); 
            treeFileNames->phylipName = file.toStdString();
        }    
    }    
    if(distancesName != 0)
    {
        if(distancesName->text() == "")
        {
            treeFileNames->distName = filePath.toStdString() + "dst";
        }
        else
        {
            QString file = distancesName->text(); 
            treeFileNames->distName = file.toStdString();
        }    
    } 
    if(nexusTreeName != 0)
    {
        if(nexusTreeName->text() == "")
        {
            treeFileNames->nexusName = filePath.toStdString() + "nxs";
        }
        else
        {
            QString file = nexusTreeName->text(); 
            treeFileNames->nexusName = file.toStdString();
        }    
    }  
    if(pimName != 0)
    {
        if(pimName->text() == "")
        {
            treeFileNames->pimName = filePath.toStdString() + "pim";
        }
        else
        {
            QString file = pimName->text(); 
            treeFileNames->pimName = file.toStdString();
        }    
    }                        
    setResult(QDialog::Accepted);
    hide();
}

void TreeOutputFileNames::cancel()
{
    setResult(QDialog::Rejected);
    hide();
}

void TreeOutputFileNames::browse(QLineEdit* lineEditName)
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

clustalw::TreeNames TreeOutputFileNames::getNames()
{
    return *(treeFileNames.get());
}

void TreeOutputFileNames::setAllPtrsToNull()
{
    nexusTreeBrowseButton = 0;
    nexusTreeName = 0;
    nexusTreeNameLabel = 0;
    pimNameBrowseButton = 0;
    pimName = 0;
    pimNameLabel = 0;
    distancesBrowseButton = 0;
    distancesName = 0;
    distancesNameLabel = 0;
    phylipTreeBrowseButton = 0;
    phylipTreeName = 0;
    phylipTreeNameLabel = 0;
    clustalTreeBrowseButton = 0;
    clustalTreeName = 0;
    clustalTreeNameLabel = 0;
    treeOutCancelButton = 0;
    okButton = 0;
    fileLayout = 0;
    fileBox = 0;
    mainLayout = 0;
}
