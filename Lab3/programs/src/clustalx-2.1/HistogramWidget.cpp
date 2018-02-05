/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 * 
 * Changes:
 *
 * 16-01-07,Nigel Brown(EMBL): added drawContents() called by paintEvent().
 *
 * 31-01-07,Nigel Brown(EMBL): changed && to || in updateBoxSize(). Changed
 * drawContents() to paint histogram 'bars' before writing ruler text as this
 * was effectively clipped by the histogram bar on the next pass.
 *
 * 06-02-07,Nigel Brown(EMBL): darker histogram bars for Mac.
 *
 * 20-02-07,Nigel Brown(EMBL): constructor and updateBoxSize() parameters
 * reordered.
 *
 * 12-4-07, Mark Larkin : Changed destructor. We dont need to delete QObjects. 
 *
 * 16-04-07,Nigel Brown(EMBL): removed destructor; arrayOfHeights not
 * allocated here.
 ****************************************************************************/

#include "HistogramWidget.h"
#include <QtGui>
#include <QImage>
//#include <QPixmap>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>
#include <cmath>
// next two only needed for cerr/debugging
//#include <iostream>
//using namespace std;

/**
 * This is the constructor for a HistogramWidget.
 * @param histogramValues[] The array of histogram values to be displayed.
 * @param length The length of the alignment.
 * @param boxSize The width of the histogram bars.
 * @param *parent The parent of this widget. This parameter is optional.
 */
HistogramWidget::HistogramWidget(int _boxWidth, int _boxSize, QWidget *parent)
    : QWidget(parent)
{
    /*
     * Set up the initial values.
     */
    alignLength = 0;
    arrayOfHeights = 0; 
    isSet = false;
    boxSize = _boxSize;
    boxWidth = _boxWidth;
    heightOfBox = 1;
    //histogramImage = 0;
    
    displayStartPos = 0; 
    extraWhiteSpace = 5;
    lengthSmallBar = 1;
    lengthLargeBar = 3;
    setBackgroundRole(QPalette::Base);
    
    /*
     * Create a color to display the ruler in.
     */
    rulerColor = new QColor;
    rulerColor->setRgb(192,215,233);

    offSetFromTop = 10;

    heightAllowed = 50;
    
    updateSizeHint();
}

/**
 * This function overrides the parents sizeHint function. It gives an idea of 
 * the size this widget needs. 
 */
QSize HistogramWidget::sizeHint() const
{
    //(void) printf("HistogramWidget::sizeHint(w=%d,h=%d) <- [ha=%d, hob=%d, ews=%d]\n",
    //              alignLength * boxWidth, (heightAllowed * heightOfBox) + extraWhiteSpace,
    //              heightAllowed, heightOfBox, extraWhiteSpace); 
    return QSize(alignLength * boxWidth, (heightAllowed * heightOfBox) + extraWhiteSpace);
}

/**
 * This function will be called every time the alignment may have changed size.
 */
void HistogramWidget::updateSizeHint()
{
    resize(alignLength * boxWidth, (heightAllowed * heightOfBox) + extraWhiteSpace);
    update();
}

void HistogramWidget::addHistogramInfo(std::vector<int>* histogramValues)
{
    arrayOfHeights = histogramValues;
    alignLength = arrayOfHeights->size();

    /* bug 143: we re-use these scores for the column quality, i.e.
       don't reduce to 99 HERE
       99 seems to be needed, otherwise histogram display is messed up
              
       // Check each element is <= 100
       for(int i = 0; i < alignLength; i++)
       {
       if((*arrayOfHeights)[i] >= 100)
       {
       (*arrayOfHeights)[i] = 99; // Make sure 99 is the maximum value.    
       }
       }
    */
    isSet = true;
    updateSizeHint();    
    calculateHistogramBars();    
}

void HistogramWidget::clearHistogram()
{
    isSet = false;
    arrayOfHeights = 0;
    alignLength = 0;
            
    updateSizeHint();    
}

/**
 * This function is an event handler. It is called automatically when the widget 
 * needs to be repainted.
 * It draws the histogram and the ruler from the appropriate start and end positions.
 * @param *event A paint event object.
 */
void HistogramWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect rect = event->rect();
    int cx = rect.x();
    int cy = rect.y();
    int cw = rect.width();
    int ch = rect.height();
    //(void) printf("HistogramWidget::paintEvent(%d,%d, %d,%d)\n", cx,cy, cw,ch);
    drawContents(&painter, cx, cy, cw, ch);
}

