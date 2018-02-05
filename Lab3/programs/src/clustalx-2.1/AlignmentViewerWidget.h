/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 * This widget contains an AlignmentWidget, HistogramWidget and a SeqNamesWidget.
 * This is the main widget for displaying the alignment 
 *
 * Changes:
 *
 * 16-01-07,Nigel Brown(EMBL): Added Q_MOS_MAC preprocessor checks for Mac OS
 * X port, which uses AlignmentWidgetScroll in place of AlignmentWidget, and
 * HistogramWidgetScroll in place of HistogramWidget. This is to work around a
 * scrolling limitation in Qt4 QScrollArea. THESE CHANGES SHOULD BE REMOVED
 * WHEN QT4 QSCROLLAREA IS FIXED! *
 * Mark: Jan 16th 2007
 * I have added an access function hasSeqNameWidgetFocus(). 
 *
 * 31-01-07,Nigel Brown(EMBL): added defaultFontSize member.
 *
 * 12-4-07, Mark Larkin : Changed destructor. We dont need to delete QObjects.
 *
 * 04-05-07,Nigel Brown(EMBL): renamed hasSeqNameWidgetFocus() to hasFocus().
 *
 * 27-06-07,Nigel Brown(EMBL): For screen layout balancing: added
 * recvNumSequencesChanged() and balanceNumSequences() slots, and
 * numSequencesChanged() signal.
 *
 * 20-7-07, Mark, I added in the resizeEvent and moveNamesScrollBarToRight function.
 ****************************************************************************/

#ifndef ALIGNMENTVIEWERWIDGET_H
#define ALIGNMENTVIEWERWIDGET_H

//AW not needed anymore #include "compile.h"  //nige

#include <vector>
#include <string>
#include "clustalW/alignment/Alignment.h"
#include "AlignmentWidget.h"
#include "HistogramWidget.h"
#include "SeqNameWidget.h"
#include "ColorParameters.h"
#include "PostscriptFileParams.h"
class QFont;
class QScrollArea;
class QScrollBar;
class QGroupBox;
class QGridLayout;
class QVBoxLayout;


/** 
 * This class is an extension of the widget QMainWindow. It contains an 
 * alignment widget, a histogram widget and a sequence name widget. 
 * It is currently used as an alignment viewer, but it will be possible to extend its
 * functionality to an alignment editor.
 */
class AlignmentViewerWidget : public QWidget
{
    Q_OBJECT

public:
    AlignmentViewerWidget(QWidget *parent = 0);
    ~AlignmentViewerWidget();
    void setAlignmentView(clustalw::Alignment* alignPtr, int fSeq, int lSeq);
    void updateView(int fSeq, int lSeq);
    void updateView();
    void updateHistogram(int fSeq, int lSeq);
    void showExceptionalRes(bool show);
    void showLowScoringSegments(bool show, clustalw::Array2D<int>* _lowScoreRes);
    int getFirstSeq(){return firstSeq;} 
    int getLastSeq(){return lastSeq;}
    int getLengthLongestSeqInRange();
    QScrollBar* getAlignmentScrollBar();
    void setColorScheme(ColorParameters* colors);
    void setPrintoutColorScheme(ColorParameters colors);
    void setBlackAndWhite();
    void setBackGroundColoring(bool backgroundColor);
    void clearSequenceSelection();
    void clearRangeSelection();
    void selectAllSequences();
    bool outputColumnScores();
    void saveSequences(QString inputFileName, int type);
    int getNumberOfSeqsSelected();
    const std::vector<int>* getSelectedRows();
    void setProfileNum(int prfNum){profileNum = prfNum;}
    void writeToPSFile(PostscriptFileParams postscriptParams);
    bool seqsLoaded();
    ColorParameters* getPtrToCurrentColorScheme()
                    {return alignWidget->getPtrToCurrentColorScheme();} 
    int getFirstSelectedCol(){return alignWidget->getFirstSelectedCol();}
    int getSecondSelectedCol(){return alignWidget->getSecondSelectedCol();} 
    bool hasFocus(); //- nige

protected:
    //void paintEvent(QPaintEvent *event);
    void resizeEvent ( QResizeEvent * event );
/* 
 * slots are functions, they can be linked to signals 
 */
public slots:
    void updateFontAndBoxes(const QString& font);
    void recvNumSequencesChanged(int count); //nige

signals:
    void numSequencesChanged(int count); //nige

private:
    void createGridBox();
    void sendInfoToAlignWidget();
    void sendInfoToNameWidget();
    int makeGapPenaltyData();
    int makeSecStructData();
    void moveNamesScrollBarToRight();
    int firstSeq;
    int lastSeq;
    QGroupBox *gridGroupBox;
    QGridLayout *layoutGridBox;

    /* 
     * Widgets to display the alignent and the histogram.
     */
    AlignmentWidget *alignWidget;
    QScrollArea *alignmentScroll;
    HistogramWidget *histWidget;
    QScrollArea *conservationScroll;

    QScrollArea *namesScroll;
    QVBoxLayout *mainlayout;

    /*
     * Widget to display the sequence names
     */
    SeqNameWidget *seqNames;

    int defaultFontSize; //nige
    int defaultBoxSize;
    int defaultBoxWidth;
    int alignLength;

    /* 
     * The main font for the application 
     */
    QFont* displayFont;
    clustalw::Alignment* alignmentToDisplay;
    
    std::vector<int> histogram; // vector of column heights
    clustalw::Array2D<int>* exRes; // Exceptional residues.
    clustalw::Array2D<int>* lowScoreRes; // low scoring residues.
    
    int profileNum;
    QString secStructData;
    QString ssName;
    int nHead;
};

#endif
