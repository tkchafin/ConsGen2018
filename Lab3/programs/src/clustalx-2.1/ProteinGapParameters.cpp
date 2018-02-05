#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QComboBox>
#include <QtGui>
#include <QLabel>
#include <QString>
#include <QMessageBox>
#include "ProteinGapParameters.h"
#include "clustalW/general/userparams.h"

ProteinGapParameters::ProteinGapParameters()
 : gapDist(4)
{
    setWindowTitle("Protein Gap Parameters");
    otherParams = new QGroupBox;
    QVBoxLayout* mainLayout = new QVBoxLayout;
     
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    
    setUpLayout();
    
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(otherParams);
    setLayout(mainLayout);
}

void ProteinGapParameters::setUpLayout()
{  
   setUpOtherParams();
   QGridLayout *otherParamLayout = new QGridLayout; 
   otherParamLayout->addWidget(resSpecPenLabel, 0, 0);
   otherParamLayout->addWidget(resSpecPen, 0, 1);
   otherParamLayout->addWidget(hydSpecPenLabel, 1, 0);
   otherParamLayout->addWidget(hydSpecPen, 1, 1);
   otherParamLayout->addWidget(hydroPhilicResLabel, 2, 0);
   otherParamLayout->addWidget(hydroPhilicRes, 2, 1);
   otherParamLayout->addWidget(gapSeparationDistLabel, 3, 0);
   otherParamLayout->addWidget(gapSeparationDist, 3, 1);
   otherParamLayout->addWidget(endGapSeparationLabel, 4, 0);
   otherParamLayout->addWidget(endGapSeparation, 4, 1);   
            
   otherParams->setLayout(otherParamLayout);   
}

void ProteinGapParameters::setUpOtherParams()
{
    resSpecPen = new QComboBox();
    resSpecPen->addItem("On");    
    resSpecPen->addItem("Off");
    resSpecPenLabel = new QLabel(tr("Residue-specific Penalties"));
    
    hydSpecPen = new QComboBox();
    hydSpecPen->addItem("On");    
    hydSpecPen->addItem("Off");
    hydSpecPenLabel = new QLabel(tr("Hydrophilic Penalties"));
    
    endGapSeparation = new QComboBox();
    endGapSeparation->addItem("Off");    
    endGapSeparation->addItem("On");
    endGapSeparationLabel = new QLabel(tr("End Gap Separation")); 
    
    hydroPhilicRes = new QLineEdit;
    hydroPhilicResLabel = new QLabel(tr("Hydrophilic Residues: ")); 
    
    gapSeparationDist = new QLineEdit;
    gapSeparationDistLabel = new QLabel(tr("Gap Separation Distance [0-100]: "));  
    
    setInitialParameterValues();   
}

void ProteinGapParameters::setInitialParameterValues()
{
    if(clustalw::userParameters->getNoPrefPenalties())
    {
        resSpecPen->setCurrentIndex(1);
    }
    else
    {
        resSpecPen->setCurrentIndex(0);
    }
    
    if(clustalw::userParameters->getNoHydPenalties())
    {
        hydSpecPen->setCurrentIndex(1);
    }
    else
    {
        hydSpecPen->setCurrentIndex(0);
    }
    
    if(clustalw::userParameters->getUseEndGaps())
    {
        endGapSeparation->setCurrentIndex(1);
    }
    else
    {
        endGapSeparation->setCurrentIndex(0);
    } 
    
    QString hydRes(clustalw::userParameters->getHydResidues().c_str());
    hydroPhilicRes->setText(hydRes); 

    QString num;
    num = num.setNum(clustalw::userParameters->getGapDist());    
    gapSeparationDist->setText(num);       
}

void ProteinGapParameters::accept()
{
    // We have 2 line edit fields. These must be validated.    
    // The first one must contain only letters.
    QString hydRes = hydroPhilicRes->text();
    QString hydResEdited;
    QChar res;
    // The second one must contain a number between 0 and 100.
    for(int i = 0; i < hydRes.size(); i++)
    {
        res = hydRes[i];
        if(res.isLetter())
        {
            hydResEdited += res;
        }
    }
    hydRes = hydResEdited;
    // Set the hydrophilic residues
    clustalw::userParameters->setHydResidues(hydRes.toStdString());
    
    bool ok;
    
    QString gapDist = gapSeparationDist->text();
    int dist;
    dist = gapDist.toInt(&ok);
    
    if(ok && dist >= 0 && dist <= 100)
    {
        clustalw::userParameters->setGapDist(dist);
    }

    // Now the combo boxes
    if(resSpecPen->currentIndex() == 1)
    {
        clustalw::userParameters->setNoPrefPenalties(true);
    }
    else
    {
        clustalw::userParameters->setNoPrefPenalties(false);
    }
    
    if(hydSpecPen->currentIndex() == 1)
    {
        clustalw::userParameters->setNoHydPenalties(true);
    }
    else
    {
        clustalw::userParameters->setNoHydPenalties(false);
    }
    
    if(endGapSeparation->currentIndex() == 1)
    {
        clustalw::userParameters->setUseEndGaps(true);
    }
    else
    {
        clustalw::userParameters->setUseEndGaps(false);
    }
        
    setResult(QDialog::Accepted);
    hide();
}
