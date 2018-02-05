/**
 * Changes:
 *
 * 16-01-07,Nigel Brown(EMBL): Added Q_MOS_MAC preprocessor checks for Mac OS
 * X port, which uses AlignmentWidgetScroll in place of AlignmentWidget, and
 * HistogramWidgetScroll in place of HistogramWidget. This is to work around a
 * scrolling limitation in Qt4 QScrollArea. THESE CHANGES SHOULD BE REMOVED
 * WHEN QT4 QSCROLLAREA IS FIXED! *
 * Andreas Wilm: fixed in Qt 4.3.0, see bug entry 
 * http://lists.trolltech.com/qt-interest/2007-01/thread00541-0.html
 * and
 * http://trolltech.org/developer/task-tracker/index_html?method=entry&id=147640
 *
 * Mark: 18-01-2007. Changed from using QFileDialog to FileDialog.
 * Mark: 19-01-2007. Changed the width of the boxes to make them narrower. Also
 * added some larger font sizes. updateFontAndBoxes is the function I changed.
 *
 * 30-01-07,Nigel Brown(EMBL): changed sizePolicy settings for
 * CLU_AVOID_MAC_SCROLL_BUG compiled histWidget and alignWidget. Changed
 * setSizePolicy() on alignWidget and histWidget.
 *
 * 31-01-07,Nigel Brown(EMBL): added defaultFontSize member. Changed
 * updateFontAndBoxes() to use OS_MAC from compile.h to set OS-specific
 * box sizes: Mac boxes now much tighter vertically around characters (ugly -
 * not portable). Removed default case, setting locals at outset.
 *
 * 03-02-07,Nigel Brown(EMBL): changed outputColumnScores() to call
 * FileDialog::getSaveDataFileName() in place of
 * FileDialog::getSaveFileName(); the new version supplies a file suffix hint,
 * which is 'qscores' as used in clustalX. Disabled "Error: Bad name format"
 * message box in outputColumnScores() since this occurred on 'Cancel'
 * action. Added 'All Files (*)' selection.
 *
 * 16-02-07,Nigel Brown(EMBL): Changed font to Courier New; made boxwidth,
 * boxsize automatically derive from font size; added more font sizes. Changed
 * parameter order on AlignmentWidget::changeBoxSizeAndFontSize().
 *
 * 20-02-07,Nigel Brown(EMBL): Changed SeqNames size policy to expanding;
 * installed event filter on namesScroll in createGridBox(); small changes to
 * createGridBox() layout; hasSeqNameWidgetFocus() tests both the QScrollarea
 * and, if necessary, the child SeqNameWidget for focus. The latter was needed
 * after changing SeqNameWidget resizing and event handling.
 *
 * 15-03-07,Nigel Brown(EMBL): added font sizes to include classical
 * typographic scalings.
 *
 * 12-4-07, Mark Larkin : Changed destructor. We dont need to delete QObjects.
 *
 * 04-05-07,Nigel Brown(EMBL): hasSeqNameWidgetFocus() now tests all children
 * for focus; renamed hasSeqNameWidgetFocus() to hasFocus().
 *
 * 09-05-07,Nigel Brown(EMBL): Added calls in createGridBox() to new
 * AlignmentWidget::registerScrollArea() and
 * SeqNameWidget::registerScrollArea(), allowing each instance to know its own
 * containing QScrollArea (This is not the same as parentWidget()).
 *
 * 27-06-07,Nigel Brown(EMBL): For screen layout balancing: added
 * recvNumSequencesChanged() and balanceNumSequences() slots, and
 * numSequencesChanged() signal.
 *****************************************************************************/


#include <QtGui>
#include <QFile>
#include <QTextStream>
#include "AlignmentViewerWidget.h"
#include "SaveSeqFile.h"
#include "clustalW/alignment/AlignmentOutput.h"
#include "PSPrinter.h"
#include "FileDialog.h"
#include <QFontMetrics>

