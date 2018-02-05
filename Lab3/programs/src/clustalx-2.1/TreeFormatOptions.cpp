#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QComboBox>
#include <QtGui>
#include <QLabel>
#include <QCheckBox>
#include "TreeFormatOptions.h"
#include "clustalW/general/userparams.h"

TreeFormatOptions::TreeFormatOptions()
{
    fileGridBox = new QGroupBox("Output Files");
    otherParams = new QGroupBox;
    QVBoxLayout* mainLayout = new QVBoxLayout;
     
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    
    setUpLayout();
    
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(fileGridBox);
    mainLayout->addWidget(otherParams);
    setLayout(mainLayout);
}

void TreeFormatOptions::setUpLayout()
{
   setUpRadioButtons(); 
   QGridLayout *grid = new QGridLayout; 
   grid->addWidget(clustal, 0, 0);
   grid->addWidget(phylip, 0, 1);
   grid->addWidget(phylipDistMat, 1, 0);
   grid->addWidget(nexus, 1, 1);
   grid->addWidget(percIdentMat, 2, 0);
   fileGridBox->setLayout(grid);
   
   setUpOtherParams();
   QGridLayout *otherParamLayout = new QGridLayout; 
   otherParamLayout->addWidget(bootStrapLabel, 0, 0);
   otherParamLayout->addWidget(bootStrap, 0, 1);
   otherParams->setLayout(otherParamLayout);   
}

void TreeFormatOptions::setUpRadioButtons()
{
    clustal = new QCheckBox("CLUSTAL format tree");
    percIdentMat = new QCheckBox("% identity matrix");
    phylip = new QCheckBox("Phylip format tree");
    phylipDistMat = new QCheckBox("Phylip distance matrix");
    nexus = new QCheckBox("Nexus format tree");
    
    if(clustalw::userParameters->getOutputTreeClustal())
    {
        clustal->setChecked(true);
    }
    if(clustalw::userParameters->getOutputPim())
    {
        percIdentMat->setChecked(true);
    } 
    if(clustalw::userParameters->getOutputTreePhylip())
    {
        phylip->setChecked(true);
    }
    if(clustalw::userParameters->getOutputTreeDistances())
    {
        phylipDistMat->setChecked(true);
    }
    if(clustalw::userParameters->getOutputTreeNexus())
    {
        nexus->setChecked(true);
    }
}

void TreeFormatOptions::setUpOtherParams()
{
    bootStrap = new QComboBox();
    bootStrap->addItem("Branch");    
    bootStrap->addItem("Node");
    if(clustalw::userParameters->getBootstrapFormat() == clustalw::BS_NODE_LABELS)
    {
        bootStrap->setCurrentIndex(1);
    }
    else
    {
        bootStrap->setCurrentIndex(0);
    }    
    bootStrapLabel = new QLabel(tr("Bootstrap labels on :"));    
}

void TreeFormatOptions::accept()
{
    if(clustal->isChecked())
    {
        clustalw::userParameters->setOutputTreeClustal(true);
    }
    else
    {
        clustalw::userParameters->setOutputTreeClustal(false);
    }
    if(percIdentMat->isChecked())
    {
        clustalw::userParameters->setOutputPim(true);
    }
    else
    {
        clustalw::userParameters->setOutputPim(false);
    } 
    if(phylip->isChecked())
    {
        clustalw::userParameters->setOutputTreePhylip(true);
    }
    else
    {
        clustalw::userParameters->setOutputTreePhylip(false);
    }
    if(phylipDistMat->isChecked())
    {
        clustalw::userParameters->setOutputTreeDistances(true);
    }
    else
    {
        clustalw::userParameters->setOutputTreeDistances(false);
    }
    if(nexus->isChecked())
    {
        clustalw::userParameters->setOutputTreeNexus(true);
    }
    else
    {
        clustalw::userParameters->setOutputTreeNexus(false);
    }
    
    if(bootStrap->currentIndex() == 1)
    {
        clustalw::userParameters->setBootstrapFormat(clustalw::BS_NODE_LABELS);
    }
    else
    {
        clustalw::userParameters->setBootstrapFormat(clustalw::BS_BRANCH_LABELS);
    }
    setResult(QDialog::Accepted);
    hide();    
}
