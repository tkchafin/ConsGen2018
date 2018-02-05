/**
 * Changes:
 * Mark: 18-01-2007. Changed from using QFileDialog to FileDialog.
 *
 * 12-03-07,Nigel Brown(EMBL): fixed range selection (bz0029) in accept(),
 * which was miscalculating the end point of the selection. Dialog no longer
 * exits and outputs default range when given bad range values; changed
 * behaviour to allow user to correct input mistake and added more specific
 * textbox warnings. Also added check that user has selected at least one
 * output format and added corresponding textbox warning.
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
#include "SaveSeqFile.h"
#include "clustalW/general/userparams.h"
#include "clustalW/general/utils.h"
#include "clustalW/alignment/AlignmentOutput.h"
#include "FileDialog.h"

SaveSeqFile::SaveSeqFile(QString inputFileName, int _type, rangeParams params)
 : type(_type)
{
    clustalw::userParameters->setRangeFromToSet(false);
    clustalw::userParameters->setSeqRange(false);
    firstSeq = params.firstSeq;
    lastSeq = params.lastSeq;
    firstRes = params.firstRes;
    lastRes = params.lastRes;

    if(type == Sequences)
    {
        title = "SAVE SEQUENCES";
        label = "Save Sequences As: (File extension will be appended)";    
    }
    else if(type == Profile1)
    {
        title = "SAVE PROFILE";
        label = "Save Profile 1 As: (File extension will be appended)";    
    }
    else if(type == Profile2)
    {
        title = "SAVE PROFILE";
        label = "Save Profile 2 As: (File extension will be appended)";    
    }
    else
    {
        title = "";
        label = "";
    }

    if(inputFileName.size() > 0)
    {
        std::string path = "";
        clustalw::utilityObject->getPath(inputFileName.toStdString(), &path);
        
        if(path.size() > 0)
        {
            fileName = QString(path.c_str());
            int len = fileName.size();
            if(fileName[len -1] == '.')
            {
                fileName[len -1] = ' ';
                fileName = fileName.trimmed();
            }
        }
        else
        {
           fileName = QDir::currentPath() + QString(QDir::separator()) + "temp";
        }
    }    
    fileGridBox = new QGroupBox("Format");
    otherParams = new QGroupBox;
    
    mainLayout = new QVBoxLayout;
     
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel())); 
    browseButton = new QPushButton(tr("Browse"));
    browseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));        
    setUpLayout();
    
    mainLayout->addWidget(fileGridBox);
    mainLayout->addWidget(otherParams);
    setLayout(mainLayout);
}

void SaveSeqFile::setUpLayout()
{
   setUpRadioButtons(); 
   grid = new QGridLayout; 
   grid->addWidget(clustal, 0, 0);
   grid->addWidget(nbrf, 0, 1);
   grid->addWidget(gcg, 1, 0);
   grid->addWidget(phylip, 1, 1);
   grid->addWidget(gde, 2, 0);
   grid->addWidget(nexus, 2, 1);
   grid->addWidget(fasta, 3, 0);
   fileGridBox->setLayout(grid);
   
   setUpOtherParams();
   otherParamLayout = new QGridLayout; 
   otherParamLayout->addWidget(gdeOutputCaseLabel, 0, 0);
   otherParamLayout->addWidget(gdeOutputCase, 0, 1);
   otherParamLayout->addWidget(clustalSequenceNumbersLabel, 1, 0);
   otherParamLayout->addWidget(clustalSequenceNumbers, 1, 1);
   otherParamLayout->addWidget(useRangeInfo, 2, 0);
   otherParamLayout->addWidget(rangeFromLabel, 3, 0);
   otherParamLayout->addWidget(rangeFrom, 3, 1);
   otherParamLayout->addWidget(rangeToLabel, 3, 2);
   otherParamLayout->addWidget(rangeTo, 3, 3);
   otherParamLayout->addWidget(rangeNumbersLabel, 4, 0);
   otherParamLayout->addWidget(rangeNumbers, 4, 1);
   otherParamLayout->addWidget(saveAsLabel, 5, 0);
   otherParamLayout->addWidget(saveAs, 6, 0, 1, 3);
   otherParamLayout->addWidget(browseButton, 6, 4);
   otherParamLayout->addWidget(okButton, 7, 0);
   otherParamLayout->addWidget(cancelButton, 7, 1);
   otherParams->setLayout(otherParamLayout); 
}

void SaveSeqFile::setUpRadioButtons()
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

void SaveSeqFile::setUpOtherParams()
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
    
    
    rangeNumbers = new QComboBox();
    rangeNumbers->addItem("Off");
    rangeNumbers->addItem("On");
    if(clustalw::userParameters->getSeqRange())
    {
        clustalSequenceNumbers->setCurrentIndex(1);
    }
    else
    {
        clustalSequenceNumbers->setCurrentIndex(0);
    }
            
    useRangeInfo = new QCheckBox("Use range information: ");
    useRangeInfo->setChecked(false);
	if(!clustalw::userParameters->getRangeFromToSet())
    {
		useRangeInfo->setChecked(false);
	}
	else
	{
	    useRangeInfo->setChecked(true);
	}
    connect(useRangeInfo, SIGNAL(clicked()), this, SLOT(changeUseRange()));
     
    rangeFrom = new QLineEdit;
    rangeTo = new QLineEdit;
    
    QString num;
    num = num.setNum(firstRes);
    rangeFrom->setText(num);
    
    num = num.setNum(lastRes);
    rangeTo->setText(num);
    
    rangeFromLabel = new QLabel(tr("Save range from: "));
    rangeToLabel = new QLabel(tr(" to: "));
    rangeNumbersLabel = new QLabel(tr("Include range numbers: ")); 
    
    saveAsLabel = new QLabel(label);
    saveAs = new QLineEdit;
    saveAs->setText(fileName);
    changeUseRange();
}

void SaveSeqFile::changeUseRange()
{
    if(useRangeInfo->isChecked())
    {
        // Enable
        rangeFrom->setEnabled(true);
        rangeTo->setEnabled(true);
        rangeNumbers->setEnabled(true);
    }
    else
    {
        // disable
        rangeFrom->setEnabled(false);
        rangeTo->setEnabled(false);
        rangeNumbers->setEnabled(false);
    }
}

void SaveSeqFile::browse()
{
    QString myFile;
                                   
    // Mark Jan 18th 2007: changes to remember working Dir    
    FileDialog fd;
    myFile = fd.getSaveFileName(this, tr("Save as"));
                                       
    if(myFile != "")
    {
        saveAs->setText(myFile);
    }
}

void SaveSeqFile::cancel()
{
    setResult(QDialog::Rejected);
    close();
}

void SaveSeqFile::accept()
{
    // Find out which ones (and how many [nige]) have been checked!
    int selected = 0;

    /*
      If 'userParameters::setOutputWhatever(bool)' returned boolean, this
      would be 1/6 as long:
      
      selected += clustalw::userParameters->setOutputClustal(clustal->isChecked());
      selected += clustalw::userParameters->setOutputGCG(gcg->isChecked());
      ...
    */

    if (clustal->isChecked()) {
        clustalw::userParameters->setOutputClustal(true);
        selected++;
    } else {
        clustalw::userParameters->setOutputClustal(false);
    }
    if (gcg->isChecked()) {
        clustalw::userParameters->setOutputGCG(true);
        selected++;
    } else {
        clustalw::userParameters->setOutputGCG(false);
    }
    if (gde->isChecked()) {
        clustalw::userParameters->setOutputGde(true);
        selected++;
    } else {
        clustalw::userParameters->setOutputGde(false);
    }
    if (fasta->isChecked()) {
        clustalw::userParameters->setOutputFasta(true);
        selected++;
    } else {
        clustalw::userParameters->setOutputFasta(false);
    }
    if (nbrf->isChecked()) {
        clustalw::userParameters->setOutputNbrf(true);
        selected++;
    } else {
        clustalw::userParameters->setOutputNbrf(false);
    }
    if (phylip->isChecked()) {
        clustalw::userParameters->setOutputPhylip(true);
        selected++;
    } else {
        clustalw::userParameters->setOutputPhylip(false);
    }
    if (nexus->isChecked()) {
        clustalw::userParameters->setOutputNexus(true);
        selected++;
    } else {
        clustalw::userParameters->setOutputNexus(false);
    }

    if (! selected) {
        QMessageBox::information(this, "", "Please select at least one output format.",
                                 QMessageBox::Ok);
        return;
    }
    
    // if range is selected
    // Check the range is ok. If it isn't use the default
    if (useRangeInfo->isChecked())
    {
        if (rangeNumbers->currentIndex() == 0)
        {
            clustalw::userParameters->setRangeFromToSet(false);
            clustalw::userParameters->setSeqRange(false);
        }
        else
        {
            clustalw::userParameters->setRangeFromToSet(true);
            clustalw::userParameters->setSeqRange(true);
        }
            
        bool ok1;
        QString textFrom = rangeFrom->text();
        int numFrom = textFrom.toInt(&ok1);
            
        bool ok2;
        QString textTo = rangeTo->text();
        int numTo = textTo.toInt(&ok2);
        
        if (!ok1 || !ok2) {
            QMessageBox::information(this, "", "Range values must be integers.",
                                     QMessageBox::Ok);
            return;
        }
        
        if (numFrom < firstRes || numFrom > lastRes ||
            numTo   < firstRes || numTo   > lastRes) {
            QMessageBox::information(this, "", "Invalid range.",
                                     QMessageBox::Ok);
            return;
        }

        if (numTo < numFrom) {
            QMessageBox::information(this, "", "Invalid range.",
                                     QMessageBox::Ok);
            return;
        }

        clustalw::userParameters->setRangeFrom(numFrom);
        clustalw::userParameters->setRangeTo(numTo);
    }

    // GDE output case
    if(gdeOutputCase->currentIndex() == 0)
    {
        clustalw::userParameters->setLowercase(true);
    }
    else
    {
        clustalw::userParameters->setLowercase(false);
    }
        
    // Clustalw sequence numbers
    if(clustalSequenceNumbers->currentIndex() == 1)
    {
        clustalw::userParameters->setClSeqNumbers(true);
    }
    else
    {
        clustalw::userParameters->setClSeqNumbers(false);
    }
        
    // save sequences as
    fileName = saveAs->text();
    int len = fileName.size();
    if(fileName == "")
    {
        fileName = QDir::currentPath() + "temp.aln";
    }
    else if(fileName[len -1] != '.')
    {
        fileName += ".";
    }
    
    setResult(QDialog::Accepted);
    hide();         
}
