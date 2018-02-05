/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie 
 *
 * Changes:
 *
 * 16-01-07,Nigel Brown(EMBL): added drawContents() called by paintEvent().
 *
 * 20-02-07,Nigel Brown(EMBL): constructor and updateBoxSize() parameters
 * reordered.
 *
 * 12-4-07, Mark Larkin : Changed destructor. We dont need to delete QObjects. 
 *
 * 16-04-07,Nigel Brown(EMBL): removed destructor; arrayOfHeights not
 * allocated here.
 ****************************************************************************/

#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QFont>
class QImage;
//class QPixmap;
#include <QPixmap> // Mark 20-4-2007
class QPoint;
class QSize;
class QString;
class QWidget;
#include <vector>
#include <QLabel>
/**
 * The HistogramWidget class displays a graphical histogram. It also displays a ruler.
 */
class HistogramWidget : public QWidget
{
    Q_OBJECT

public:
    HistogramWidget(int boxWidth, int boxSize, QWidget *parent = 0);
    QSize sizeHint() const;
    void addHistogramInfo(std::vector<int>* histogramValues);
    void clearHistogram();
    const std::vector<int>* getColumnScores(){return arrayOfHeights;}
public slots:
    void updateBoxSize(int boxWidth, int boxSize);
    void updateSizeHint();
signals:
    void characterSelected(const QString &character);

protected:
    void paintEvent(QPaintEvent *event);
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch); //nige
    //void mousePressEvent(QMouseEvent *event);  //nige test

private:
    void calculateHistogramBars();
    QFont rulerFont;
    int alignLength; 
    int heightAllowed;
    int boxSize;
    int boxWidth;
    float heightOfBox;
    std::vector<int>* arrayOfHeights;
    QVector<QPixmap> histogramBars;  
    int displayStartPos;
    int offSetFromTop;
    int extraWhiteSpace;
    QColor *rulerColor;
    int lengthSmallBar;
    int lengthLargeBar;
    bool isSet;
};

#endif
