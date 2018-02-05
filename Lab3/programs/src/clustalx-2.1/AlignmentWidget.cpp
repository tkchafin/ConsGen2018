/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 * The AlignmentWidget is used to display a multiple alignment. It takes sequences
 * in the form of QString objects. The conservation information is displayed,
 * if it is available.
 *
 * Changes:
 *
 * 18-12-06,Nigel Brown(EMBL): calcConserveInfo(): removed decrement by 2
 * of maxAA, since this is already done by the owner class; simplified a
 * few l-value expressions.
 *
 * 16-01-07,Nigel Brown(EMBL): added drawContents() called by paintEvent().
 *
 * 31-01-07,Nigel Brown(EMBL): changed && to || in changeBoxSizeAndFontSize().
 *
 * 03-02-07,Nigel Brown(EMBL): changed pencolor to white for
 * lowScoreColoredRes and exceptionalColoredRes in generatePixMaps()
 * in keeping with 'clustalX'.
 *
 * 06-02-07,Nigel Brown(EMBL): darker low scoring and exceptional residues for
 * Mac.
 *
 * 20-02-07,Nigel Brown(EMBL): constructor and changeBoxSizeAndFontSize()
 * parameters reordered.
 *
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects.
 *
 * 25-04-07,Nigel Brown(EMBL): Removed keyPressEvent(), keyReleaseEvent(),
 * shiftPressed. Keyboard events handled by KeyController singleton. Fixed
 * problem in shift-selection of columns which did not extend selection
 * backwards by replacing with new procedure selectColumns().
 *
 * 04-05-07,Nigel Brown(EMBL): moved getFirstSelectedCol() and
 * getSecondSelectedCol() in from include file; modified them to ensure they
 * return low-to-high ordered values, regardless of the actual mouse selection
 * ordering.
 *
 * 09-05-07,Nigel Brown(EMBL): added ensureVisible() call on new
 * 'scrollArea' member inside mouseMoveEvent() to scroll when column
 * range selection extends beyond visible.
 *
 * 2007-12-03, Andreas Wilm (UCD): removed nigels comments from
 * include line to avoid compiler warnings
 *
 ****************************************************************************/

#include <QtGui>
#include "AlignmentWidget.h"
#include "clustalW/general/userparams.h"
#include "ClustalQtParams.h"
#include "KeyController.h"
// KeyController.h - nige
//AW not needed anymore#include "compile.h"
//#include <iostream>
//using namespace std;

/**
 * This is the constructor for an AlignmentWidget.
 * @param num The number of sequences to be displayed in the AlignmentWidget.
 * @param length The length of the alignment.
 * @param boxsize The size of the box to be used in displaying one residue.
 * @param *parent The parent widget of this widget, an optional parameter.
 */
AlignmentWidget::AlignmentWidget(int boxwidth, int boxsize, QFont* font, QWidget *parent)
    : QWidget(parent),
      blackAndWhite(false),
      selectedColoredRes(0),
      lowScoreColoredRes(0),
      exceptionalColoredRes(0),
      conservationPixmaps(0),
      secStructPixmaps(0),
      backGroundColor(true),
      secStruct(0),
      defaultIndex(0)
{
    setEnabled(true);
    setFocusPolicy(Qt::StrongFocus);

    defaultPixMap.fill(Qt::white);
    exResLoaded = false;
    showExceptionalRes = false;
    lowScoreResLoaded = false;
    showLowScoreRes = false;
    defaultColor = QColor(Qt::white);
    secStructInfo = "";   
    alignment = 0;
    firstSeq = 0;
    lastSeq = 0;
    exceptionalRes = 0;
    lowScoreRes = 0;
    colorAlignment = 0;
    /*
     * Set up initial values
     */
    firstColumnSelected = -1;
    secondColumn = -1;
    setMouseTracking(true);
    boxSize = boxsize;
    boxWidth = boxwidth;
    defaultPixMap = QPixmap(boxWidth, boxSize);
    defaultPixMap.fill();  
    offSetFromTop = 2;
    displayStartPos = 0;

    conserveDisplay = false;
    mousePressed = false;
    alignmentSet = false;
    showAlignment = false;
    displayFont = *font;

    alignLength = 0;
    numSequencesSoFar = 0;
    /*
     * Set up the colors to be used in displaying the alignment
     */
    selectedColor = new QColor;
    lightGrey = new QColor;
    slateGrey = new QColor;
    backGroundGrey = new QColor;
   
    selectedColor->setRgb(197, 197, 226);
#if OS_MAC
    lightGrey->setRgb(112, 112, 112);
    slateGrey->setRgb(60, 60, 60);
#else
    lightGrey->setRgb(211, 211, 211);
    slateGrey->setRgb(112, 128, 144);
#endif
    backGroundGrey->setRgb(221, 221, 221);
    setUpWeakAndStrongGroups();
   
    conservationChars = " .:*";   
}