AlignmentViewerWidget::AlignmentViewerWidget(QWidget *parent)
 : QWidget(parent),
   namesScroll(0),
   profileNum(0),
   nHead(0)
{
    firstSeq = 0; 
    lastSeq = 0;
    alignmentToDisplay = 0;
    defaultFontSize = 10;

    /*
     * 9 out of 10 Goldilocks prefer 'Courier New'
     *
     * nige (with apologies to the three bears)
     **/
    //displayFont = new QFont("Helvetica"); //too chunky
    //displayFont = new QFont("Courier");   //too heavy
    displayFont = new QFont("Courier New"); //just right
    if (! displayFont->exactMatch())
        displayFont->setFamily("Courier");  //heavier
    displayFont->setPixelSize(defaultFontSize);

    QFontMetrics fontMetrics(*displayFont);
    QSize size = fontMetrics.size(Qt::TextSingleLine, QString("W"));
    defaultBoxWidth = size.width();
    defaultBoxSize  = size.height();
    //cout << defaultFontSize << " @ " << defaultBoxWidth << " x " << defaultBoxSize << endl;

    exRes = new clustalw::Array2D<int>();
    
    alignWidget = new AlignmentWidget(defaultBoxWidth, defaultBoxSize, displayFont);
    alignWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    histWidget = new HistogramWidget(defaultBoxWidth, defaultBoxSize);
    histWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    seqNames = new SeqNameWidget(defaultBoxSize, displayFont);
    //seqNames->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding); //- nige
    seqNames->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    createGridBox();
    
    /*
     * create a layout for this widget.
     */
    mainlayout = new QVBoxLayout;
    mainlayout->addWidget(gridGroupBox);
    setLayout(mainlayout);
    
    updateFontAndBoxes("10");    
}

AlignmentViewerWidget::~AlignmentViewerWidget()
{
    delete exRes; // Mark 12-4-07, changed destructor.
}

void AlignmentViewerWidget::setAlignmentView(clustalw::Alignment* alignPtr, int fSeq, 
                                             int lSeq)
{
    alignmentToDisplay = alignPtr;
    firstSeq = fSeq;
    lastSeq = lSeq;
    sendInfoToAlignWidget();
    sendInfoToNameWidget();
}

void AlignmentViewerWidget::updateView()
{
    nHead = makeSecStructData();
    if(nHead == 0)
    {
        nHead = makeGapPenaltyData();
    }
        
    seqNames->updateDisplay(firstSeq, lastSeq, nHead, ssName);
    
    updateHistogram(firstSeq, lastSeq);
    alignWidget->updateDisplay(firstSeq, lastSeq, exRes, nHead, secStructData);
    moveNamesScrollBarToRight();//Mark 20-07-07
}

void AlignmentViewerWidget::updateView(int fSeq, int lSeq)
{
    firstSeq = fSeq;
    lastSeq = lSeq; 
    updateView();   
}

int AlignmentViewerWidget::getLengthLongestSeqInRange()
{
    return alignmentToDisplay->getLengthLongestSequence(firstSeq, lastSeq);
}

void AlignmentViewerWidget::showLowScoringSegments(bool show, 
                                                clustalw::Array2D<int>* _lowScoreRes)
{
    lowScoreRes = _lowScoreRes;
    alignWidget->showLowScoreSeg(show, lowScoreRes);
}

void AlignmentViewerWidget::showExceptionalRes(bool show)
{
    alignWidget->showExceptionalResidues(show);
}

void AlignmentViewerWidget::updateHistogram(int fSeq, int lSeq)
{
    int numSeqs = lastSeq - firstSeq + 1;
    if(fSeq != 0 && lSeq != 0)
    {
        exRes->clearArray();
        exRes->ResizeRect(numSeqs, 
                        alignmentToDisplay->getLengthLongestSequence(firstSeq, lastSeq));
        histogram = *(alignmentToDisplay->QTcalcHistColumnHeights(firstSeq - 1, numSeqs,
                                                                exRes));
        histWidget->addHistogramInfo(&histogram);
    }
    else
    {
        histWidget->clearHistogram();
    }
}

void AlignmentViewerWidget::updateFontAndBoxes(const QString& font)
{
    int newFontSize = font.toUInt();

    switch (newFontSize) {
    case 72: //-
    case 60: //-
    case 48: //-
    case 36: //-
    case 30:
    case 28:
    case 26:
    case 24:
    case 22:
    case 21: //-
    case 20:
    case 18:
    case 16:
    case 14:
    case 12:
    case 11:
    case 10:
    case  9:
    case  8:
        break;
    default:
        newFontSize = defaultFontSize;
    }

    displayFont->setPixelSize(newFontSize);

    QFontMetrics fontMetrics(*displayFont);
    QSize size = fontMetrics.size(Qt::TextSingleLine, QString("W"));
    int newBoxWidth = size.width();
    int newBoxSize  = size.height();
    //cout << newFontSize << " @ " << newBoxWidth << " x " << newBoxSize << endl;

    histWidget->updateBoxSize(newBoxWidth, newBoxSize);
    alignmentScroll->setVisible(false);
    alignWidget->changeBoxSizeAndFontSize(newBoxWidth, newBoxSize, displayFont);
    alignmentScroll->setVisible(true);
    seqNames->changeBoxSizeAndFontSize(newBoxSize, displayFont);
    repaint();
}

