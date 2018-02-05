/**
 * Changes:
 * Mark: 18-01-2007. Changed from using QFileDialog to FileDialog.
 *
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects.
 *
 * 26-11-07,Nigel Brown(EMBL): A bug was noticed on Mac and Linux such
 * that the default printing colours were never selected. Removed the
 * default colour parameter file from the dialogue as the default is
 * internally known. Added a call to the Resources singelton to obtain
 * the true location of the color parameter file.
 *
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
#include "WritePostscriptFile.h"
#include "clustalW/general/userparams.h"
#include "clustalW/general/utils.h"
#include "clustalW/alignment/AlignmentOutput.h"
#include "ClustalQtParams.h"
#include "FileDialog.h"
#include "Resources.h"

WritePostscriptFile::WritePostscriptFile(QString inputFileName, int type, int lengthLongest)
 : blockLength(150),
   lengthLongestSeq(lengthLongest)
{
    
    title = "Write Postscript File";
    
    if(type == Sequences)
    {
        label = "Write Sequences To: ";    
    }
    else if(type == Profile1)
    {
        label = "Write Profile 1 To: ";    
    }
    else if(type == Profile2)
    {
        label = "Write Profile 2 To: ";    
    }
    else
    {
        label = "";
    }
    
    if(inputFileName.size() > 0)
    {
        std::string path = "";
        clustalw::utilityObject->getPath(inputFileName.toStdString(), &path);
        cout << fileName.toStdString() << "\n";
        
        if(path.size() > 0)
        {
            fileName = QString(path.c_str()) + "ps";
        }
        else
        {
           fileName = QDir::currentPath() + QString(QDir::separator()) + "temp.ps";
        }
    }
    std::cout << fileName.toStdString();
    otherParams = new QGroupBox;
    fileBox = new QGroupBox;
    mainLayout = new QVBoxLayout;
     
    okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel())); 
    browseButton = new QPushButton(tr("Browse"));
    browseButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));        
    
    setUpLayout();
    
    mainLayout->addWidget(fileBox);
    mainLayout->addWidget(otherParams);
    setLayout(mainLayout);
        
}

void WritePostscriptFile::setUpLayout()
{
    setFileBox();
    setUpOtherParams();
}

void WritePostscriptFile::setFileBox()
{
    fileLayout = new QGridLayout;
    writeSeqsToLabel = new QLabel(label);
    writeSeqsTo = new QLineEdit;
    writeSeqsTo->setText(fileName);
    
    psColorsFileLabel = new QLabel("PS Colors File:");
    psColorsFile = new QLineEdit;
    
    //26-11-07,nige
    //psColorsFile->setText(psPrintFile);
    psColorsFile->setText("");
    //26-11-07,nige: end

    fileLayout->addWidget(writeSeqsToLabel, 0, 0);
    fileLayout->addWidget(writeSeqsTo, 1, 0);
    fileLayout->addWidget(browseButton, 1, 1);
    fileLayout->addWidget(psColorsFileLabel, 2, 0);
    fileLayout->addWidget(psColorsFile, 3, 0);
    
    fileBox->setLayout(fileLayout);
}

void WritePostscriptFile::setUpOtherParams()
{
    otherParamLayout = new QGridLayout;
    
    pageSizeLabel = new QLabel("Page Size");
    pageSize = new QComboBox();
    pageSize->addItem("A4");
    pageSize->addItem("A3");
    pageSize->addItem("US Letter");
    pageSize->setCurrentIndex(A4);
    connect(pageSize, SIGNAL(activated(int)), this, SLOT(changeBlockSize()));
    
    orientationLabel = new QLabel("Orientation");
    orientation = new QComboBox();
    orientation->addItem("Landscape");
    orientation->addItem("Portrait");
    pageSize->setCurrentIndex(LANDSCAPE);
    connect(orientation, SIGNAL(activated(int)), this, SLOT(changeBlockSize()));
    
    printHeaderLabel = new QLabel("Print Header: ");
    printHeader = new QComboBox();
    printHeader->addItem("Yes");
    printHeader->addItem("No");
    printHeader->setCurrentIndex(0);
    
    printQualityCurveLabel = new QLabel("Print Quality Curve: ");
    printQualityCurve = new QComboBox();
    printQualityCurve->addItem("Yes");
    printQualityCurve->addItem("No");
    printQualityCurve->setCurrentIndex(0);
    
    printRulerLabel = new QLabel("Print Ruler: ");
    printRuler = new QComboBox();
    printRuler->addItem("Yes");
    printRuler->addItem("No");
    printRuler->setCurrentIndex(0);
    
    printResidueNumbersLabel = new QLabel("Print Residue Numbers: ");
    printResidueNumbers = new QComboBox();
    printResidueNumbers->addItem("Yes");
    printResidueNumbers->addItem("No");
    printResidueNumbers->setCurrentIndex(0);
    
    resizeToFitLabel = new QLabel("Resize to fit page: ");
    resizeToFit = new QComboBox();
    resizeToFit->addItem("Yes");
    resizeToFit->addItem("No");
    resizeToFit->setCurrentIndex(0);
    
    QString num;
        
    printFromPosLabel = new QLabel("Print from position: ");
    printFromPos = new QLineEdit;
    num.setNum(1);
    printFromPos->setText(num);
    
    printToPosLabel = new QLabel("to: ");
    printToPos = new QLineEdit;
    num.setNum(lengthLongestSeq);
    printToPos->setText(num);
    
    num.setNum(getSuggestedBlockSize());
    useBlockLengthLabel = new QLabel("Use block length: ");
    useBlockLength = new QLineEdit;
    useBlockLength->setText(num);
    
    otherParamLayout->addWidget(pageSizeLabel, 0, 0);
    otherParamLayout->addWidget(pageSize, 0, 1);
    otherParamLayout->addWidget(orientationLabel, 0, 2);
    otherParamLayout->addWidget(orientation, 0, 3);
    otherParamLayout->addWidget(printHeaderLabel, 1, 0);
    otherParamLayout->addWidget(printHeader, 1, 1);
    otherParamLayout->addWidget(printQualityCurveLabel, 1, 2);
    otherParamLayout->addWidget(printQualityCurve, 1, 3);
    otherParamLayout->addWidget(printRulerLabel, 2, 0);
    otherParamLayout->addWidget(printRuler, 2, 1);
    otherParamLayout->addWidget(printResidueNumbersLabel, 2, 2);
    otherParamLayout->addWidget(printResidueNumbers, 2, 3);
    otherParamLayout->addWidget(resizeToFitLabel, 3, 0);
    otherParamLayout->addWidget(resizeToFit, 3, 1);
    otherParamLayout->addWidget(printFromPosLabel, 4, 0);
    otherParamLayout->addWidget(printFromPos, 4, 1);
    otherParamLayout->addWidget(printToPosLabel, 4, 2);
    otherParamLayout->addWidget(printToPos, 4, 3);
    otherParamLayout->addWidget(useBlockLengthLabel, 5, 0);
    otherParamLayout->addWidget(useBlockLength, 5, 1);
    otherParamLayout->addWidget(okButton, 6, 0);
    otherParamLayout->addWidget(cancelButton, 6, 1);
    otherParams->setLayout(otherParamLayout);
}

void WritePostscriptFile::accept()
{
    // writeSeqsTo
    QString writeTo;
    writeTo = writeSeqsTo->text();    
    if(writeTo.size() == 0)
    {
        writeTo = QDir::currentPath() + "temp.ps";
    }
    
    // psColorsFile
    QString psColors = psColorsFile->text();    
    if(psColors.size() == 0)
    {
        //26-11-07,nige: end
        //psColors = QDir::currentPath() + "usedefaultfile";
        Resources *res = Resources::Instance();    
        psColors = res->searchPathsForFile(psPrintFile);
        //26-11-07,nige: end
    }
        
    // pageSize
    int pageSizeChoice;
    if(pageSize->currentIndex() == 0)
    {
        pageSizeChoice = A4;
    }
    else if(pageSize->currentIndex() == 1)
    {
        pageSizeChoice = A3;
    }
    else
    {
        pageSizeChoice = USLETTER;
    }
    
    // orientation
    int orientationChoice;
    if(orientation->currentIndex() == 0)
    {
        orientationChoice = LANDSCAPE;
    }
    else
    {
        orientationChoice = PORTRAIT;
    }
    
    // printHeader
    bool printHead; 
    if(printHeader->currentIndex() == 0)
    {
        printHead = true;
    }
    else
    {
        printHead = false;
    }
    
    // printQualityCurve
    bool printCurve;
    if(printQualityCurve->currentIndex() == 0)
    {
        printCurve = true;
    }
    else
    {
        printCurve = false;
    }
    
    // printRuler
    bool printRule;
    if(printRuler->currentIndex() == 0)
    {
        printRule = true;
    }
    else
    {
        printRule = false;
    }
    
    // printResidueNumbers
    bool printResNum;
    if(printResidueNumbers->currentIndex() == 0)
    {
        printResNum = true;
    }
    else
    {
        printResNum = false;
    }
    
    // resizeToFit
    bool resize;
    if(resizeToFit->currentIndex() == 0)
    {
        resize = true;
    }
    else
    {
        resize = false;
    }
    
    // printFromPos
    QString lineText = printFromPos->text();
    int from;
    if(lineText.size() > 0)
    {
        bool ok;
        from = lineText.toInt(&ok);
        if(!ok)
        {
            from = 1;
        }
        else if(from < 1 || from > lengthLongestSeq)
        {
            from = 1;
        }
    } 
    else
    {
        from = 1;
    }
    
    // printToPos
    lineText = printToPos->text();
    int to;
    if(lineText.size() > 0)
    {
        bool ok;
        to = lineText.toInt(&ok);
        if(!ok)
        {
            to = lengthLongestSeq;
        }
        else if(to < 1 || to > lengthLongestSeq)
        {
            to = lengthLongestSeq;
        }        
    } 
    else
    {
        to = lengthLongestSeq;
    }
        
    // useBlockLength
    lineText = useBlockLength->text();
    int blockLen;
    if(lineText.size() > 0)
    {
        bool ok;
        blockLen = lineText.toInt(&ok);
        if(!ok)
        {
            blockLen = getSuggestedBlockSize();
        }
        else if(blockLen < 1)
        {
            blockLen = getSuggestedBlockSize();                               
        }
        else if(blockLen > getSuggestedBlockSize())
        {
            int answer;
            QString myMessage = "Block size is greater than suggested size.\n"; 
            myMessage += "Sequences may not fit on the page. Use suggested size?";
            answer = QMessageBox::information(this, "", myMessage, QMessageBox::Yes,
                                             QMessageBox::No);
            if(answer == QMessageBox::Yes)
            {
                blockLen = getSuggestedBlockSize();
            }       
        }        
    } 
    else
    {
        blockLen = getSuggestedBlockSize();
    }
    
    params.reset(new PostscriptFileParams(writeTo, psColors, pageSizeChoice,
                                          orientationChoice, printHead, printCurve, 
                                          printRule, printResNum, resize, from, to,
                                          blockLen));
                                          
    setResult(QDialog::Accepted);
    hide();
}

void WritePostscriptFile::cancel()
{
    setResult(QDialog::Rejected);
    hide();
}

void WritePostscriptFile::browse()
{
    QString myFile;
    
    // Mark Jan 18th 2007: changes to remember working Dir    
    FileDialog fd;
    myFile = fd.getSaveFileName(this, tr("Save as"));
                                                                      
    if(myFile != "")
    {
        writeSeqsTo->setText(myFile);
    }
}

void WritePostscriptFile::changeBlockSize()
{
    QString num;
    num.setNum(getSuggestedBlockSize());
    useBlockLength->setText(num);
}

PostscriptFileParams WritePostscriptFile::getParams()
{
    return *(params.get());
}

int WritePostscriptFile::getSuggestedBlockSize()
{
    int _orientation = orientation->currentIndex();
    int paperSize = pageSize->currentIndex();
    
    if(_orientation == LANDSCAPE && paperSize == A4)
    {
        return 150;
    }
    else if(_orientation == LANDSCAPE && paperSize == A3)
    {
        return 250;
    }
    else if(_orientation == LANDSCAPE && paperSize == USLETTER)
    {
        return 150;
    }   
    else if(_orientation == PORTRAIT && paperSize == A4)
    {
        return 80;
    }
    else if(_orientation == PORTRAIT && paperSize == A3)
    {
        return 150;
    }
    else if(_orientation == PORTRAIT && paperSize == USLETTER)
    {
        return 150;
    }
    else
    {
        return 80; // minimum size that will fit any page.
    }        
    
}