void AlignmentWidget::setAlignment(const clustalw::SeqArray* align,
                                   clustalw::Alignment* _alignObjPtr)
{
    alignment = align;
    alignObjPtr = _alignObjPtr;
    alignmentSet = true;
    firstSeq = 0;
    lastSeq = 0;
    numSequencesSoFar = 0;
   
    colorAlignment = new clustalw::Array2D<QPixmap*>;
    selectedColoredRes = new std::vector<QPixmap>;
    lowScoreColoredRes = new std::vector<QPixmap>;
    exceptionalColoredRes = new std::vector<QPixmap>;
    conservationPixmaps = new std::vector<QPixmap>;
    secStructPixmaps = new std::vector<QPixmap>;
   
    calcConserveInfo();
    updateSizeHint();
}

void AlignmentWidget::updateDisplay(int fSeq, int lSeq, clustalw::Array2D<int>* exRes,
                                    int nhead, QString secStructString)
{
    if(fSeq != 0 && lSeq != 0)
    {
        lowScoreResLoaded = false;
        showLowScoreRes = false;
        alignmentSet = true;
        showAlignment = true;
        exResLoaded = true;
        firstSeq = fSeq;
        lastSeq = lSeq;
        numSequencesSoFar = lastSeq - firstSeq + 1;
        secStruct = nhead;
        secStructInfo = secStructString;
        updateSizeHint();
        calcConserveInfo();
        exceptionalRes = exRes;
        calculatePixMaps();
        firstColumnSelected = -1; // dont show previous selection
        secondColumn = -1;       
    }
    else
    {
        firstSeq = 0;
        lastSeq = 0;
        alignmentSet = false;
        showAlignment = false;
        lowScoreResLoaded = false;
        showLowScoreRes = false;       
        exResLoaded = false;
        secStruct = 0;
        secStructInfo = "";       
        colorAlignment->clearArray();
        //selectedColoredRes->clear();
        numSequencesSoFar = 0;
        firstColumnSelected = -1;
        secondColumn = -1;
        updateSizeHint();      
    }
    update();
}

void AlignmentWidget::calculatePixMaps()
{
    int lengthLongestSeq = alignObjPtr->getLengthLongestSequence(firstSeq,lastSeq);
    std::map<char, QColor*>::iterator colorIterator;
    QFontMetrics fontMetrics(displayFont);
   
    colorAlignment->clearArray();
    colorAlignment->ResizeRect(numSequencesSoFar + getExtraRowNum(), lengthLongestSeq + 1);  
    makeColorMask(&colMask, currentColorScheme);

    int extraRow = getExtraRowNum();
    int currentSeq;       
    char currentChar;
    int codeForAA;
    int colorRow;
    int codeForSpace = clustalw::userParameters->resIndex(
                                          clustalw::userParameters->getAminoAcidCodes(), '-');
    if(secStruct == 1)
    {
        generateSecStructurePixmaps();
    }
                                         
    for(int i = 0; i < colorAlignment->getRowSize(); i++)
    {
        currentSeq = firstSeq + i - extraRow;
        for(unsigned int j = 0; j < lengthLongestSeq; j++)
        {
            (*colorAlignment)[i][j] = &defaultPixMap;                              

            if(i == 0)
            {
                // Conservation information.
                currentChar = conservationInfo[j];
                int code = clustalw::userParameters->resIndex(conservationChars, currentChar);
               
                if(code >= 0 && code < conservationPixmaps->size())
                {
                    (*colorAlignment)[i][j] = &(*conservationPixmaps)[code];
                }
                else
                {
                    (*colorAlignment)[i][j] = &defaultPixMap;
                }
            }
            else if(i == 1 && secStruct == 1)
            {
                (*colorAlignment)[i][j] = &(*secStructPixmaps)[j];
            }
            else
            {
                if((*alignment)[currentSeq].size() > j + 1)
                {
                    codeForAA = (*alignment)[currentSeq][j + 1];
                    if(codeForAA == 31 || codeForAA == 30)
                    {
                        char res = '-';
                        codeForAA = clustalw::userParameters->resIndex(
                                         clustalw::userParameters->getAminoAcidCodes(), res);
                    }
                }
                else
                {
                    codeForAA = -1;
                }               
               
                if(lowScoreResLoaded && showLowScoreRes &&
                  (*lowScoreRes)[currentSeq - firstSeq][j] == 1)
                {
                    if(codeForAA >=0 && codeForAA < lowScoreColoredRes->size())
                    {              
                        (*colorAlignment)[i][j] = &(*lowScoreColoredRes)[codeForAA];
                    }
                    else
                    {
                        (*colorAlignment)[i][j] = &defaultPixMap;
                    }                    
                }
                else if(showExceptionalRes && exResLoaded &&
                       (*exceptionalRes)[currentSeq - firstSeq][j] == 1)
                {
                    if(codeForAA >=0 && codeForAA < exceptionalColoredRes->size())
                    {              
                        (*colorAlignment)[i][j] = &(*exceptionalColoredRes)[codeForAA];
                    }
                    else
                    {
                        (*colorAlignment)[i][j] = &defaultPixMap;
                    }
                }
                else
                {
                    if(colMask.getRowSize() > (i - extraRow) && colMask.getColSize() > j)
                    {
                        if(blackAndWhite)
                        {
                            colorRow = 0;
                        }
                        else
                        {
                            colorRow = colMask[i - extraRow][j];
                        }
                    }
                    else
                    {
                        colorRow = 0; // default
                    }
                    if(colorRow >= 0 && colorRow < coloredLetters.getRowSize() &&
                       codeForAA >=0 && codeForAA < coloredLetters.getColSize())
                    {              
                        (*colorAlignment)[i][j] = &coloredLetters[colorRow][codeForAA];
                    }
                    else
                    {
                        (*colorAlignment)[i][j] = &defaultPixMap;
                    }                   
                }
            }
        }
    }
    update();
}

