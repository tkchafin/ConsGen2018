/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 * 
 * CHANGES:
 * Mark: Jan 16th 2007. I am working on Bugzilla bug 11. Allowing the user to
 * paste sequences into the lower profile even though there has only been one
 * profile added. I have changed the pasteSequences() function.
 
 * Mark: Jan 18th 2007. Bugzilla bug 3. Changed the way Files are opened
 *
 * 12-4-07, Mark Larkin : Changed destructor. We dont need to delete QObjects.
 *
 * 25-04-07,Nigel Brown(EMBL): Added keyPressEvent() and keyReleaseEvent().
 *
 * 16-5-07, Mark Larkin: Added a submenu to the tree menu to allow the selection of
 * NJ or UPGMA for tree construction. Also added support functions.
 * Also added a sub menu for iteration selection. 
 *
 * 22-5-2007, Mark Larkin: added function hideLowScoreSegs() 
 *
 * 23-5-2007, Mark Larkin: added resetAndHideLowScoreSeqs()
 *
 * 24-05-07,Nigel Brown(EMBL): simplified default parameter loading mechanism;
 * removed setPathToExecutable(), removeExecutableNameFromPath(),
 * executablePath members. Added loadDefaultParamFile().
 *
 * 2007-12-03, Andreas Wilm (UCD):  moved loadSequences() with optional
 * file argument to public slot
 *
 ****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <list>
#include "AlignmentViewerWidget.h"
#include "AlignmentFormatOptions.h"
#include "TreeFormatOptions.h"
#include "ProteinGapParameters.h"
#include "AlignmentParameters.h"
#include "ColumnScoreParams.h"
#include "LowScoringSegParams.h"
#include "PairwiseParams.h"
#include "clustalW/Clustal.h"
#include "clustalW/general/userparams.h"
#include "clustalW/alignment/Sequence.h"
#include "ColorFileXmlParser.h"
#include "ColorParameters.h"
#include "SearchForString.h"
#include "version.h"

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QComboBox;
class QStringList;
class QGroupBox;
class QTextEdit;
class QListView;
class QListWidget;
class QListWidgetItem;
class QTextCursor;
class QFont;
class QTableView;
class QScrollArea;
class QScrollBar;
class QCheckBox;
class QVBoxLayout;
class QHBoxLayout;
class QKeyEvent;


/** 
 * This class is an extension of the widget QMainWindow. It contains an 
 * alignment widget, a histogram widget and a sequence name widget. 
 * It is currently used as an alignment viewer, but it will be possible to extend its
 * functionality to an alignment editor.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    
public slots:
    void loadSequences(QString myFile = "");
    void commandLineHelp();
    
    // the following are public so they can be called from main.cpp at startup
    void setProfileModeCmd();
    void loadProfile1(string myFile);
    void loadProfile2(string myFile);

protected:

/* 
 * slots are functions, they can be linked to signals 
 */
    void showEvent(QShowEvent* event);
     
private slots:
    void changeMode(int mode);
    void enableProfileActions();
    void enableSequenceActions();
    
    // Functions for menu actions
    // File menu
    void appendSequences();
    void saveSequencesAs();
    void loadProfile1();
    void loadProfile2();
    void saveProfile1As();
    void saveProfile2As();
    void writeAlignmentAsPostscript();
    void writeProfile1AsPostscript();
    void writeViewer1AsPostscript(int type);
    void writeProfile2AsPostscript();
    void quitProgram();
    // Edit menu
    void cutSequences();
    void pasteSequences();
    void selectAllSequences();
    void selectProfile1();
    void selectProfile2();
    void addProfile2ToProfile1();
    void clearSequenceSelection();
    void clearRangeSelection();
    void searchForString();
    void removeAllGaps();
    void removeGapOnlyColumns();
    //Alignment menu
    void doCompleteAlignment();
    void produceGuideTreeOnly();
    void doAlignmentFromGuideTree();
    void realignSelectedSequences();
    void realignSelectedResidueRange();
    void alignProfile2ToProfile1();
    void alignProfilesFromGuideTrees();
    void alignSequencesToProfile1();
    void alignSequencesToProfile1FromTree();
    void outputFormatOptions();
    void setDefaultParameters();
    // Submenu
    void resetNewGapsBeforeAlignment();
    void resetAllGapsBeforeAlignment();
    void pairwiseAlignmentParameters();
    void multipleAlignmentParameters();
    void proteinGapParameters();
    void secondaryStructureParameters();
    // Tree menu
    void drawNJTree();
    void bootstrapNJTree();
    void excludePositionsWithGaps();
    void correctForMultipleSubstitutions();
    void treeOutputFormatOptions();
    // Colors menu
    void backgroundColoring();
    void blackAndWhite();
    void defaultColors();
    void loadColorParameterFile();
    // Quality menu
    void showLowScoreSeg();
    void showExceptionalRes();
    void lowScoreSegParams();
    void columnScoreParams();
    void saveColumnScoresToFile();

    // Help menu
    void generalHelp();
    void inputOutputHelp();
    void editAlignHelp();
    void multiAlignHelp();
    void profileAlignHelp();
    void secondaryStructureHelp();
    void treesHelp();
    void colorsHelp();
    void alignQualityHelp();
    void referencesHelp();
    // Clustering algorithm submenu
    void setUPGMA(); // Mark 16-5-07
    void setNJ(); // Mark 16-5-07
    
    // Iteration submenu
    void setNoIteration(); // Mark 16-5-07
    void setTreeIteration(); // Mark 16-5-07
    void setAlignIteration(); // Mark 16-5-07
        
    void scrollLockClicked();
    void searchFromStart();
    void searchAgain();
