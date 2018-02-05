/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 * 
 * Changes:
 *
 * 20-02-07,Nigel Brown(EMBL): made getExtraRowNum() a const member so it can
 * be called inside sizeHint(); added eventFilter() to catch containing
 * viewport resize events; added single setMaxNameLength() member to compute
 * maximum typeset text width; added computeSize() member to determine final
 * widget size from text size or containing viewport size; added
 * horizontalMargin member: Widget now fills viewport and resizes neatly.
 *
 * 11-04-07,Nigel Brown(EMBL): Added new member 'ctrlPressed'.
 *
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. 
 *
 * 25-04-07,Nigel Brown(EMBL): removed 'shiftPressed', 'ctrlPressed' members
 * and the keyPressEvent() and keyReleaseEvent() handlers. These are now
 * handled by the external KeyController class.
 *
 * 09-05-07,Nigel Brown(EMBL): added 'mousePressed'. Renamed
 * rowSelected' to 'firstRow' and added 'secondRow'. 'scrollArea' and
 * registerScrollArea() allowing the instance to know its own
 * containing QScrollArea (This is not the same as parentWidget()).
 *
 * 27-06-07,Nigel Brown(EMBL): For screen layout balancing: renamed
 * numSequencesSoFar as numSequences, added numSequences2, which records the
 * number of sequences in the *other* profile window; Added setNumSequences2().
 ****************************************************************************/

#ifndef SEQNAMEWIDGET_H
#define SEQNAMEWIDGET_H

#include <QFont>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>
#include <QVector>
#include <vector>
#include "clustalW/alignment/Alignment.h"
class QMouseEvent;
class QPaintEvent;
class QMessageBox;
class QScrollArea; //- nige

/**
 * This class is used to display the list of the sequence names. It has funcitons 
 * that deal with the selection of multiple names at a time.
 */
class SeqNameWidget : public QWidget
{
    Q_OBJECT

public:
    SeqNameWidget(int boxSize, QFont* font, QWidget *parent = 0);
    void setNumOfSequences(int num);
    void addSequenceName(QString *seq);
    void changeBoxSizeAndFontSize(int boxSize, QFont* fontPtr);
    void updateSizeHint();
    void setNames(clustalw::Alignment* align);
    void updateDisplay(int fSeq, int lSeq, int nhead, QString ssName);
    void clearSequenceSelection();
    void selectAllSequences();
    int getNumberOfSeqsSelected();
    const std::vector<int>* getSelectedRows(){return &selectedRows;};
    int getExtraRowNum() const; //-nige
    QString getNameSSHeader(){return secStructName;}
    //bool widgetHasFocus 
    void registerScrollArea(QScrollArea *s) { scrollArea = s; }; //- nige
    void setNumSequences2(int count); //nige
    
public slots:

signals:

protected:
    void mousePressEvent( QMouseEvent *event);  
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);     
    void paintEvent( QPaintEvent *event);
    int setMaxNameLength();
    bool eventFilter(QObject *o, QEvent *e); //- nige
    QSize computeSize() const; //- nige

private:
    int firstSeq, lastSeq;
    int nHead;
    QFont displayFont;
    int numRowsShowing;
    bool mousePressed; //- nige
    int firstRow; //- nige
    int secondRow; //- nige
    QSize sizeHint() const;
    int boxSize;
    int pixelsize;
    int alignLength;
    int numSequences;
    int numSequences2;
    int offSetFromTop;
    int horizontalMargin;  //margin for identifiers, nige
    clustalw::Alignment* alignment;
    QColor* selectedColor;
    QColor *backGroundGrey;
    std::vector<int> selectedRows; 
    int maxNameLength;
    bool isSet;
    bool showNames;
    QString secStructName;
    QSize viewSize; //containing viewport size, nige
    QScrollArea *scrollArea; //-nige
};

#endif
