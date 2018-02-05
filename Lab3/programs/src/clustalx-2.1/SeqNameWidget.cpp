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
 * horizontalMargin member. Visual effect of all this is to extend widget out
 * to boundaries of viewport, allowing justification, colouring of whole
 * region, and highlighting out to full width of viewport.
 *
 * 12-03-07,Nigel Brown(EMBL): changed background colour to very pale grey.
 *
 * 11-04-07,Nigel Brown(EMBL): Modified mousePressEvent(), keyPressEvent() and
 * keyReleaseEvent() to extended sequence selection to allow [shift]-select
 * for a block of sequences and [ctrl]-select to deselect a single sequence
 * therein. Added new member 'ctrlPressed'. Initialised 'ctrlPressed' and
 * 'rowSelected' in constructor. Note: on the Mac [ctrl] is achieved using the
 * "Option" key (handled transparently by Qt). Behaviour on left-click is:
 *     select:              select this row.
 *     [shift]-select:      set block from last selected row to this row.
 *     [ctrl]-select:       deselect this row.
 * The member 'rowSelected' stores the last selected row. It is reset to an
 * invalid value on [ctrl]-select.
 *
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. 
 *
 * 25-04-07,Nigel Brown(EMBL): removed 'shiftPressed', 'ctrlPressed' members
 * and the keyPressEvent() and keyReleaseEvent() handlers. These are now
 * handled by the external KeyController class.
 *
 * 02-05-07,Nigel Brown(EMBL): Changed mousePressEvent() behaviour to add:
 *     [ctrl]-select:       *toggle* this row.
 *
 * 09-05-07,Nigel Brown(EMBL): Added drag and [shift]-drag selection
 * of row indentifiers via new member 'mousePressed', mouseMoveEvent()
 * and mouseReleaseEvent(); Changed 'rowSelected' to 'firstRow' and
 * added 'secondRow' store row range information.
 *
 * 09-05-07,Nigel Brown(EMBL): Added ensureVisible() call on new
 * 'scrollArea' member inside mouseMoveEvent() to scroll when
 * selection extends beyond visible.
 *
 * 15-05-07,Nigel Brown(EMBL): mousePressEvent(), mouseMoveEvent() now test
 * numSequencesSoFar before handling event.
 *
 * 27-06-07,Nigel Brown(EMBL): For screen layout balancing: renamed
 * numSequencesSoFar as numSequences, added numSequences2, which records the
 * number of sequences in the *other* profile window; Added setNumSequences2()
 * and changed computeSize() to compute height based on the *larger* of
 * numSequences and numSequences2.
 * 
 ****************************************************************************/

#include <QtGui>
#include "SeqNameWidget.h"
#include "KeyController.h" //- nige
#include "clustalW/alignment/Alignment.h"

const int    DEFAULT_MAX_NAME_LENGTH = 15;
const QColor DEFAULT_BACKGROUND_COLOUR(0xF9,0xF9,0xF9);

/**
 * This is the constructor for a SeqNameWidget. It sets up all the initial parameters.
 * @param num The number of sequence names to be displayed.
 * @param boxsize The size of the box to contain the names.
 * @param *parent The parent widget. This is optional.
 */
SeqNameWidget::SeqNameWidget(int boxsize, QFont* font, QWidget *parent)
    : QWidget(parent),
      firstSeq(0), 
      lastSeq(0),
      nHead(0),
      boxSize(boxsize)
{
    /* 
     * Set up initial values 
     */
    isSet = false;
    showNames = false;
    mousePressed = false; //- nige
    setMouseTracking(true);
    offSetFromTop = 2;  //it is 2 in AlignmentWidget
    horizontalMargin = 5; //- nige
    firstRow = -1; //- nige
    secondRow = -1; //- nige
    alignment = 0;
    secStructName = "";
    numSequences = 0;
    numSequences2 = 0; //nige

    viewSize = QSize(0,0);
    /* 
     * Set the font 
     */
    displayFont = *font;
    setMaxNameLength();

    /* 
     * Set up the colors to be used in displaying the alignment 
     */
    selectedColor = new QColor;
    selectedColor->setRgb(180,228,254);
    backGroundGrey = new QColor;
    backGroundGrey->setRgb(221,221,221);
    /* 
     * Set the focus policy to allow the widget to recieve keyboard events 
     */
    setFocusPolicy( Qt::StrongFocus ); 
}

