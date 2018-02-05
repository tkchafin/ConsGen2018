#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QComboBox>
#include <QtGui>
#include <QLabel>
#include "AlignmentFormatOptions.h"
#include "clustalW/general/userparams.h"

AlignmentFormatOptions::AlignmentFormatOptions()
{
    setWindowTitle("Output Format Options");
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

void AlignmentFormatOptions::setUpLayout()
{
   setUpRadioButtons(); 
   QGridLayout *grid = new QGridLayout; 
   grid->addWidget(clustal, 0, 0);
   grid->addWidget(nbrf, 0, 1);
   grid->addWidget(gcg, 1, 0);
   grid->addWidget(phylip, 1, 1);
   grid->addWidget(gde, 2, 0);
   grid->addWidget(nexus, 2, 1);
   grid->addWidget(fasta, 3, 0);
   fileGridBox->setLayout(grid);
   
   setUpOtherParams();
   QGridLayout *otherParamLayout = new QGridLayout; 
   otherParamLayout->addWidget(gdeOutputCaseLabel, 0, 0);
   otherParamLayout->addWidget(gdeOutputCase, 0, 1);
   otherParamLayout->addWidget(clustalSequenceNumbersLabel, 1, 0);
   otherParamLayout->addWidget(clustalSequenceNumbers, 1, 1);
   otherParamLayout->addWidget(outputOrderLabel, 2, 0);
   otherParamLayout->addWidget(outputOrder, 2, 1);
   otherParamLayout->addWidget(parameterOutputLabel, 3, 0);
   otherParamLayout->addWidget(parameterOutput, 3, 1);
   otherParams->setLayout(otherParamLayout);   
}

void AlignmentFormatOptions::setUpRadioButtons()
{
    clustal = new QCheckBox("CLUSTAL format"); 
    gcg = new QCheckBox("GCG/MSF format");
    gde = new QCheckBox("GDE format");
    fasta = new QCheckBox("FASTA format");
    nbrf = new QCheckBox("NBRF/PIR format");
    phylip = new QCheckBox("PHYLIP format");
    nexus = new QCheckBox("NEXUS format");
        
    if(clustalw::userParameters->getOutputClustal())
    {
        clustal->setChecked(true);
    }
    if(clustalw::userParameters->getOutputGCG())
    {
        gcg->setChecked(true);
    }
    if(clustalw::userParameters->getOutputGde())
    {
        gde->setChecked(true);
    }
    if(clustalw::userParameters->getOutputFasta())
    {
        fasta->setChecked(true);
    }
    if(clustalw::userParameters->getOutputNbrf())
    {
        nbrf->setChecked(true);
    }
    if(clustalw::userParameters->getOutputPhylip())
    {
        phylip->setChecked(true);
    }  
    if(clustalw::userParameters->getOutputNexus())
    {
        nexus->setChecked(true);
    }             
}

void AlignmentFormatOptions::setUpOtherParams()
{
    gdeOutputCase = new QComboBox();
    gdeOutputCase->addItem("Lower");    
    gdeOutputCase->addItem("Upper");
    if(clustalw::userParameters->getLowercase())
    {
        gdeOutputCase->setCurrentIndex(0);
    }
    else
    {
        gdeOutputCase->setCurrentIndex(1);
    }
    gdeOutputCaseLabel = new QLabel(tr("GDE output case :"));
    
    clustalSequenceNumbers = new QComboBox();
    clustalSequenceNumbers->addItem("Off");
    clustalSequenceNumbers->addItem("On");
    if(clustalw::userParameters->getClSeqNumbers())
    {
        clustalSequenceNumbers->setCurrentIndex(1);
    }
    else
    {
        clustalSequenceNumbers->setCurrentIndex(0);
    }
    clustalSequenceNumbersLabel = new QLabel(tr("CLUSTALW sequence numbers :"));
    
    outputOrder = new QComboBox();
    outputOrder->addItem("Aligned");
    outputOrder->addItem("Input");
    if(clustalw::userParameters->getOutputOrder() == clustalw::ALIGNED)
    {
        outputOrder->setCurrentIndex(0);
    }
    else
    {
        outputOrder->setCurrentIndex(1);
    }
    outputOrderLabel = new QLabel(tr("Output order :"));  
    
    parameterOutput = new QComboBox();     
    parameterOutput->addItem("Off");
    parameterOutput->addItem("On");
    if(clustalw::userParameters->getSaveParameters())
    {
        parameterOutput->setCurrentIndex(1);
    }
    else
    {
        parameterOutput->setCurrentIndex(0);
    }
    parameterOutputLabel = new QLabel(tr("Parameters output :"));    
}

void AlignmentFormatOptions::accept()
{
    // Output file options
    if(clustal->isChecked())
    {
        clustalw::userParameters->setOutputClustal(true);
    }
    else
    {
        clustalw::userParameters->setOutputClustal(false);
    }
    if(gcg->isChecked())
    {
        clustalw::userParameters->setOutputGCG(true);
    }
    else
    {
        clustalw::userParameters->setOutputGCG(false);
    }
    if(gde->isChecked())
    {
        clustalw::userParameters->setOutputGde(true);
    }
    else
    {
        clustalw::userParameters->setOutputGde(false);
    }
    if(fasta->isChecked())
    {
        clustalw::userParameters->setOutputFasta(true);
    }
    else
    {
        clustalw::userParameters->setOutputFasta(false);
    }
    if(nbrf->isChecked())
    {
        clustalw::userParameters->setOutputNbrf(true);
    }
    else
    {
        clustalw::userParameters->setOutputNbrf(false);
    }
    if(phylip->isChecked())
    {
        clustalw::userParameters->setOutputPhylip(true);
    }
    else
    {
        clustalw::userParameters->setOutputPhylip(false);
    }  
    if(nexus->isChecked())
    {
        clustalw::userParameters->setOutputNexus(true);
    }
    else
    {
        clustalw::userParameters->setOutputNexus(false);
    }

    // GDE output file case 
    if(gdeOutputCase->currentIndex() == 0)
    {
        clustalw::userParameters->setLowercase(true);
    }
    else
    {
        clustalw::userParameters->setLowercase(false);
    }
    
    if(clustalSequenceNumbers->currentIndex() == 1)
    {
        clustalw::userParameters->setClSeqNumbers(true);
    }
    else
    {
        clustalw::userParameters->setClSeqNumbers(false);
    }
    
    if(outputOrder->currentIndex() == 0)
    {
        clustalw::userParameters->setOutputOrder(clustalw::ALIGNED);
    }
    else
    {
        clustalw::userParameters->setOutputOrder(clustalw::INPUT);
    }
    
    if(parameterOutput->currentIndex() == 1)
    {
        clustalw::userParameters->setSaveParameters(true);
    }
    else
    {
        clustalw::userParameters->setSaveParameters(false);
    }
    
    setResult(QDialog::Accepted);
    hide();            
}
