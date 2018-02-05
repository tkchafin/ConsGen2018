#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QComboBox>
#include <QtGui>
#include <QLabel>
#include <QLineEdit>
#include "SecStructOptions.h"
#include "clustalW/general/userparams.h"

SecStructOptions::SecStructOptions()
{
    setWindowTitle("Secondary Structure Options");
    QVBoxLayout* mainLayout = new QVBoxLayout;
    
    outputGridBox = new QGroupBox("Output");
    ssGridBox = new QGroupBox;
    ssParamsGridBox = new QGroupBox;
    verticalBox = new QGroupBox;

    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));   
    
    setUpCombo();
    setUpOutput();
    setUpOtherParams();
    
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(ssGridBox);
    mainLayout->addWidget(outputGridBox);
    mainLayout->addWidget(ssParamsGridBox);
    setLayout(mainLayout);     
}

void SecStructOptions::setUpCombo()
{
    QGridLayout *grid = new QGridLayout;
    useProfile1SS = new QComboBox;
    useProfile1SS->addItem("Yes");
    useProfile1SS->addItem("No");
    if(clustalw::userParameters->getUseSS1())
    {
        useProfile1SS->setCurrentIndex(0);
    }
    else
    {
        useProfile1SS->setCurrentIndex(1);
    }
    
    useProfile2SS = new QComboBox;
    useProfile2SS->addItem("Yes");
    useProfile2SS->addItem("No");  
    if(clustalw::userParameters->getUseSS2())
    {
        useProfile2SS->setCurrentIndex(0);
    }
    else
    {
        useProfile2SS->setCurrentIndex(1);
    }
        
    useProfile1SSLabel = new QLabel("Use profile 1 secondary structure / penalty mask");
    useProfile2SSLabel = new QLabel("Use profile 2 secondary structure / penalty mask");
    
    grid->addWidget(useProfile1SSLabel, 0, 0);
    grid->addWidget(useProfile1SS, 0, 1);
    grid->addWidget(useProfile2SSLabel, 1, 0);
    grid->addWidget(useProfile2SS, 1, 1);    
    
    ssGridBox->setLayout(grid);
      
}

void SecStructOptions::setUpOutput()
{
    QGridLayout *grid = new QGridLayout;
    secStruct = new QCheckBox("Secondary Structure");
    gapMask = new QCheckBox("Gap Penalty Mask"); 
    if(clustalw::userParameters->getOutputStructPenalties() == clustalw::OUTSECST)
    {
        secStruct->setChecked(true);
        gapMask->setChecked(false);        
    }
    else if(clustalw::userParameters->getOutputStructPenalties() == clustalw::OUTGAP)
    {
        gapMask->setChecked(true);
        secStruct->setChecked(false);        
    }
    else if(clustalw::userParameters->getOutputStructPenalties() == clustalw::OUTBOTH)
    {
        secStruct->setChecked(true);
        gapMask->setChecked(true);            
    }
    else
    {
        secStruct->setChecked(false);
        gapMask->setChecked(false);    
    }
    grid->addWidget(secStruct, 0, 0);
    grid->addWidget(gapMask, 0, 1);
    outputGridBox->setLayout(grid);
}

void SecStructOptions::setUpOtherParams()
{
    QString myNum;
    
    helixGapPen = new QLineEdit;
    myNum.setNum(clustalw::userParameters->getHelixPenalty());
    helixGapPen->setText(myNum);
    
    strandGapPen = new QLineEdit;
    myNum.setNum(clustalw::userParameters->getStrandPenalty());
    strandGapPen->setText(myNum);
    
    loopGapPen = new QLineEdit;
    myNum.setNum(clustalw::userParameters->getLoopPenalty());
    loopGapPen->setText(myNum);
        
    ssTerminalPen = new QLineEdit;
    myNum.setNum(clustalw::userParameters->getHelixEndPenalty());
    ssTerminalPen->setText(myNum);
        
    helixTerminalPosWithin = new QLineEdit;
    myNum.setNum(clustalw::userParameters->getHelixEndMinus());
    helixTerminalPosWithin->setText(myNum);
        
    helixTerminalPosOutside = new QLineEdit;
    myNum.setNum(clustalw::userParameters->getHelixEndPlus());
    helixTerminalPosOutside->setText(myNum);
        
    strandTerminalPosWithin = new QLineEdit;
    myNum.setNum(clustalw::userParameters->getStrandEndMinus());
    strandTerminalPosWithin->setText(myNum);
    
    strandTerminalPosOutside = new QLineEdit;
    myNum.setNum(clustalw::userParameters->getStrandEndPlus());
    strandTerminalPosOutside->setText(myNum);
    
    helixGapPenLabel = new QLabel("Helix Gap Penalty [1-9]:");
    strandGapPenLabel = new QLabel("Strand Gap Penalty [1-9]:");
    loopGapPenLabel = new QLabel("Loop Gap Penalty [1-9]:");
    ssTerminalPenLabel = new QLabel("Secondary Structure Terminal Penalty [1-9]:");
    helixTerminalPosWithinLabel = new QLabel("Helix Terminal Positions [1-9]    within:");
    helixTerminalPosOutsideLabel = new QLabel("outside:");
    strandTerminalPosWithinLabel = new QLabel("Strand Terminal Positions [1-9]    within:");
    strandTerminalPosOutsideLabel = new QLabel("outside:");
    
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(helixGapPenLabel, 0, 0);
    grid->addWidget(helixGapPen, 0, 1);
    grid->addWidget(strandGapPenLabel, 1, 0);
    grid->addWidget(strandGapPen, 1, 1);
    grid->addWidget(loopGapPenLabel, 2, 0);
    grid->addWidget(loopGapPen, 2, 1);
    grid->addWidget(ssTerminalPenLabel, 3, 0);
    grid->addWidget(ssTerminalPen, 3, 1);
    grid->addWidget(helixTerminalPosWithinLabel, 4, 0);
    grid->addWidget(helixTerminalPosWithin, 4, 1);
    grid->addWidget(helixTerminalPosOutsideLabel, 5, 0);
    grid->addWidget(helixTerminalPosOutside, 5, 1);
    grid->addWidget(strandTerminalPosWithinLabel, 6, 0);
    grid->addWidget(strandTerminalPosWithin, 6, 1);
    grid->addWidget(strandTerminalPosOutsideLabel, 7, 0);
    grid->addWidget(strandTerminalPosOutside, 7, 1);    
    
    ssParamsGridBox->setLayout(grid);                        
}