void SeqNameWidget::setNames(clustalw::Alignment* align)
{
    alignment = align;
    isSet = true;
    if(align != 0)
    {
        numSequences = align->getNumSeqs();
        selectedRows.clear();
    }
    selectedRows.resize(numSequences);
    updateSizeHint();  //- may not be necessary now, 20-02-07,nige
}

void SeqNameWidget::updateDisplay(int fSeq, int lSeq, int nhead,  QString ssName)
{
    if(fSeq != 0 && lSeq != 0)
    {
        isSet = true;
        showNames = true;
        nHead = nhead;
        secStructName = ssName;
        firstSeq = fSeq;
        lastSeq = lSeq;
        numSequences = lastSeq - firstSeq + 1;
        selectedRows.clear();
        selectedRows.resize(numSequences);
        setMaxNameLength();  //recompute width
    }
    else
    {
        isSet = false;
        showNames = false;
        nHead = 0;
        secStructName = "";
        firstSeq = fSeq;
        lastSeq = lSeq;
        numSequences = 0;
        selectedRows.clear();
        selectedRows.resize(numSequences);            
        updateSizeHint();  //- may not be necessary now, 20-02-07,nige
    }
    resize(computeSize());
    update();
}

QSize SeqNameWidget::computeSize() const
{    
    if (! alignment)
        return viewSize;

    int extraRow   = getExtraRowNum();   
    int width      = DEFAULT_MAX_NAME_LENGTH;
    int maxNumSeqs = (numSequences2 > numSequences ? numSequences2 : numSequences);
    int height     = (maxNumSeqs + extraRow) * boxSize;
    
    if (maxNameLength > width) {
        width = maxNameLength;
    }
    width += horizontalMargin * 2;

    if (width  < viewSize.width())
        width  = viewSize.width();
    if (height < viewSize.height())
        height = viewSize.height();

    //printf("SNW::computeSize(%0x, w=%d, h=%d) [mnl=%d; bh=%d] [ns=%d:%d/%d]\n",
    //       (int)this, width, height, maxNameLength, boxSize, maxNumSeqs, numSequences, numSequences2);

    return QSize(width, height);
}

/**
 * This function overrides the parents sizeHint function. It gives an idea of 
 * the size this widget needs. 
 */
//- may not be necessary now, 20-02-07,nige
QSize SeqNameWidget::sizeHint() const
{
    QSize newSize = computeSize();

    //printf("SeqNameWidget::sizeHint(%0x, w=%d, h=%d) [mnl=%d; bh=%d]\n",
    //       (int)this, newSize.width(), newSize.height(), maxNameLength, boxSize);
    
    return newSize;
}

//- may not be necessary now, 20-02-07,nige
void SeqNameWidget::updateSizeHint()
{    
    QSize newSize = computeSize();

    //printf("SeqNameWidget::updateSizeHint(%0x, w=%d, h=%d) [mnl=%d; bh=%d]\n",
    //       (int)this, newSize.width(), newSize.height(), maxNameLength, boxSize);
    
    resize(newSize);
}

/**
 * This function is an event handler. It finds the sequence name that has been clicked.
 * @param *event A mouse event.
 * 
 * 11-04-07,nige: extended to add [shift]-select and [ctrl]-select.
 */