void AlignmentViewerWidget::createGridBox()
{
    /* 
     * Create a grid group box for the alignment viewer part 
     */
    gridGroupBox = new QGroupBox();
    layoutGridBox = new QGridLayout; 

    alignmentScroll = new QScrollArea;
    alignmentScroll->setWidget(alignWidget);
    alignWidget->registerScrollArea(alignmentScroll); //- nige

    conservationScroll = new QScrollArea;
    conservationScroll->setWidget(histWidget);
    conservationScroll->setFixedHeight(histWidget->height());

    alignmentScroll->setBackgroundRole(QPalette::Base);
    alignmentScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    conservationScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    conservationScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    conservationScroll->setBackgroundRole(QPalette::Base);
    
    namesScroll = new QScrollArea;
    namesScroll->setWidget(seqNames);
    namesScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    namesScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    namesScroll->setBackgroundRole(QPalette::Base);

    /**
     * 20-02-07,Nigel: child intercepts resize events to adjust own size.
     */
    namesScroll->viewport()->installEventFilter(seqNames);
    seqNames->registerScrollArea(namesScroll); //- nige

    QScrollBar* alignHorizontalScroll = alignmentScroll->horizontalScrollBar();
    QScrollBar* conserveHorizontalScroll = conservationScroll->horizontalScrollBar();
    QScrollBar* alignVerticalScroll = alignmentScroll->verticalScrollBar();
    QScrollBar* namesVerticalScroll = namesScroll->verticalScrollBar();

    connect(alignHorizontalScroll, SIGNAL(valueChanged(int)), conserveHorizontalScroll,
            SLOT(setValue(int)));
    connect(alignVerticalScroll, SIGNAL(valueChanged(int)), namesVerticalScroll,
            SLOT(setValue(int)));
    connect(namesVerticalScroll, SIGNAL(valueChanged(int)), alignVerticalScroll,
            SLOT(setValue(int)));

    /*
     * Set the layout of the widgets.
     */
    layoutGridBox->addWidget(namesScroll,        0, 0, 1, 1);
    layoutGridBox->addWidget(alignmentScroll,    0, 1, 1, 5);
    layoutGridBox->addWidget(conservationScroll, 1, 1, 1, 5);

    /*
     * Set stretch and size policies.
     */
    layoutGridBox->setRowStretch(0, 5);
    layoutGridBox->setRowStretch(1, 0);
    layoutGridBox->setColumnStretch(0, 1);
    layoutGridBox->setColumnStretch(1, 5);

    //layoutGridBox->setRowStretch(0, 20);
    //layoutGridBox->setRowStretch(2, 0);
    //layoutGridBox->setColumnStretch(0, 1);
    //layoutGridBox->setColumnStretch(2, 3);

    gridGroupBox->setLayout(layoutGridBox);
    gridGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
       
}

void AlignmentViewerWidget::setColorScheme(ColorParameters* colors)
{
    alignWidget->setColorScheme(colors);
}    

void AlignmentViewerWidget::setPrintoutColorScheme(ColorParameters colors)
{
    alignWidget->setPrintoutColorScheme(colors);
}

void AlignmentViewerWidget::setBlackAndWhite()
{
    alignWidget->setBlackAndWhite();
}

void AlignmentViewerWidget::setBackGroundColoring(bool backgroundColor)
{
    alignWidget->setBackGroundColoring(backgroundColor);
}

/**
 * The function getAlignmentScrollBar is used to return the horizontal scrollbar from
 * the alignment scroll area. This is to allow the scroll lock in the profile viewing 
 * mode.
 * @return Pointer to the horizontal scroll bar in alignment scroll area.
 */
QScrollBar* AlignmentViewerWidget::getAlignmentScrollBar()
{
    return alignmentScroll->horizontalScrollBar();
}

