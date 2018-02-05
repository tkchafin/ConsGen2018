/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 *
 * Changes:
 *
 * 16-01-07,Nigel Brown(EMBL): added drawContents() called by paintEvent().
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
 * getSecondSelectedCol() into AlignmentWidget.cpp.
 *
 * 09-05-07,Nigel Brown(EMBL): Added 'mousePressed', mouseMoveEvent()
 * and mouseReleaseEvent(); Changed 'rowSelected' to 'firstRow' and
 * added 'secondRow' store row range information.
 *
 * 09-05-07,Nigel Brown(EMBL): Added 'scrollArea' and registerScrollArea()
 * allowing the instance to know its own containing QScrollArea (This is not
 * the same as parentWidget()).
 ****************************************************************************/

#ifndef ALIGNMENTWIDGET_H
#define ALIGNMENTWIDGET_H

#include <QFont>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>
#include <QVector>
#include <vector>
#include <string>
#include "clustalW/alignment/Alignment.h"
#include "ColorParameters.h"
#include <QKeyEvent>
#include <QPixmap>
class QMouseEvent;
class QPaintEvent;
class QMessageBox;
class QKeyEvent;
class QScrollArea; //- nige

/**
 * The AlignmentWidget is used to display a multiple alignment. It takes sequences
 * in the form of QString objects. The conservation information is displayed, 
 * if it is available. 
 */
class AlignmentWidget : public QWidget
{
    Q_OBJECT

public:
    AlignmentWidget(int boxWidth, int boxSize, QFont* font, QWidget *parent = 0);
    QSize sizeHint() const;
    void changeBoxSizeAndFontSize(int _boxWidth, int _boxSize, QFont* fontPtr);
    void updateSizeHint();
    void setAlignment(const clustalw::SeqArray* align, 
                      clustalw::Alignment* _alignObjPtr);
    void updateDisplay(int firstSeq, int lastSeq, clustalw::Array2D<int>* exRes, int nhead,
                       QString secStruct);
    void updateLowScoreSeg();
    void showExceptionalResidues(bool show);
    bool getShowExceptionalResidues(){return showExceptionalRes;}
    void showLowScoreSeg(bool show, clustalw::Array2D<int>* _lowScoreRes);
    bool getShowLowScoreSeg(){return showLowScoreRes;}
    void setColorScheme(ColorParameters* colors);
    void setPrintoutColorScheme(ColorParameters colors);
    void setBlackAndWhite();
    void setBackGroundColoring(bool backgroundColor); 
    bool getBackGroundColor(){return backGroundColor;}
    void clearRangeSelection();
    QString getConservationInfo(){return QString(conservationInfo.c_str());}
    QString getSecStructInfo(){return secStructInfo;} 
    clustalw::Array2D<QColor> getPrintoutColorMask();
    ColorParameters* getPtrToCurrentColorScheme(){return currentColorScheme;}
    int getFirstSelectedCol();
    int getSecondSelectedCol();
    void registerScrollArea(QScrollArea *s) { scrollArea = s; }; //- nige

public slots:

signals:
    void characterSelected(const QString &character);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);     
    void paintEvent(QPaintEvent *event);
    void selectColumns(int column); //nige
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch); //nige

private:
    char getAminoAcidCode(int i);
    void setUpWeakAndStrongGroups();
    void calcConserveInfo();
    void calculatePixMaps();
    void makeColorMask(clustalw::Array2D<QColor>* colMask, 
                        ColorParameters* colorScheme);
    void makeColorMask(clustalw::Array2D<int>* colMask, 
                        ColorParameters* colorScheme);                        
    void generatePixMaps();
    void generateSecStructurePixmaps();
    QString initConsensus();
    int getExtraRowNum();
    
    int firstSeq, lastSeq;
    QFont displayFont;
    int firstColumnSelected;
    int secondColumn;
    int boxSize;
    int boxWidth;
    int pixelsize;
    int alignLength;
    int numSequencesSoFar;
    int offSetFromTop;
    int displayStartPos;

    QPixmap defaultPixMap;
    QPixmap defaultGreyPixMap;
    QString conserveInfo;

    QColor *selectedColor; // This will stay here!!!!! Others will go!!!
    
    QColor *lightGrey;
    QColor *slateGrey;
    QColor *backGroundGrey;

    bool conserveDisplay;
    bool mousePressed;
    const clustalw::SeqArray* alignment;
    clustalw::Alignment* alignObjPtr;
    bool alignmentSet;
    bool showAlignment;
    std::vector<std::string> strongGroup; 
    std::vector<std::string> weakGroup;
    std::string conservationInfo;    
    clustalw::Array2D<int>* exceptionalRes;
    bool exResLoaded;
    bool showExceptionalRes;
    std::string conservationChars;
    bool lowScoreResLoaded;
    bool showLowScoreRes;
    
    int colorScheme;
    
    clustalw::Array2D<int>* lowScoreRes;
    clustalw::Array2D<QPixmap*>* colorAlignment;
    clustalw::Array2D<QColor> colorMask;
    clustalw::Array2D<int> colMask;
    QColor defaultColor;
    clustalw::Array2D<QPixmap> coloredLetters;    
    std::vector<QPixmap>* selectedColoredRes;
    std::vector<QPixmap>* lowScoreColoredRes;
    std::vector<QPixmap>* exceptionalColoredRes;
    std::vector<QPixmap>* conservationPixmaps;
    std::vector<QPixmap>* secStructPixmaps;
    // Color scheme stuff!!!
    ColorParameters* currentColorScheme;
    ColorParameters printoutColorScheme;
    
    bool blackAndWhite;
    bool backGroundColor;
    int secStruct;
    QString secStructInfo;
    int defaultIndex;      

    QScrollArea *scrollArea; //-nige
};

#endif