void SeqNameWidget::mousePressEvent(QMouseEvent *event)
{
    if (numSequences > 0 && event->button() == Qt::LeftButton) {
        
        mousePressed = true;
    
        KeyController *kbd = KeyController::Instance();
        
        /* find which row has been selected */
        int newRow = (event->y() / boxSize);
        newRow -= getExtraRowNum();
        
        /* starting over? */
	if (firstRow < 0)
	    firstRow = secondRow = newRow;

        if (0 <= newRow && newRow < (int)selectedRows.size()) {
            if (kbd->shiftPressed()) {
                /* select a range of rows */
                if (firstRow < newRow) {
                    for (int i = firstRow; i <= newRow; i++) {
                        selectedRows[i] = 1;
                    }
                } else {
                    for (int i = firstRow; i >= newRow; i--) {
                        selectedRows[i] = 1;
                    }
                }
                secondRow = newRow;
            } else if (kbd->ctrlPressed()) {
                /* nige: toggle */
                if (selectedRows[newRow] == 0) {
                    /* select this row and leave firstRow */
                    selectedRows[newRow] = 1;
                } else {
                    /* deselect this row and clear firstRow */
                    selectedRows[newRow] = 0;
                    firstRow = -1;
                }
            } else {
                /* deselect previous selection and select this row */
                for (int i = 0; i < numSequences; i++) {
                    selectedRows[i] = 0;
                }
                selectedRows[newRow] = 1;
                firstRow = newRow;
            }
        }
        update();
        return;
    }

    QWidget::mousePressEvent(event);
}