void AlignmentViewerWidget::sendInfoToAlignWidget()
{
    alignWidget->setAlignment(alignmentToDisplay->getSeqArray(), alignmentToDisplay);
}

void AlignmentViewerWidget::sendInfoToNameWidget()
{
    seqNames->setNames(alignmentToDisplay);
}

void AlignmentViewerWidget::clearSequenceSelection()
{
    seqNames->clearSequenceSelection();
}

void AlignmentViewerWidget::clearRangeSelection()
{
    alignWidget->clearRangeSelection();
}

void AlignmentViewerWidget::selectAllSequences()
{
    seqNames->selectAllSequences();
}

void AlignmentViewerWidget::saveSequences(QString inputFileName, int type)
{
    if(firstSeq > 0 && lastSeq >= firstSeq)
    {
        int lastRes = alignmentToDisplay->getLengthLongestSequence(firstSeq, lastSeq);
    
        rangeParams alignmentRange;
        alignmentRange.firstSeq = firstSeq;
        alignmentRange.lastSeq = lastSeq;
        alignmentRange.firstRes = 1;
        alignmentRange.lastRes = lastRes;
        int code;
        SaveSeqFile saveSeqs(inputFileName, type, alignmentRange);
        code = saveSeqs.exec();
    
        if(code == QDialog::Accepted)
        {
            // save sequences as
            QString saveFileName = saveSeqs.getFileName();
            if(saveFileName.size() > 0)
            {
                // Save the file!!
                clustalw::AlignmentOutput alignmentOutput;
                alignmentOutput.openAlignmentOutput(saveFileName.toStdString());
                alignmentOutput.createAlignmentOutput(alignmentToDisplay, firstSeq, lastSeq);
            }
            else
            {
                QString tempFileName =  QDir::currentPath() + "temp";
                QString message = "File name cannot be blank. Using default:\n" +
                                   tempFileName;
                QMessageBox::information(this, "", message, QMessageBox::Ok);
            }
        }
    }
    else
    {
        QMessageBox::information(this, "", "No sequences loaded.", QMessageBox::Ok);
    }    
}

int AlignmentViewerWidget::getNumberOfSeqsSelected()
{
    return seqNames->getNumberOfSeqsSelected();
}

const std::vector<int>* AlignmentViewerWidget::getSelectedRows()
{
    return seqNames->getSelectedRows();
}

bool AlignmentViewerWidget::outputColumnScores()
{
    int numSeqsSelected = seqNames->getNumberOfSeqsSelected();
    if(numSeqsSelected == 0)
    {
        QMessageBox::information(this, "ClustalQt", 
                "Select sequences to be written by clicking on the names.", QMessageBox::Ok);
        return false;        
    }
    else
    {
        QString fileName;

        // Mark Jan 18th 2007: changes to remember working Dir    
        FileDialog fd;
        fileName = fd.getSaveDataFileName("qscores", this, tr("Choose a file name to save under."),
					  "Score files (*.qscores);;All Files (*)");
	    
        if(fileName == "")
        {
            //QMessageBox::information(this, "ClustalQt", "Error: Bad name format.",  //nige
	    //                         QMessageBox::Ok); //nige
            return false;        
        }
        else
        {
            // Open the file
            QFile columnScoreFile(fileName);
            if(!columnScoreFile.open(QIODevice::WriteOnly))
            {
                QMessageBox::information(this, "ClustalQt", 
                "Could not open save file.", QMessageBox::Ok);
                return false;    
            }
            else
            {    
                QTextStream out(&columnScoreFile);
                // Get the sequence numbers to be output from the names object
                const std::vector<int>* selected = seqNames->getSelectedRows();
                // Get the sequences from the alignmentToDisplay object!!
                const clustalw::SeqArray* seqArray = alignmentToDisplay->getSeqArray();
                // Get the scores from the histogram object! 
                const std::vector<int>* columnScores = histWidget->getColumnScores();
                int val;
                char res;
                bool gapCol;
                int colScore;
                int gapPos1 = clustalw::userParameters->getGapPos1();
                int gapPos2 = clustalw::userParameters->getGapPos2();
                
                // Get the maximum length of the sequences
                int maxLength = 0;
                int lengthCurrentSeq = 0;
                int numSeqs = alignmentToDisplay->getNumSeqs();
                
                for(int i = 0; i < numSeqs; i++)
                {
                    if((*selected)[i] == 1)
                    {
                        lengthCurrentSeq = alignmentToDisplay->getSeqLength(i + 1);
                        if(lengthCurrentSeq > maxLength)
                        {
                            maxLength = lengthCurrentSeq;
                        }
                    }
                }
                
                for(int j = 1; j <= maxLength; j++)
                {
                    // Check for a column of gaps!
                    gapCol = true;
                    for(int i = 1; i <= numSeqs; i++)
                    {                        
                        if((*selected)[i - 1] == 1)
                        {
                            if(j <= alignmentToDisplay->getSeqLength(i))
                            {
                                val = (*seqArray)[i][j];
                                if(val != gapPos1 && val != gapPos2)
                                {
                                    gapCol = false;
                                    break;
                                }
                            }
                        }
                    }

                    if(gapCol == false)
                    {                    
                        for(int i = 1; i <= numSeqs; i++)
                        {
                        
                            if((*selected)[i - 1] == 1)
                            {
                                if(j > alignmentToDisplay->getSeqLength(i))
                                {
                                    res = '-';
                                }
                                else
                                {
                                    val = (*seqArray)[i][j];
                                
                                    if(val == gapPos1 || val == gapPos2)
                                    {
                                        res = '-';
                                    }
                                    else
                                    {
                                        res = clustalw::userParameters->getAminoAcidCode(val);
                                    }
                                }
                                // Now write to file!
                                out << res << " "; 
                            }
                        }
                        // AW: problem: this comes from histogram data
                        // which sets max values to 99 instead of 100
                        colScore = (*columnScores)[j - 1];
                        out << "\t" << colScore <<"\n";
                    }
                }
                columnScoreFile.close();
                std::string name = fileName.toStdString();
                clustalw::utilityObject->info("File %s saved", name.c_str());
                return true;
            }
            
        }   
    }
}