void AlignmentWidget::generateSecStructurePixmaps()
{
    QFontMetrics fontMetrics(displayFont);
    int lengthLongestSeq = alignObjPtr->getLengthLongestSequence(firstSeq,lastSeq);
    secStructPixmaps->clear();
    secStructPixmaps->resize(lengthLongestSeq + 1);
    defaultGreyPixMap = QPixmap(boxWidth, boxSize);
    defaultGreyPixMap.fill(backGroundGrey->rgb());
      
    for(int i = 0; i < secStructInfo.size(); i++)
    {
        if(i < secStructPixmaps->size() && i < secStructInfo.size())
        {
            displayFont.setBold(true);          
               
            // secondary structure info
            (*secStructPixmaps)[i] = defaultGreyPixMap;
            (*secStructPixmaps)[i].fill(backGroundGrey->rgb());
           
            QPainter paintChar(&(*secStructPixmaps)[i]);
            paintChar.setFont(displayFont);                        
            paintChar.drawText((boxWidth / 2) -
                fontMetrics.width(secStructInfo[i])/2,
                offSetFromTop + fontMetrics.ascent(),
                QString(secStructInfo[i]));
        }
    }
}

void AlignmentWidget::showLowScoreSeg(bool show, clustalw::Array2D<int>* _lowScoreRes)
{
    int numSeqs = lastSeq - firstSeq + 1;
    if(_lowScoreRes != 0)
    {
        if((_lowScoreRes->getRowSize() == numSeqs + 1) &&
           (_lowScoreRes->getColSize() == alignObjPtr->getLengthLongestSequence(firstSeq,
                                                                             lastSeq) + 1))
        {
            lowScoreRes = _lowScoreRes;
            lowScoreResLoaded = true;
            showLowScoreRes = show;
        }
    }
    else
    {
        lowScoreResLoaded = false;
        showLowScoreRes = false;
    }
    calculatePixMaps();
    update();
}

/**
 * The function sizeHint is overloaded from the QWidget class. It gives a recomended size for
 * the widget when resizing is performed.
 */
QSize AlignmentWidget::sizeHint() const
{
    int extraRow;
    if(secStruct == 1)
    {
        extraRow = 2;
    }
    else
    {
        extraRow = 1;
    }
    return QSize(alignLength * boxWidth, (numSequencesSoFar + extraRow) * boxSize);
}

void AlignmentWidget::showExceptionalResidues(bool show)
{
    showExceptionalRes = show;
    calculatePixMaps();
    update();
}

/**
 * This function is an event handler. It finds the column in the alignment
 * that has been clicked.
 * @param *event A mouse event.
 */
void AlignmentWidget::mousePressEvent(QMouseEvent *event)  //nige
{
    KeyController *kbd = KeyController::Instance();

    if (event->button() == Qt::LeftButton) {
        mousePressed = true;

        int column = (event->x() / boxWidth);
        if (kbd->shiftPressed()) {
            selectColumns(column);
        } else {
            firstColumnSelected = column;
            secondColumn = -1;
        }
        update(); //nige
        return;
    }
    QWidget::mousePressEvent(event);
}