void SecStructOptions::accept()
{
    bool ok;
    // Using secondary structure information??
    if(useProfile1SS->currentIndex() == 0)
    {
        clustalw::userParameters->setUseSS1(true);
    }
    else
    {
        clustalw::userParameters->setUseSS1(false);
    }    

    if(useProfile2SS->currentIndex() == 0)
    {
        clustalw::userParameters->setUseSS2(true);
    }
    else
    {
        clustalw::userParameters->setUseSS2(false);
    }

    // Output secondary structure information???
    if(secStruct->isChecked() && !gapMask->isChecked())
    {
        clustalw::userParameters->setOutputStructPenalties(clustalw::OUTSECST);
    }
    else if(gapMask->isChecked() && !secStruct->isChecked())
    {
        clustalw::userParameters->setOutputStructPenalties(clustalw::OUTGAP);     
    }
    else if(secStruct->isChecked() && gapMask->isChecked())
    {
        clustalw::userParameters->setOutputStructPenalties(clustalw::OUTBOTH);           
    }
    else
    {
        clustalw::userParameters->setOutputStructPenalties(clustalw::OUTNONE);    
    }
    // Now check all the numbers in the boxes are 1-9
    QString helixGapPenString = helixGapPen->text();
    int pen = helixGapPenString.toInt(&ok);
    if(ok && pen >= 1 && pen <= 9)
    {
        clustalw::userParameters->setHelixPenalty(pen);
    }
    
    QString strandGapPenString = strandGapPen->text();
    pen = strandGapPenString.toInt(&ok);
    if(ok && pen >= 1 && pen <= 9)
    {
        clustalw::userParameters->setStrandPenalty(pen);
    }
        
    QString loopGapPenString = loopGapPen->text();
    pen = loopGapPenString.toInt(&ok);
    if(ok && pen >= 1 && pen <= 9)
    {
        clustalw::userParameters->setLoopPenalty(pen);
    }    
    
    QString ssTerminalPenString = ssTerminalPen->text();
    pen = ssTerminalPenString.toInt(&ok);
    if(ok && pen >= 1 && pen <= 9)
    {
        clustalw::userParameters->setHelixEndPenalty(pen);
    }    
    
    QString helixTerminalPosWithinString = helixTerminalPosWithin->text();
    pen = helixTerminalPosWithinString.toInt(&ok);
    if(ok && pen >= 1 && pen <= 9)
    {
        clustalw::userParameters->setHelixEndMinus(pen);
    }
        
    QString helixTerminalPosOutsideString = helixTerminalPosOutside->text();
    pen = helixTerminalPosOutsideString.toInt(&ok);
    if(ok && pen >= 1 && pen <= 9)
    {
        clustalw::userParameters->setHelixEndPlus(pen);
    }
        
    QString strandTerminalPosWithinString = strandTerminalPosWithin->text();
    pen = strandTerminalPosWithinString.toInt(&ok);
    if(ok && pen >= 1 && pen <= 9)
    {
        clustalw::userParameters->setStrandEndMinus(pen);
    }
           
    QString strandTerminalPosOutsideString = strandTerminalPosOutside->text();
    pen = strandTerminalPosOutsideString.toInt(&ok);
    if(ok && pen >= 1 && pen <= 9)
    {
        clustalw::userParameters->setStrandEndPlus(pen);
    }
                    
    setResult(QDialog::Accepted);
    hide();
}