int AlignmentViewerWidget::makeGapPenaltyData()
{
    int i,n = 0;    
    if(firstSeq > 0 && lastSeq >= firstSeq)
    {    
        char val;
        int len = alignmentToDisplay->getSeqLength(firstSeq);
        ssName = "";
        secStructData = "";
        bool ok = false;
        vector<char>* gapMask;
        
        if (profileNum == 1)
        {
            secStructData += QString(len, ' ');
            if (clustalw::userParameters->getStructPenalties1() == clustalw::GMASK && 
                clustalw::userParameters->getUseSS1() == true) 
            {
                gapMask = alignmentToDisplay->getGapPenaltyMask1();
                ok = true;
            }
        }
        else if (profileNum == 2)
        {
            if (clustalw::userParameters->getStructPenalties2() == clustalw::GMASK && 
                clustalw::userParameters->getUseSS2() == true) 
            {
                gapMask = alignmentToDisplay->getGapPenaltyMask2();
                ok = true;
            }
        }
        if(ok)
        {        
            n = 1;
            
            ssName = "Gap Penalties";
            
            for(i = 0; i < len && i < gapMask->size(); i++) 
            {
                val = (*gapMask)[i];
                if (val == clustalw::userParameters->getGapPos1())
                {
                    secStructData[i] = '-';
                }
                else
                {
                    secStructData[i] = val;
                }
            }
        }
    }
    return(n);
}

int AlignmentViewerWidget::makeSecStructData()
{
    int i,n = 0;
    char val;
    if(firstSeq > 0 && lastSeq >= firstSeq)
    {    
        vector<char> ssMask;
        int len = alignmentToDisplay->getSeqLength(firstSeq);
        ssName = "";
        secStructData = ""; 
        vector<char>* _secStructMask;
        bool ok = false;
        
        if (profileNum == 1)
        {
            if (clustalw::userParameters->getStructPenalties1() == clustalw::SECST 
                && clustalw::userParameters->getUseSS1() == true) 
            {
                _secStructMask = alignmentToDisplay->getSecStructMask1();
                ok = true;
            }
        }
        else if (profileNum == 2)
        {
            if (clustalw::userParameters->getStructPenalties2() == clustalw::SECST 
                && clustalw::userParameters->getUseSS2() == true) 
            {
                _secStructMask = alignmentToDisplay->getSecStructMask2();
                ok = true;
            }
        }
        
        if(ok)
        {    
            n = 1;
            ssName = "Structures";
            ssMask.assign(len + 10, 0);
            secStructData = "";
            secStructData += QString(len, ' ');
                
            for (i = 0; i < len && i < _secStructMask->size();i++)
            {
                ssMask[i] = _secStructMask->at(i);
            }
            clustalw::AlignmentOutput out;
            out.printSecStructMask(len, _secStructMask, &ssMask);

            for(i = 0; i < len && i < ssMask.size(); i++) 
            {
                val = ssMask[i];
                if (val == clustalw::userParameters->getGapPos1())
                {
                    secStructData[i] = '-';
                }
                else
                {
                    secStructData[i] = val;
                }
            }
        }
    }
    return(n);
}