private:
    void createActions();
    void createMenus();
    void createAlignModeMenu();
    void createFontMenu();
    void createHorizontalBox();
    void errorHandler(int errorCode, string offendingSeq=NULL);
    void calculateLowScoreAlign();
    void calculateLowScoreProfile();
    void recalculateLowScoreSegments();
    void loadDefaultParamFile(const QString &fileName, ColorParameters* colorParam, 
                              bool showMessage = true); //- nige
    bool readInParamFile(QString file, ColorParameters* colorParam, bool showMessage = true);
    void setUpSearchForStringDialog();
    bool findNextInstanceOfString(const std::vector<int>* seqsSelected, int firstSeq, 
                                  Viewer viewerSearched);
    vector<int> getAllSelectedSeqsVector();
    bool allSeqsInAlignmentOrProfileSelected(std::vector<int>* seqsSelected);
    bool anySeqsSelected();
    void setAllPtrsToZero();
    void hideLowScoreSegs();
    void resetAndHideLowScoreSeqs();
    QVBoxLayout *mainlayout;
    QWidget *sideFiller;
    QHBoxLayout *layoutHbox;
    
    AlignmentViewerWidget* alignmentViewer; // Displayes alignment
    AlignmentViewerWidget* profile2Viewer; // displays profile 2
    clustalw::Clustal* clustalObj; 
    clustalw::Alignment* alignPtr;
    int profileMode;
    int multiMode;
    int currentMode;
    
    QWidget *w; 
    
    QMenu *fileMenu; 
    QMenu *editMenu; 
    QMenu *alignmentMenu; 
    QMenu *treeMenu;
    QMenu *colorMenu;
    QMenu *qualityMenu;
    QMenu *helpMenu;
    list<QMenu *> toplevelMenuList;// AW used for enabling/disabling menus
        
    QMenu *treeSubMenu; // Mark 16-5-07
    QMenu *iterationSubMenu; // Mark 16-5-07
    QComboBox *alignmentModeMenu;
    QComboBox *fontMenu;


    /* 
     * Each command in Qt is usually implemented as an action. 
     * These are the commands for the file menu
     */
    QActionGroup *alignmentGroup;
    QAction *actionLoadSequences;
    QAction *actionAppendSequences;
    QAction *actionSaveSeqAs;
    QAction *actionLoadProfile1;
    QAction *actionLoadProfile2;
    QAction *actionSaveProfile1As;
    QAction *actionSaveProfile2As;
    QAction *actionWriteAlignmentAsPost;
    QAction *actionWriteProfile1AsPost;
    QAction *actionWriteProfile2AsPost;
    QAction *actionQuit;
    
    /* 
     * Actions for the clustal edit menu 
     */
    QAction *actionCutSequences;
    QAction *actionPasteSequences;
    QAction *actionSelectAllSequences;
    QAction *actionSelectProfile1;
    QAction *actionSelectProfile2;
    QAction *actionAddProfile2ToProfile1;
    QAction *actionClearSequenceSelection;
    QAction *actionClearRangeSelection;
    QAction *actionSearchForString;
    QAction *actionRemoveAllGaps;
    QAction *actionRemoveGapOnlyColumns;

    /* 
     * Actions for the Alignment menu 
     */
    QAction *actionDoCompleteAlign;
    QAction *actionDoGuideTreeOnly;
    QAction *actionDoAlignmentFromGuideTree;
    QAction *actionRealignSelectSeq;
    QAction *actionRealignSelectRange;
    QAction *actionAlignProfile1ToProfile2;
    QAction *actionAlignProfileFromTree;
    QAction *actionAlignSequencesToProfile1;
    QAction *actionAlignSequencesToProfile1FromTree;
    QMenu *menuAlignParameters;
    QAction *actionOutputFormatOptions;
    QAction *actionResetNewGapsBeforeAlignment;
    QAction *actionResetAllGapsBeforeAlignment;
    QAction *actionPairwiseAlignmentParameters;
    QAction *actionMultipleAlignmentParameters;
    QAction *actionProteinGapParameters;
    QAction *actionSecondaryStructureParameters;
    QAction *actionSetDefaultParameters;

    /* 
     * Actions for the Tree menu 
     */
    QAction *actionDrawNJTree;
    QAction *actionBootstrapNJTree;
    QAction *actionExcludePosWithGap;
    QAction *actionCorrectForMultiSub;
    QAction *actionTreeOutputFormatOptions;
    /* 
     * Actions for the color Menu 
     */
    QAction *actionBackColoring;
    QAction *actionBlackAndWhite;
    QAction *actionDefaultColors;
    QAction *actionLoadColorParameterFile;

    /* 
     * Actions for the quality Menu 
     */
    QAction *actionShowLowScoreSeg;
    QAction *actionShowExceptionalRes;
    QAction *actionLowScoreSegParams;
    QAction *actionColumnScoreParams;
    QAction *actionSaveColumnScoresToFile;
    
    /* 
     * Actions for the help menu
     */
    QAction *actionGeneralHelp;
    QAction *actionInputOutputHelp;
    QAction *actionEditAlignHelp;
    QAction *actionMultiAlignHelp;
    QAction *actionProfileAlignHelp;
    QAction *actionSecondaryStructureHelp;
    QAction *actionTreesHelp;
    QAction *actionColorsHelp;
    QAction *actionAlignQualityHelp;
    QAction *actionCommandLineHelp;
    QAction *actionReferencesHelp;
        
    /** Mark 16-5-07
     * Actions for the Clustering algorithm sub menu
     */
    QAction *actionUseUPGMA; // Mark 16-5-07
    QAction *actionUseNJ;  // Mark 16-5-07
    
    /** Mark 16-5-07
     * Actions for the iteration sub menu
     */
    QAction *actionNoIteration; // Mark 16-5-07
    QAction *actionTreeIteration; // Mark 16-5-07
    QAction *actionAlignIteration; // Mark 16-5-07
           
    QGroupBox *horizontalGroupBox;

    QCheckBox* profileScrollLock;
    /* 
     * The main font for the application 
     */
    QFont displayFont;
    
    AlignmentParameters* alignmentParams;
    PairwiseParams* pairwiseParams;
    ColumnScoreParams* colScoreParams;
    LowScoringSegParams* lowScoreParams;
    SearchForString* searchSeqs; 
    
    bool showExceptionalResidues;
    bool showLowScoreSegments;
    
    bool seqWeightCalculatedAlignmentViewer;
    vector<int>* lowScoreSeqWeightAlignmentViewer; // Should be nSeqs long + 1
    clustalw::Array2D<int>* lowScoreResAlignmentViewer; 
    
    bool seqWeightCalculatedProfile2Viewer;
    vector<int>* lowScoreSeqWeightProfile2Viewer; // Should be nSeqs long + 1
    clustalw::Array2D<int>* lowScoreResProfile2Viewer;  
    
    QLabel* fontLabel;
    QLabel* alignModeLabel;
    QLabel* bottomInfoLabel;
    
    ColorParameters proteinColors;
    ColorParameters dnaColors;
    ColorParameters userColors;
    ColorParameters printerColors;
    
    QString protColorFile;
    QString dnaColorFile;
    QString printerColorFile;   
    
    QString sequenceFileName;
    QString profile2FileName;
    bool backGroundColors;
    int searchBeginSeq;
    int searchBeginRes; 
    
    std::string searchString; 
    vector<clustalw::Sequence> cutSeqs;

    //- nige
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

    void toplevelMenusEnabled(bool b);
};

#endif
