/**
 * Changes:
 *
 * Mark: 18-01-2007. Changed from using QFileDialog to FileDialog.
 *
 * 30-03-07,Nigel Brown(EMBL): renamed variables 'AlignProf1ToProf2' to
 * 'AlignProf2ToProf1' and 'AlignProf1ToProf2WithTree' to
 * 'AlignProf2ToProf1WithTree', for consistency with clustalx and user
 * interface menu naming.
 *
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects.
 *
 * 18-06-07,Nigel Brown(EMBL): Made the 'ok' button in each dialogue be the
 * default instead of a Browse button.
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
#include "AlignOutputFileNames.h"
#include "clustalW/general/userparams.h"
#include "clustalW/general/utils.h"
#include "ClustalQtParams.h"
#include "FileDialog.h"
AlignOutputFileNames::AlignOutputFileNames(QString inputFileName, FileNameDialogType _type,
                                           AlignType _alignType, QString profileName)
 : type(_type),
   alignType(_alignType)
{    
    setAllPtrsToNull();
    
    if(type == CompleteAlign)
    {
        title = "Complete Alignment";    
    }
    else if(type == AlignFromTree)
    {
        title = "Alignment From Guide Tree";    
    }
    else if(type == RealignSelectedSeqs)
    {
        title = "Realign Sequences";    
    }
    else if(type == RealignSeqRange)
    {
        title = "Realign Residue Range";
    }
    else if(type == DoGuideTreeOnly)
    {
        title = "Create Tree";
    }
    else if(type == AlignProf2ToProf1)
    {
        title = "Profile To Profile Alignment";
    }
    else if(type == AlignSeqsToProf1)
    {
        title = "Sequences To Profile Alignment";
    }
    else if(type == AlignProf2ToProf1WithTree)
    {
        title = "Profile Alignment From Tree";
    }
    else if(type == AlignSeqsToProf1WithTree)
    {
        title = "Sequence To Profile Alignment From Tree";
    }
    else
    {
        title = "";
    }
    
    setWindowTitle(title);
    
    filePath = getPathFromFileName(inputFileName);
    
    if(profileName != "")
    {
        profile2Path = getPathFromFileName(profileName);
    }
    else
    {
        profile2Path = "";
    }
    mainLayout = new QVBoxLayout;
     
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    alignOutCancelButton = new QPushButton(tr("Cancel"));
    alignOutCancelButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(alignOutCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));        
    
    fileBox = new QGroupBox();
    
    setUpLayout();
    mainLayout->addWidget(topBox);
    mainLayout->addWidget(fileBox);
    setLayout(mainLayout);
}

QString AlignOutputFileNames::getPathFromFileName(QString fileName)
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

void AlignOutputFileNames::setUpOptional()
{
    int row = 0;
    fileLayout = new QGridLayout;
    if(type != DoGuideTreeOnly)
    {
        outputFilesLabel = new QLabel(tr("Output Alignment Files"));
        fileLayout->addWidget(outputFilesLabel, row, 0); 
        row++;
        
        if(clustalw::userParameters->getOutputClustal())
        {
            clustalNameLabel = new QLabel(tr("Clustal: "));
            clustalName = new QLineEdit();
            clustalName->setText(filePath + "aln");
            clustalName->setMinimumWidth(MinLineEditWidth);
            clustalBrowseButton = new QPushButton("Browse");
	    clustalBrowseButton->setAutoDefault(false); //nige
            connect(clustalBrowseButton, SIGNAL(clicked()), this, SLOT(browseClustal()));
            fileLayout->addWidget(clustalNameLabel, row, 0);
            fileLayout->addWidget(clustalName, row, 1);
            fileLayout->addWidget(clustalBrowseButton, row, 2);
            row++;
        }
        if(clustalw::userParameters->getOutputNbrf())
        {
            nbrfNameLabel = new QLabel(tr("NBRF/PIR: "));
            nbrfName = new QLineEdit();
            nbrfName->setText(filePath + "pir");
            nbrfName->setMinimumWidth(MinLineEditWidth);
            nbrfBrowseButton = new QPushButton("Browse");
	    nbrfBrowseButton->setAutoDefault(false); //nige
            connect(nbrfBrowseButton, SIGNAL(clicked()), this, SLOT(browseNBRF()));
            fileLayout->addWidget(nbrfNameLabel, row, 0);
            fileLayout->addWidget(nbrfName, row, 1);
            fileLayout->addWidget(nbrfBrowseButton, row, 2);
            row++;    
        }
        if(clustalw::userParameters->getOutputGCG())
        {
            gcgNameLabel = new QLabel(tr("GCG/MSF: "));
            gcgName = new QLineEdit();
            gcgName->setText(filePath + "msf");
            gcgName->setMinimumWidth(MinLineEditWidth);
            gcgBrowseButton = new QPushButton("Browse");
	    gcgBrowseButton->setAutoDefault(false); //nige
            connect(gcgBrowseButton, SIGNAL(clicked()), this, SLOT(browseGCG()));
            fileLayout->addWidget(gcgNameLabel, row, 0);
            fileLayout->addWidget(gcgName, row, 1);
            fileLayout->addWidget(gcgBrowseButton, row, 2);
            row++;     
        }
        if(clustalw::userParameters->getOutputPhylip())
        {
            phylipNameLabel = new QLabel(tr("Phylip: "));
            phylipName = new QLineEdit();
            phylipName->setText(filePath + "phy");
            phylipName->setMinimumWidth(MinLineEditWidth);
            phylipBrowseButton = new QPushButton("Browse");
	    phylipBrowseButton->setAutoDefault(false); //nige
            connect(phylipBrowseButton, SIGNAL(clicked()), this, SLOT(browsePhylip()));
            fileLayout->addWidget(phylipNameLabel, row, 0);
            fileLayout->addWidget(phylipName, row, 1);
            fileLayout->addWidget(phylipBrowseButton, row, 2);
            row++;     
        }
        if(clustalw::userParameters->getOutputGde())
        {
            gdeNameLabel = new QLabel(tr("GDE: "));
            gdeName = new QLineEdit();
            gdeName->setText(filePath + "gde");
            gdeName->setMinimumWidth(MinLineEditWidth);
            gdeBrowseButton = new QPushButton("Browse");
	    gdeBrowseButton->setAutoDefault(false); //nige
            connect(gdeBrowseButton, SIGNAL(clicked()), this, SLOT(browseGDE()));
            fileLayout->addWidget(gdeNameLabel, row, 0);
            fileLayout->addWidget(gdeName, row, 1);
            fileLayout->addWidget(gdeBrowseButton, row, 2);
            row++;     
        }
        if(clustalw::userParameters->getOutputNexus())
        {
            nexusNameLabel = new QLabel(tr("Nexus: "));
            nexusName = new QLineEdit();
            nexusName->setText(filePath + "nxs");
            nexusName->setMinimumWidth(MinLineEditWidth);
            nexusBrowseButton = new QPushButton("Browse");
	    nexusBrowseButton->setAutoDefault(false); //nige
            connect(nexusBrowseButton, SIGNAL(clicked()), this, SLOT(browseNexus()));
            fileLayout->addWidget(nexusNameLabel, row, 0);
            fileLayout->addWidget(nexusName, row, 1);
            fileLayout->addWidget(nexusBrowseButton, row, 2);
            row++;     
        }
        if(clustalw::userParameters->getOutputFasta() && type != RealignSeqRange)
        {
            fastaNameLabel= new QLabel(tr("Fasta: "));
            fastaName = new QLineEdit();
            fastaName->setText(filePath + "fasta");
            fastaName->setMinimumWidth(MinLineEditWidth);
            fastaBrowseButton = new QPushButton("Browse");
	    fastaBrowseButton->setAutoDefault(false); //nige
            connect(fastaBrowseButton, SIGNAL(clicked()), this, SLOT(browseFasta()));
            fileLayout->addWidget(fastaNameLabel, row, 0);
            fileLayout->addWidget(fastaName, row, 1);
            fileLayout->addWidget(fastaBrowseButton, row, 2);
            row++;    
        }
        if(type == RealignSeqRange)
        {
            realignSegEndGapPenaltiesLabel = new QLabel(tr("Realign Segment End Gap Penalties"));
            realignSegEndGapPenalties = new QComboBox;
            realignSegEndGapPenalties->addItem("ON");
            realignSegEndGapPenalties->addItem("OFF");
            fileLayout->addWidget(realignSegEndGapPenaltiesLabel, row, 0, 1, 2);
            fileLayout->addWidget(realignSegEndGapPenalties, row, 2, 1, 1);
            row++;
        }
    }
    okButton->setAutoDefault(true); //nige
    fileLayout->addWidget(okButton, row, 0);
    fileLayout->addWidget(alignOutCancelButton, row, 1);
    fileBox->setLayout(fileLayout);
}

void AlignOutputFileNames::setUpLayout()
{
    setUpTopBox();
    setUpOptional();
}

void AlignOutputFileNames::setUpTopBox()
{
    topBox = new QGroupBox();
    topBoxLayout = new QGridLayout();
    
    guideTreeSeqBrowseButton = new QPushButton("Browse");  
    guideTreeSeqBrowseButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    guideTreeSeqBrowseButton->setAutoDefault(false); //nige
    
    if(type == AlignFromTree || type == AlignProf2ToProf1WithTree ||
       type == AlignSeqsToProf1WithTree)
    {
        guideTreeNameLabelSeq = new QLabel(tr("Input Guide Tree:"));        
        connect(guideTreeSeqBrowseButton, SIGNAL(clicked()), this,
                SLOT(browseInputTreeSeq()));
    }
    else
    {
        guideTreeNameLabelSeq = new QLabel(tr("Output Guide Tree:"));        
        connect(guideTreeSeqBrowseButton, SIGNAL(clicked()), this,
                SLOT(browseGuideTreeSeq()));
    } 

    topBoxLayout->addWidget(guideTreeNameLabelSeq, 0, 0);
    guideTreeNameSeq = new QLineEdit();
    guideTreeNameSeq->setMinimumWidth(MinLineEditWidth);
    guideTreeNameSeq->setText(filePath + "dnd");    
    topBoxLayout->addWidget(guideTreeNameSeq, 1, 0);     
    
    topBoxLayout->addWidget(guideTreeSeqBrowseButton, 1, 1);
        
    if(type == AlignProf2ToProf1 || type == AlignProf2ToProf1WithTree)
    {
        guideTreeNameProfile2 = new QLineEdit();
        guideTreeNameProfile2->setMinimumWidth(MinLineEditWidth);
        guideTreeNameProfile2->setText(profile2Path + "dnd");    
        topBoxLayout->addWidget(guideTreeNameProfile2, 2, 0); 
        guideTreeProfile2BrowseButton = new QPushButton("Browse");  
        guideTreeProfile2BrowseButton->setSizePolicy(QSizePolicy::Maximum,
                                                     QSizePolicy::Maximum);
        guideTreeProfile2BrowseButton->setAutoDefault(false); //nige
        if(type == AlignProf2ToProf1)
        {    
            connect(guideTreeProfile2BrowseButton, SIGNAL(clicked()), this,
                    SLOT(browseGuideTreeProfile()));
        }
        else
        {
            connect(guideTreeProfile2BrowseButton, SIGNAL(clicked()), this,
                    SLOT(browseInputTreeProfile()));        
        }
        topBoxLayout->addWidget(guideTreeProfile2BrowseButton, 2, 1);   
    }        
    
    topBox->setLayout(topBoxLayout);   
}

void AlignOutputFileNames::browseInputTreeSeq()
{
    browseInputTree(guideTreeNameSeq);
}

void AlignOutputFileNames::browseInputTreeProfile()
{
    browseInputTree(guideTreeNameProfile2);
}


void AlignOutputFileNames::browseInputTree(QLineEdit* lineEditName)
{
    QString myFile;
                                   
    // Mark Jan 18th 2007: changes to remember working Dir
    FileDialog fd;
    myFile = fd.getOpenFileName(this, tr("Input Guide Tree"));
                                       
    if(myFile != "")
    {
        lineEditName->setText(myFile);
    }
}

void AlignOutputFileNames::browseGuideTreeSeq()
{
    browse(guideTreeNameSeq);
}

void AlignOutputFileNames::browseGuideTreeProfile()
{
    browse(guideTreeNameProfile2);
}

void AlignOutputFileNames::browseClustal()
{
    browse(clustalName);
}

void AlignOutputFileNames::browseNBRF()
{
    browse(nbrfName);
}

void AlignOutputFileNames::browseGCG()
{
    browse(gcgName);
}

void AlignOutputFileNames::browseNexus()
{
    browse(nexusName);
}

void AlignOutputFileNames::browseFasta()
{
    browse(fastaName);
}

void AlignOutputFileNames::browsePhylip()
{
    browse(phylipName);
}

void AlignOutputFileNames::browseGDE()
{
    browse(gdeName);
}

void AlignOutputFileNames::accept()
{
    outFileNames.reset(new clustalw::AlignmentFileNames);
    if(guideTreeNameSeq != 0)
    {
        if(guideTreeNameSeq->text() == "")
        {
            outFileNames->treeFile = filePath.toStdString() + "dnd";
        }
        else
        {
            QString file = guideTreeNameSeq->text(); 
            outFileNames->treeFile = file.toStdString();
        }
    }
    if(guideTreeNameProfile2 != 0)
    {
        if(guideTreeNameProfile2->text() == "")
        {
            outFileNames->profile2TreeFile = profile2Path.toStdString() + "dnd";
        }
        else
        {
            QString file = guideTreeNameProfile2->text(); 
            outFileNames->profile2TreeFile = file.toStdString();
        }
    }    
    if(clustalName != 0)
    {
        if(clustalName->text() == "")
        {
            outFileNames->clustalFile = filePath.toStdString() + "aln";
        }
        else
        {
            QString file = clustalName->text(); 
            outFileNames->clustalFile = file.toStdString();
        }    
    }
    if(nbrfName != 0)
    {
        if(nbrfName->text() == "")
        {
            outFileNames->nrbfFile = filePath.toStdString() + "pir";
        }
        else
        {
            QString file = nbrfName->text(); 
            outFileNames->nrbfFile = file.toStdString();
        }    
    }
    if(gcgName != 0)
    {
        if(gcgName->text() == "")
        {
            outFileNames->gcgFile = filePath.toStdString() + "gcg";
        }
        else
        {
            QString file = gcgName->text(); 
            outFileNames->gcgFile = file.toStdString();
        }    
    }
    if(phylipName != 0)
    {
        if(phylipName->text() == "")
        {
            outFileNames->phylipFile = filePath.toStdString() + "phy";
        }
        else
        {
            QString file = phylipName->text(); 
            outFileNames->phylipFile = file.toStdString();
        }     
    }
    if(gdeName != 0)
    {
        if(gdeName->text() == "")
        {
            outFileNames->gdeFile = filePath.toStdString() + "gde";
        }
        else
        {
            QString file = gdeName->text(); 
            outFileNames->gdeFile = file.toStdString();
        }     
    }
    if(nexusName != 0)
    {
        if(nexusName->text() == "")
        {
            outFileNames->nexusFile = filePath.toStdString() + "nxs";
        }
        else
        {
            QString file = nexusName->text(); 
            outFileNames->nexusFile = file.toStdString();
        }     
    }
    if(fastaName != 0)
    {
        if(fastaName->text() == "")
        {
            outFileNames->fastaFile = filePath.toStdString() + "fasta";
        }
        else
        {
            QString file = fastaName->text(); 
            outFileNames->fastaFile = file.toStdString();
        }     
    }
    if(realignSegEndGapPenalties != 0)
    {
        if(realignSegEndGapPenalties->currentIndex() == 0)
        {
            realignEndGapPen = true;
        }
        else
        {
            realignEndGapPen = false;
        }
    }                            
    setResult(QDialog::Accepted);
    hide();
}

void AlignOutputFileNames::cancel()
{
    setResult(QDialog::Rejected);
    hide();
}

void AlignOutputFileNames::browse(QLineEdit* lineEditName)
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

clustalw::AlignmentFileNames AlignOutputFileNames::getNames()
{
    return *(outFileNames.get());
}

void AlignOutputFileNames::setAllPtrsToNull()
{
    realignSegEndGapPenalties = 0;
    realignSegEndGapPenaltiesLabel = 0;
    fastaBrowseButton = 0;
    fastaName = 0;
    fastaNameLabel = 0;
    nexusBrowseButton = 0;
    nexusName = 0;
    nexusNameLabel = 0;
    gdeBrowseButton = 0;
    gdeName = 0;
    gdeNameLabel = 0;
    phylipBrowseButton = 0;
    phylipName = 0;
    phylipNameLabel = 0;
    gcgBrowseButton = 0;
    gcgName = 0;
    gcgNameLabel = 0;
    nbrfBrowseButton = 0;
    nbrfName = 0;
    nbrfNameLabel = 0;
    clustalBrowseButton = 0;
    clustalName = 0;
    clustalNameLabel = 0;
    outputFilesLabel = 0;
    fileLayout = 0;
    guideTreeSeqBrowseButton = 0;
    guideTreeNameSeq = 0;
    guideTreeProfile2BrowseButton = 0;
    guideTreeNameProfile2 = 0;    
    guideTreeNameLabelSeq = 0;
    topBoxLayout = 0;
    topBox = 0;
    fileBox = 0;
    alignOutCancelButton = 0;
    okButton = 0;
    mainLayout = 0;
}