void HistogramWidget::drawContents(QPainter *painter, int cx, int cy, int cw, int ch)
{
    painter->fillRect(cx, cy, cw, ch, QBrush(Qt::blue));
    painter->setBackgroundMode(Qt::OpaqueMode);
    //painter->setBackground(QBrush(Qt::white));

    // Andreas Wilm (UCD):ff this is too small especially on high
    // resolutions. Calculate dynamically
    rulerFont.setPixelSize(8);
    painter->setFont(rulerFont);    
    
    QFontMetrics fontMetrics(rulerFont);    
    
    int offSetForRuler = fontMetrics.height() - 1;
       
    // Set up the begin and end columns
    int beginColumn = cx        / boxWidth;
    int endColumn   = (cx+cw-1) / boxWidth;
    
    int index = 0;
    int rulerNum;
    int xPos;
    int yPos = 0;        

    //(void) printf("HistogramWidget::drawContents(x=%d,y=%d, w=%d,h=%d)(bw=%d,bh=%d) -> [bc=%d,ec=%d]\n",
    //              cx,cy, cw,ch, boxWidth,boxSize, beginColumn,endColumn);

    if(isSet && arrayOfHeights)
    {
        //31-01-07,nige: the histogram vertical stripes must be painted first
        //otherwise the current stripe may paint over the last ruler text.
        for (int column = beginColumn; column <= endColumn; ++column) 
        {
            if(column >= 0 && column < arrayOfHeights->size())
            {
                index = (*arrayOfHeights)[column];

                // bug 143: dynamically set to 99 here (see above)
                //          otherwise histogram is messed up
                if (index>=100)
                    index=99;

                if(index >= 0 && index < histogramBars.size() &&
                   !histogramBars[index].isNull())
                {
                    painter->drawPixmap((column) * boxWidth, 0, histogramBars[index]);
                }
            }
        }
        for (int column = beginColumn; column <= endColumn; ++column) 
        {
            if(column >= 0 && column < arrayOfHeights->size())
            {
                rulerNum = column + 1;
                if((rulerNum % 10) == 0) 
                {   
                    painter->drawLine(column * boxWidth + (boxWidth / 2), offSetForRuler, 
                                      column * boxWidth + (boxWidth / 2), 
                                      offSetForRuler + lengthLargeBar);
                
                    xPos = column * boxWidth + (boxWidth / 2) - 
                            (fontMetrics.width(QString::number(rulerNum))) / 2;
                    
                    if(rulerNum >= 10000)
                    {
                        painter->drawText(xPos, yPos, 7 * boxWidth, boxSize,
                                          Qt::TextSingleLine, QString::number(rulerNum));
                    }
                    else if(rulerNum >= 1000 )
                    {
                        painter->drawText(xPos, yPos, 6 * boxWidth, boxSize,
                                          Qt::TextSingleLine, QString::number(rulerNum));
                    }
                    else if(rulerNum >= 100 )
                    {
                        painter->drawText(xPos, yPos, 5 * boxWidth, boxSize,
                                          Qt::TextSingleLine, QString::number(rulerNum));
                    }
                    else if(rulerNum >= 10 )
                    {
                        painter->drawText(xPos, yPos, 3 * boxWidth, boxSize,
                                          Qt::TextSingleLine, QString::number(rulerNum));
                    }
                    else
                    {
                        painter->drawText(xPos, yPos, boxWidth, boxSize,
                                          Qt::TextSingleLine, QString::number(rulerNum));
                    }
                }
                else
                {
                    if(rulerNum == 1)
                    {
                        painter->drawText((boxWidth / 2) - 
                                          (fontMetrics.width(QString::number(0))) / 2, yPos, boxWidth,
                                          boxSize, Qt::TextSingleLine, QString::number(rulerNum));
                    }
                    painter->drawLine(column * boxWidth + (boxWidth / 2), offSetForRuler, 
                                      column * boxWidth + (boxWidth / 2), 
                                      offSetForRuler + lengthSmallBar);
                }                
            }            
        }
    }
}

void HistogramWidget::updateBoxSize(int _boxWidth, int _boxSize)
{
    if (boxWidth != _boxWidth || boxSize != _boxSize)
    {
        boxWidth = _boxWidth;
        boxSize  = _boxSize;
        resize(alignLength * boxWidth, (heightAllowed * heightOfBox) + extraWhiteSpace);
        calculateHistogramBars();
        update();
    }
}

void HistogramWidget::calculateHistogramBars()
{
    histogramBars.clear();
    histogramBars.resize(100);
    int heightOfBar;
    for(int i = 0; i < histogramBars.size(); i++)
    {
        QPoint origin(0, height());
        histogramBars[i] = QPixmap(boxWidth, height());
        heightOfBar = (i * heightOfBox) / 3;
        QPainter painter(&histogramBars[i]);
        painter.fillRect(0, 0, boxWidth, height(), QBrush(Qt::white));    
        painter.translate(origin);        
#if OS_MAC
        painter.fillRect(0, 0, boxWidth, -heightOfBar, QBrush(Qt::gray));
#else
        painter.fillRect(0, 0, boxWidth, -heightOfBar, QBrush(Qt::lightGray));
#endif
        painter.fillRect(0, -height(), boxWidth, 11, QBrush(rulerColor->rgb()));
    }
}

/* TEST
void HistogramWidget::mousePressEvent(QMouseEvent *event)  //nige
{
    if (event->button() == Qt::LeftButton)
    {
        QSize size = sizeHint();
        printf("SIZEHINT: w=%d, h=%d\n", size.width(), size.height());
    }
    else
    {
        QWidget::mousePressEvent(event);
    }
}
TEST end */