/**
 * This function is an event handler. It finds the column in the alignment that the mouse
 * is at when the mouse button has been released.
 * @param *event A mouse event.
 */
void AlignmentWidget::mouseReleaseEvent(QMouseEvent *event)  //nige
{
    if (event->button() == Qt::LeftButton) {
        mousePressed = false;
        update();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void AlignmentWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed == true) {

        //scroll if necessary to continue selection
        scrollArea->ensureVisible(event->x(), event->y());

        int column = (event->x() / boxWidth);
        secondColumn = column;
        update();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void AlignmentWidget::selectColumns(int column) {
    //cout << "setCols(IN  " << firstColumnSelected << "," << secondColumn << ")\n";
    bool growright = true;

    if (secondColumn == -1) {
        if (column < firstColumnSelected)
            growright = false;
        secondColumn = firstColumnSelected;
    } else if (firstColumnSelected > secondColumn) {
        growright = false;
    }

    if (growright) {
        if (column < firstColumnSelected) {
            //reverse direction
            // c < f <= s ---> f<=>s then c < f
            firstColumnSelected = secondColumn;
            secondColumn = column;
        } else if (column > secondColumn) {
            // f <= s < c ---> f < s
            secondColumn = column;
        } else {
            // f (c) s    ---> f < c
            secondColumn = column;
        }
    } else {//growleft
        if (column < secondColumn) {
            // c < s < f ---> c < f
            secondColumn = column;
        } else if (column > firstColumnSelected) {
            //reverse direction
            // s < f < c ---> f<=>s then f < c
            firstColumnSelected = secondColumn;
            secondColumn = column;
        } else {
            // s (c) f   ---> c < f
            secondColumn = column;
        }
    }
    //cout << "setCols(OUT " << firstColumnSelected << "," << secondColumn << ")\n";
}

/**
 * This function will be called every time sequences are changed. So for example after
 * an alignment, after we add more sequences etc
 */
void AlignmentWidget::updateSizeHint()
{
    int lengthLongest = alignObjPtr->getLengthLongestSequence(firstSeq, lastSeq);
    int extraRow = getExtraRowNum();
    resize(lengthLongest * boxWidth,
          (numSequencesSoFar + extraRow) * boxSize);
}

/**
 * This function is an event handler. It repaints the widget whenever it needs
 * to be repainted. It is called automatically.
 * It also calculates the number of columns showing and colors in the alignment.
 * @param *event A resize event.
 */
void AlignmentWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect rect = event->rect();
    int cx = rect.x();
    int cy = rect.y();
    int cw = rect.width();
    int ch = rect.height();
    //(void) printf("paintEvent(%d,%d, %d,%d)\n", cx,cy, cw,ch);
    drawContents(&painter, cx, cy, cw, ch);
}

void AlignmentWidget::drawContents(QPainter *painter, int cx, int cy, int cw, int ch)
{
    painter->fillRect(cx, cy, cw, ch, QBrush(Qt::green));//white));
    painter->setBackgroundMode(Qt::OpaqueMode);
    
    // Set up the begin and end columns and rows. 
    int beginRow    = cy        / boxSize;
    int beginColumn = cx        / boxWidth;
    int endRow      = (cy+ch-1) / boxSize;
    int endColumn   = (cx+cw-1) / boxWidth;

    int selectedRes;
    conserveDisplay = true;
    int seqLength = 0;
    int extra = getExtraRowNum();
    
    //(void) printf("drawContents(x=%d,y=%d, w=%d,h=%d)(bw=%d,bh=%d) -> [br=%d,er=%d, bc=%d,ec=%d]\n",
                  //cx,cy, cw,ch, boxWidth, boxSize, beginRow,endRow, beginColumn,endColumn);

    if(firstSeq != 0 && alignmentSet && showAlignment)
    {
        for (int row = beginRow; row <= endRow; ++row) 
        {
            for (int column = beginColumn; column <= endColumn; ++column) 
            {
                //printf("draw[0](%d,%d)\n", row, column);

                if(firstSeq + row - extra >= 1)
                {
                    seqLength = alignObjPtr->getSeqLength(firstSeq + row - extra);
                }
                else
                {
                    seqLength = 0;
                }

                if (row > (extra - 1) && column < seqLength && 
                (((column >= firstColumnSelected) && (column <= secondColumn))
                    || ((column >= secondColumn) && (column <= firstColumnSelected) &&
                          secondColumn != -1))) // Selected ???? 
                {
                    selectedRes = (*alignment)[firstSeq + row - extra][column + 1];
                    
                    if(selectedRes >= selectedColoredRes->size())
                    {
                        selectedRes = selectedColoredRes->size() - 1; // '-'
                    }
                    if(selectedRes >= 0 && !(*selectedColoredRes)[selectedRes].isNull())
                    {
                        //printf("draw[1](%d,%d)\n", row, column);

                        painter->drawPixmap((column) * boxWidth, row * boxSize,
                                            (*selectedColoredRes)[selectedRes]);
                    }  
                }                    
                else
                {
                    // NOTE we need to have this check here. It was causing problems
                    // when removing gaps sometimes. Seems to be ok now!!! 
                    if(row < colorAlignment->getRowSize() && row >= 0 &&
                       column < colorAlignment->getColSize() && column >= 0)
                    {
                        //printf("draw[2](%d,%d) [%d,%d]\n", row, column,
                        //       colorAlignment->getRowSize(),
                        //       colorAlignment->getColSize()
                         //   );
                
                        if(!(*colorAlignment)[row][column]->isNull())
                        {
                            //printf("draw[3](%d,%d) [%d,%d]\n", row, column,
                            //       colorAlignment->getRowSize(),
                            //       colorAlignment->getColSize()
                            //    );
                            painter->drawPixmap((column) * boxWidth, 
                                                row * boxSize,*(*colorAlignment)[row][column]);
                        }
                    }
                }
            }
        }
    }
}

void AlignmentWidget::changeBoxSizeAndFontSize(int _boxWidth, int _boxSize, QFont* fontPtr)
{
    if (boxWidth != _boxWidth || boxSize != _boxSize) //nige: changed && to ||
    {       
        boxWidth = _boxWidth;
        boxSize  = _boxSize;
        displayFont = *fontPtr;
        int lengthLongest = alignObjPtr->getLengthLongestSequence(firstSeq, lastSeq);
        resize(lengthLongest * boxWidth, (numSequencesSoFar + 1) * boxSize);
        defaultPixMap = QPixmap(boxWidth, boxSize);
        defaultPixMap.fill(Qt::white);
        generatePixMaps();
        calculatePixMaps();   
        repaint();
    }
}

/**
 * This is a wrapper for the function in the clustalw code.
 */
char AlignmentWidget::getAminoAcidCode(int i)
{
    if(i < 0 || i >= clustalw::userParameters->getMaxAA())
    {
        return '-';
    }
    return clustalw::userParameters->getAminoAcidCode(i);
}

void AlignmentWidget::setUpWeakAndStrongGroups()
{
    strongGroup.resize(9);
    strongGroup[0] = string("STA");
    strongGroup[1] = string("NEQK");
    strongGroup[2] = string("NHQK");
    strongGroup[3] = string("NDEQ");
    strongGroup[4] = string("QHRK");
    strongGroup[5] = string("MILV");
    strongGroup[6] = string("MILF");
    strongGroup[7] = string("HY");
    strongGroup[8] = string("FYW");
   
    weakGroup.resize(11);
    weakGroup[0] = string("CSA");
    weakGroup[1] = string("ATV");
    weakGroup[2] = string("SAG");
    weakGroup[3] = string("STNK");
    weakGroup[4] = string("STPA");
    weakGroup[5] = string("SGND");
    weakGroup[6] = string("SNDEQK");
    weakGroup[7] = string("NDEQHK");
    weakGroup[8] = string("NEQHRK");
    weakGroup[9] = string("FVLIM");
    weakGroup[10] = string("HFY");
}

void AlignmentWidget::calcConserveInfo()
{
    if(firstSeq != 0 && lastSeq != 0)
    {
        int catident1[clustalw::NUMRES];
        int catident2[clustalw::NUMRES];
        int _fSeq, _lSeq;
        int _fRes, _lRes;   
        _fSeq = firstSeq;
        _lSeq = lastSeq;
        _fRes = 1;
        _lRes = alignObjPtr->getLengthLongestSequence(_fSeq, _lSeq);
   
        conservationInfo.clear();
        conservationInfo.resize(_lRes);
   
        int ident;
        char c;

        for(int i = _fRes; i <= _lRes; ++i)
        {
            conservationInfo[i - 1] = ' ';
            ident = 0;

            for(int j = 1; j <= strongGroup.size(); j++)
            {
                catident1[j - 1] = 0;
            }
            for(int j = 1; j <= weakGroup.size(); j++)
            {
                catident2[j - 1] = 0;
            }
               
            for(int j = _fSeq; j <= _lSeq; ++j)
            {
                if((i + _fRes - 1 < (*alignment)[j].size()) &&
                   (i + _fRes - 1 < (*alignment)[_fSeq].size()))
                {
                    /* nige,18-12-06: maxAA already decremented in UserParameters;
                     * introduce some locals for readability
                     */
                    //if(((*alignment)[_fSeq][i + _fRes - 1] >= 0) &&
                    //      ((*alignment)[_fSeq][i + _fRes - 1]
                    //      <= clustalw::userParameters->getMaxAA() - 2))
                    //    Fowler, my ar*e... introduce some locals
                    int thisRes = (*alignment)[_fSeq][i + _fRes - 1];
                    int maxAA = clustalw::userParameters->getMaxAA();

                    if (thisRes >= 0 && thisRes <= maxAA)
                    {
                        // Count how many are identical to the first sequence
                        /* nige,18-12-06 */
                        //if((*alignment)[_fSeq][i + _fRes - 1] ==
                        //    (*alignment)[j][i + _fRes - 1])
                        if(thisRes == (*alignment)[j][i + _fRes - 1])
                 /* nige,end */
                        {
                            ++ident;
                        }
                        // Count how many are in the same category.
                        for(int k = 1; k <= strongGroup.size(); k++)
                        {
                            for(int l = 0; (c = strongGroup[k - 1][l]); l++)
                            {
                                int resCode = (*alignment)[j][i + _fRes - 1];
                                if (getAminoAcidCode(resCode) == c)
                                {
                                    catident1[k - 1]++;
                                    break;
                                }
                            }
                        }
                        for(int k = 1; k <= weakGroup.size(); k++)
                        {
                            for(int l = 0; (c = weakGroup[k - 1][l]); l++)
                            {
                                int resCode = (*alignment)[j][i + _fRes - 1];
                                if (getAminoAcidCode(resCode) == c)
                                {
                                    catident2[k - 1]++;
                                    break;
                                }
                            }
                        }
                    }
                }   
            }
           
            // Now do the conservation part for each block.
            if(ident == _lSeq - _fSeq + 1)
            {
                conservationInfo[i - 1] = '*'; // All residues the same!
            }
            else if (!clustalw::userParameters->getDNAFlag())
            {
                for(int k = 1; k <= strongGroup.size(); k++)
                {
                    if (catident1[k - 1] == _lSeq - _fSeq + 1)
                    {
                        // All residues member of the same category
                        conservationInfo[i - 1] = ':';
                        break;
                    }
                }
                if(conservationInfo[i - 1] == ' ')
                {
                    for(int k = 1; k <= weakGroup.size(); k++)
                    {
                        if (catident2[k - 1] == _lSeq - _fSeq + 1)
                        {
                            // All residues member of the same category
                            conservationInfo[i - 1] = '.';
                            break;
                        }
                    }
                }
            }
        }
        conserveDisplay = true;
    }
}

void AlignmentWidget::setColorScheme(ColorParameters* colors)
{
    currentColorScheme = colors;
    blackAndWhite = false;
    generatePixMaps();
    if(alignmentSet && showAlignment)
    {
        calculatePixMaps();
    }
}   

void AlignmentWidget::setPrintoutColorScheme(ColorParameters colors)
{
    printoutColorScheme = colors;
}

void AlignmentWidget::setBlackAndWhite()
{
    blackAndWhite = true;
    if(alignmentSet && showAlignment)
    {
        generatePixMaps();
        calculatePixMaps();
    }
}

void AlignmentWidget::setBackGroundColoring(bool _backGroundColor)
{
    backGroundColor = _backGroundColor;
    if(alignmentSet && showAlignment)
    {
        generatePixMaps();
        calculatePixMaps();
    }
}

void AlignmentWidget::makeColorMask(clustalw::Array2D<QColor>* colMask,
                                    ColorParameters* colorScheme)
{
    QColor defaultColor = currentColorScheme->getDefaultColor();
   
    int lengthLongest = alignObjPtr->getLengthLongestSequence(firstSeq, lastSeq);
   
    colMask->clearArray();
    colMask->ResizeRect(numSequencesSoFar, lengthLongest);
    clustalw::Array2D<int> intColMask;
    makeColorMask(&intColMask, colorScheme);
   
    for(int i = 0; i < colMask->getRowSize(); i++)
    {
        for(int j = 0; j < colMask->getColSize(); j++)
        {
            if(i < intColMask.getRowSize() && j < intColMask.getColSize())
            {
                (*colMask)[i][j] = colorScheme->getColorFromNum(intColMask[i][j]);
            }
            else
            {
                (*colMask)[i][j] = defaultColor;
            }
        }
    }
}

void AlignmentWidget::makeColorMask(clustalw::Array2D<int>* colMask,
                                    ColorParameters* colorScheme)
{  
    int lengthLongest = alignObjPtr->getLengthLongestSequence(firstSeq, lastSeq);
   
    colMask->clearArray();
    colMask->ResizeRect(numSequencesSoFar, lengthLongest);
   
    for(int i = 0; i < colMask->getRowSize(); i++)
    {
        for(int j = 0; j < colMask->getColSize(); j++)
        {
            (*colMask)[i][j] = colorScheme->getDefaultIndex();
        }
    }
    int currentSeq;
    int code;
    QString consensus;
    consensus = initConsensus();
    std::string mystring = consensus.toStdString();
    QChar alignRes;
   
    for(int seq = 0; seq < numSequencesSoFar; seq++)
    {
        currentSeq = firstSeq + seq;
        for(int res = 0; res < lengthLongest; res++)
        {
            if(res + 1 < (*alignment)[currentSeq].size() && (lastSeq >= currentSeq))
            {
                code = (*alignment)[currentSeq][res + 1];
                alignRes = QChar(getAminoAcidCode(code));
            }
            else
            {
                alignRes = QChar('-');
            }
           
            (*colMask)[seq][res] = colorScheme->getResidueColorNum(alignRes,
                                                                   consensus[res]);
            if((*colMask)[seq][res] > colorScheme->getNumColors())
            {
                colorScheme->getResidueColorNum(alignRes, consensus[res]);
            }                                                      
        }
    } 
}

QString AlignmentWidget::initConsensus()
{
    QString consData;
    int numRes, seq, res, par, consTotal, i;
    QChar residue;
    int code;
    int numConsParams = currentColorScheme->getNumConsensusParams();
    int currentSeq;
   
    int lengthLongest = alignObjPtr->getLengthLongestSequence(firstSeq, lastSeq);
   
    std::vector<consensusParams>* consParams = currentColorScheme->getConPar();
   
    consData.resize(lengthLongest + 1);
    for(int j = 0; j < consData.size(); j++)
    {
        consData[j] = ' ';
    }
   
    for(res = 0; res < lengthLongest; res++)
    {
        for(par = 0; par < numConsParams; par++)
        {
            consTotal = numRes = 0;
           
            for(seq = 0; seq < numSequencesSoFar; seq++)
            {
                currentSeq = firstSeq + seq;
               
                if(res + 1 < (*alignment)[currentSeq].size() && (lastSeq >= currentSeq))
                {
                    code = (*alignment)[currentSeq][res + 1];
                    residue = QChar(getAminoAcidCode(code));
                    residue = residue.toLower();
        
                    if(residue.isLetter())
                    {
                        numRes++;
                    }
                    for(i = 0; i < (*consParams)[par].length; i++)
                    {
                        if(residue == (*consParams)[par].cutOffList[i])
                        {
                            consTotal++;
                        }
                    }
                }   
            }
            if(numRes != 0)
            {
                if(((consTotal * 100) / numRes) >= (*consParams)[par].cutOffPercent)
                {
                    consData[res] = (*consParams)[par].consensusName;
                }
            }
        }   
    }
    return consData;
}

void AlignmentWidget::clearRangeSelection()
{
    firstColumnSelected = -1;
    secondColumn = -1;
    update();
}

int AlignmentWidget::getFirstSelectedCol() {
    if (firstColumnSelected < secondColumn)
        return firstColumnSelected;
    else
        return secondColumn;
}

int AlignmentWidget::getSecondSelectedCol() {
    if (firstColumnSelected < secondColumn)
        return secondColumn;
    else
        return firstColumnSelected;
}

int AlignmentWidget::getExtraRowNum()
{
    int extraRow = 1;
    if(secStruct == 1)
    {
        extraRow = 2;
    }
    else
    {
        extraRow = 1;
    }
    return extraRow;
}

void AlignmentWidget::generatePixMaps()
{
    QFontMetrics fontMetrics(displayFont);
    // 0 has the default, colors then, selected = numColors+1,
    std::string aminoAcid = clustalw::userParameters->getAminoAcidCodes();
    int numDiffRes = aminoAcid.size();
    //code = userParameters->resIndex(userParameters->getAminoAcidCodes(), search[i]);
    int numColors = currentColorScheme->getNumColors();
    // number of rows = numColors + white
    int numRows = numColors + 1;
    defaultIndex = 0;
    coloredLetters.clearArray();   
    coloredLetters.ResizeRect(numRows, numDiffRes);
    QColor currentColor;
    // For each of the colors, create the PixMaps
    QColor penColor;
    QBrush boxesBrush;
    char currentChar;
    for(int i = 0; i < numRows; i++)
    {
        if(i == 0)
        {
            currentColor = currentColorScheme->getColorFromNum(i);
            penColor = QColor(Qt::black);
            boxesBrush = QBrush(currentColor.rgb());        
        }
        else if(i <= numColors)
        {
            currentColor = currentColorScheme->getColorFromNum(i);
            if(backGroundColor)
            {
                penColor = QColor(Qt::black);
                boxesBrush = QBrush(currentColor.rgb());
            }
            else
            {
                penColor = currentColor;
                boxesBrush = QColor(Qt::white);
            }           
        }
        else
        {
            currentColor = currentColorScheme->getColorFromNum(0); // default
            penColor = QColor(Qt::black);
            boxesBrush = QBrush(currentColor.rgb());           
        }
        for(int j = 0; j < aminoAcid.size(); j++)
        {
            // get a pen color and a background color!!!!!
            coloredLetters[i][j] = QPixmap(boxWidth, boxSize);
            QPainter paintChar(&coloredLetters[i][j]);           
            coloredLetters[i][j].fill(boxesBrush.color());
           
            currentChar = aminoAcid[j];
           
            paintChar.setFont(displayFont);
            paintChar.setPen(QPen(penColor));
            paintChar.drawText((boxWidth / 2) -
                               fontMetrics.width(currentChar)/2,
                               offSetFromTop + fontMetrics.ascent(),
                               QString(QChar(currentChar)));         
        }
    }
   
    QBrush selectedBrush, exceptionalBrush, lowScoreBrush;
    selectedBrush = QBrush(selectedColor->rgb());
    lowScoreBrush = QBrush(slateGrey->rgb());
    exceptionalBrush = QBrush(lightGrey->rgb());
   
    int length = aminoAcid.size();
    selectedColoredRes->clear();
    selectedColoredRes->resize(length);
    lowScoreColoredRes->clear();
    lowScoreColoredRes->resize(length);
    exceptionalColoredRes->clear();
    exceptionalColoredRes->resize(length);
       
    for(int i = 0; i < length; i++)
    {
        currentChar = aminoAcid[i];
       
        (*selectedColoredRes)[i] = QPixmap(boxWidth, boxSize);
        (*selectedColoredRes)[i].fill(selectedBrush.color());
        QPainter paintChar(&(*selectedColoredRes)[i]);
        paintChar.setFont(displayFont);
        paintChar.drawText((boxWidth / 2) -
                           fontMetrics.width(currentChar)/2,
                           offSetFromTop + fontMetrics.ascent(),
                           QString(QChar(currentChar)));

        (*lowScoreColoredRes)[i] = QPixmap(boxWidth, boxSize);
        (*lowScoreColoredRes)[i].fill(lowScoreBrush.color());
        QPainter paintChar2(&(*lowScoreColoredRes)[i]);
        paintChar2.setFont(displayFont);
        paintChar2.setPen(QPen(QColor(Qt::white)));  //nige
        paintChar2.drawText((boxWidth / 2) -
                           fontMetrics.width(currentChar)/2,
                           offSetFromTop + fontMetrics.ascent(),
                           QString(QChar(currentChar)));
                          
        (*exceptionalColoredRes)[i] = QPixmap(boxWidth, boxSize);
        (*exceptionalColoredRes)[i].fill(exceptionalBrush.color());
        QPainter paintChar3(&(*exceptionalColoredRes)[i]);
        paintChar3.setFont(displayFont);
        paintChar3.setPen(QPen(QColor(Qt::white)));  //nige
        paintChar3.drawText((boxWidth / 2) -
                           fontMetrics.width(currentChar)/2,
                           offSetFromTop + fontMetrics.ascent(),
                           QString(QChar(currentChar)));                                     
    }
   
    QBrush conservationBrush;
    conservationBrush = QBrush(backGroundGrey->rgb());
    int numConserveChars = conservationChars.size();
    conservationPixmaps->resize(numConserveChars);
    displayFont.setBold(true);
               
    for(int i = 0; i < numConserveChars; i++)
    {
        (*conservationPixmaps)[i] = QPixmap(boxWidth, boxSize);       
        (*conservationPixmaps)[i].fill(conservationBrush.color());
        QPainter paintChar(&(*conservationPixmaps)[i]);
        currentChar = conservationChars[i];
        paintChar.setFont(displayFont);       
        paintChar.drawText((boxWidth / 2) -
            fontMetrics.width(currentChar)/2,
            offSetFromTop + fontMetrics.ascent(),
            QString(QChar(currentChar)));
    }          
}

clustalw::Array2D<QColor> AlignmentWidget::getPrintoutColorMask()
{
    clustalw::Array2D<QColor> printoutColorMask;
    makeColorMask(&printoutColorMask, &printoutColorScheme);
    return printoutColorMask;
}