void SeqNameWidget::mouseReleaseEvent(QMouseEvent *event)  //nige
{
    if (event->button() == Qt::LeftButton) {
        mousePressed = false;
        update();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void SeqNameWidget::mouseMoveEvent(QMouseEvent *event)  //nige
{
    KeyController *kbd = KeyController::Instance();
    
    if (numSequences > 0 && mousePressed && !kbd->ctrlPressed()) {

        //scroll if necessary to continue selection
        scrollArea->ensureVisible(event->x(), event->y());

        //which row? don't overshoot either end
        int newRow = (event->y() / boxSize) - getExtraRowNum();
        if (newRow < 0)
            newRow = 0;
        if (newRow >= numSequences)
            newRow = numSequences - 1;
        //printf("%d -> %d, %d\n", newRow, firstRow, secondRow);

        if (firstRow <= secondRow) {
            if (newRow < secondRow) {
                //deselect between newRow and secondRow:
                // F === N === S  ->  F === N --- S
                for (int i = newRow+1; i <= secondRow; i++) {
                    //cout << "case 1\n";
                    selectedRows[i] = 0;
                }
            } else {
                //reselect from firstRow to newRow
                // F === S --- N  ->  F === S === N
                for (int i = firstRow; i <= newRow; i++) {
                    //cout << "case 2\n";
                    selectedRows[i] = 1;
                }
            }
        } else {
            if (secondRow < newRow) {
                //deselect between newRow and secondRow:
                // S === N === F  ->  S --- N === F
                for (int i = secondRow; i < newRow; i++) {
                    //cout << "case 3\n";
                    selectedRows[i] = 0;
                }
            } else {
                //reselect from firstRow to newRow
                // N --- S === F  ->  N === S === F
                for (int i = newRow; i <= firstRow; i++) {
                    //cout << "case 4\n";
                    selectedRows[i] = 1;
                }
            }
        }
        secondRow = newRow;
        update();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

/* 16-02-07,nige: track the containing viewport's size, then pass event on */
bool SeqNameWidget::eventFilter(QObject *o, QEvent *e)
{
    //printf("SeqNameWidget::eventFilter(%0x, %d)\n", (int)this, e->type());

    if (parentWidget() && parentWidget() == o) {
        if (e->type() == QEvent::Resize) {
            QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(e);
            
            //record known viewport size
            viewSize = resizeEvent->size();

            QSize newSize = computeSize();
            resize(newSize);

            //printf("SeqNameWidget::eventFilter(%0x, resize) nw=%d nh=%d, mnl=%d\n",
            //       (int)this, newSize.width(), newSize.height(), maxNameLength);

            //fall through to allow the viewport to respond
            return false;
        }
    }
    return false;
}

/**
 * This is an event handler. It repaints the widget when it needs to be repainted.
 * @param *event A paint event.
 */
void SeqNameWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QBrush(DEFAULT_BACKGROUND_COLOUR));
    painter.setFont(displayFont);

    QRect redrawRect = event->rect();

    // Set up the begin and end rows. 
    int extra = getExtraRowNum();
    int beginRow = 0;
    int endRow = numSequences - 1 + extra;
    int currentRow = firstSeq;

    //width of text drawing area
    int textWidth = width() - horizontalMargin * 2;
    
    QFontMetrics fontMetrics(displayFont);
    painter.setPen(QPen(Qt::black));

    //printf("SeqNameWidget::paintEvent(%0x, w=%d, h=%d) [mnl=%d; bh=%d]\n",
    //       (int)this, width(), height(), maxNameLength, boxSize);

    if (firstSeq != 0 && isSet && alignment && showNames)
    {
        for (int row = beginRow; row <= endRow; ++row) 
        {
            currentRow = firstSeq + row - extra; 
            
            if (row == 0)
            {
                painter.fillRect(0, 0, width(), boxSize,
                                 QBrush(backGroundGrey->rgb()));
            }
            else if (nHead == 1 && row == 1)
            {
                painter.fillRect(0, row * boxSize, width(), boxSize,
                                 QBrush(backGroundGrey->rgb()));
                painter.drawText(QRect(horizontalMargin, row * boxSize + offSetFromTop, textWidth, boxSize),
                                 Qt::AlignRight, secStructName);
            }
            else
            { 
                // If the row has been selected, then highlight it. 
                if ((int)selectedRows.size() > row - extra && selectedRows[row - extra] == 1)
                {
                    painter.fillRect(0, row * boxSize, width(), boxSize,
                                     QBrush(selectedColor->rgb()));
                }
                painter.drawText(QRect(horizontalMargin, row * boxSize + offSetFromTop, textWidth, boxSize),
                                 Qt::AlignRight, 
                                 QString(alignment->getName(currentRow).c_str()));
            }
        }
    }
}

/*
 * Recompute maxNameLength based on the lengths of the identifier strings
 * typeset in the current font at the current size, or
 * DEFAULT_MAX_NAME_LENGTH, whichever is larger.
 *
 * Created: 16-02-07,Nigel Brown(EMBL)
 **/
int SeqNameWidget::setMaxNameLength() {
    //printf("setMaxNameLength(%0x)\n", (int)this);
    maxNameLength = 0;
    if (alignment) {
        QFontMetrics fontMetrics(displayFont);
        clustalw::Alignment::NameIterator i;
        i.begin(alignment);
        while (! i.end()) {
            string name = i.next();
            //cout << "name: " << name << endl;
            int length = fontMetrics.width(QString(name.c_str()));
           if (length > maxNameLength)
                maxNameLength = length;
        }
    }
    if (maxNameLength < 1)
        maxNameLength = DEFAULT_MAX_NAME_LENGTH;

    return maxNameLength;
}

void SeqNameWidget::changeBoxSizeAndFontSize(int _boxSize, QFont* font)
{
    boxSize = _boxSize;
    displayFont = *font;
    setMaxNameLength();
    resize(computeSize());
    update();
}

void SeqNameWidget::clearSequenceSelection()
{
    int sizeOfSelected = selectedRows.size();

    for(int i = 0; i < sizeOfSelected; i++)
    {
        selectedRows[i] = 0;
    }
    update();
}

void SeqNameWidget::selectAllSequences()
{
    int sizeOfSelected = selectedRows.size();

    for(int i = 0; i < sizeOfSelected; i++)
    {
        selectedRows[i] = 1;
    }
    update();
}

int SeqNameWidget::getNumberOfSeqsSelected()
{
    int num = 0;
    int sizeOfSelected = selectedRows.size();
    
    for(int i = 0; i < sizeOfSelected; i++)
    {
        if(selectedRows[i] == 1)
        {
            num++;
        }
    }
    return num++;
}

int SeqNameWidget::getExtraRowNum() const //-nige
{
    if (nHead != 1)
        return 1;
    return 2;
}

//nige 26-06-07
void SeqNameWidget::setNumSequences2(int count) {
    //printf("RECV(SNW, setNumSequences2) %0x (%d)\n", (int)this, count);
    numSequences2 = count;
}