void AlignmentViewerWidget::writeToPSFile(PostscriptFileParams postscriptParams)
{
    if(firstSeq != 0 && lastSeq != 0)
    {           
        PostscriptFileParams params = postscriptParams;
        clustalw::Array2D<QColor> colorMask;
        colorMask = alignWidget->getPrintoutColorMask();
    
        psPrintParams printParams;
        printParams.alignPtr = alignmentToDisplay;
        printParams.colorMask = &colorMask;
        
        if(alignWidget->getShowExceptionalResidues() && exRes != 0)
        {
            printParams.showExRes = true;
            printParams.exRes = *exRes;            
        }
        else
        {
            printParams.showExRes = false;
        }

        printParams.firstSeq = firstSeq;
        printParams.histogram = &histogram;
        printParams.lastSeq = lastSeq;
        
        if(alignWidget->getShowLowScoreSeg() && lowScoreRes != 0)
        {
            printParams.showLowScoreRes = true;
            printParams.lowScoreRes = *lowScoreRes;            
        }
        else
        {
            printParams.showLowScoreRes = false;
        }        
        
        if((profileNum == 1 && 
            clustalw::userParameters->getStructPenalties1() != clustalw::NONE && 
            clustalw::userParameters->getUseSS1() == true) ||
            (profileNum == 2 && 
            clustalw::userParameters->getStructPenalties2() != clustalw::NONE && 
            clustalw::userParameters->getUseSS2() == true))
        {
            printParams.nameHeader.resize(2);
            printParams.nameHeader[0] = "";
            printParams.nameHeader[1] = seqNames->getNameSSHeader(); 
            
            printParams.seqHeader.resize(2);
            printParams.seqHeader[0] = alignWidget->getConservationInfo();
            printParams.seqHeader[1] = alignWidget->getSecStructInfo();                       
        }
        else
        {
            printParams.nameHeader.resize(1);
            printParams.nameHeader[0] = "";
            
            printParams.seqHeader.resize(1);
            printParams.seqHeader[0] = alignWidget->getConservationInfo();
        }
        
        printParams.nSeqs = lastSeq - firstSeq + 1;;
    
        // Note need to do something with backGroundColors
        printParams.backGroundColors = alignWidget->getBackGroundColor();
        
        PSPrinter myPrinter(&printParams);
        myPrinter.writePSFile(&params);
        
        QString qtName = params.getWriteSeqsToFile();
        std::string name = qtName.toStdString();
        clustalw::utilityObject->info("Postscript file %s written", name.c_str());
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), 
                tr("Error: No file loaded."));
    }
}

bool AlignmentViewerWidget::seqsLoaded()
{
    if(firstSeq != 0 && lastSeq != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool AlignmentViewerWidget::hasFocus()
{
    int c = 0; // nige again
    c += namesScroll->hasFocus();
    c += seqNames->hasFocus();
    c += alignmentScroll->hasFocus();
    c += alignWidget->hasFocus();
    //these seem not to work:
    c += conservationScroll->hasFocus();
    c += histWidget->hasFocus();
    return (c > 0 ? true : false);
}

//nige
void AlignmentViewerWidget::recvNumSequencesChanged(int count) {
    //printf("RECV/EMIT(AVW, recvNumSequencesChanged) %0x (%d)\n", (int)this, count);
    emit numSequencesChanged(count);
}


void AlignmentViewerWidget::resizeEvent ( QResizeEvent * event )
{
    moveNamesScrollBarToRight();//Mark 20-7-07
}

/*Mark 20-7-07*/
void AlignmentViewerWidget::moveNamesScrollBarToRight()
{
    if(namesScroll != 0)
    {
        QScrollBar* horizNameScrollBar = namesScroll->horizontalScrollBar();
        int scrollBarWidth = horizNameScrollBar->maximumWidth();
        horizNameScrollBar->setValue(scrollBarWidth);
    }
}
