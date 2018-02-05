/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 *
 * Changes:
 *
 * Nigel Brown,EMBL,18-12-06: changed version string.
 *
 * Mark: 18-01-2007: Made changes for bug 3, to remember the last directory. Using 
 * FileDialog instead of QFileDialog.
 * 
 * Mark: 19-1-2007 added more items to the font combobox.
 *
 * 31-01-07,Nigel Brown(EBL): changed font sizes available in createFontMenu()
 * controlled by OS_MAC from "compile.h".
 *
 * 03-02-07,Nigel Brown(EMBL): added 'All Files (*)' selection in
 * loadColorParameterFile(). All references to getOpenFileName() changed to
 * getOpenDataFileName().
 *
 * 07-02-07,Nigel Brown(EMBL): fixed typos in main menu.
 *
 * 16-02-07,Nigel Brown(EMBL): added more font sizes to createFontMenu().
 *
 * 05-03-07,Nigel Brown(EMBL): upcased search string in searchAgain().
 *
 * 12-03-07,Nigel Brown(EMBL): fixed Alignment menu typo.
 *
 * 15-03-07,Nigel Brown(EMBL): changed font sizes to mainly classical
 * typographic scalings.
 *
 * 26-03-07,Nigel Brown(EMBL): loadProfile2() now redraws original Profile1
 * after error loading Profile2.
 *
 * 30-03-07,Nigel Brown(EMBL): loadProfile2() now redraws original Profile1
 * after error loading Profile2; alignProfile2ToProfile1(),
 * alignProfilesFromGuideTrees(), alignSequencesToProfile1() and
 * alignSequencesToProfile1FromTree() now draw Profile1 and Profile2
 * separately after alignment.
 *
 * 02-04-07,Nigel Brown(EMBL): uncommented the "No sequences in file..."
 * warning in errorHandler() and disabled its counterpart in
 * FileReader::seqInput() which was causing a crash during the repaint after
 * the user accepts the message, because some state (what exactly?) is
 * unstable at that depth after a sequence load error, specifically when
 * loading an empty file after already loading some sequences.
 *
 * 12-4-07, Mark Larkin : Changed destructor. We dont need to delete QObjects.
 *
 * 25-04-07,Nigel Brown(EMBL): Added a singleton KeyController class and
 * keyPressEvent() and keyReleaseEvent(), which call KeyController to set the
 * keyboard state for the whole application. All handlers of [Ctrl]+* keyboard
 * shortcuts MUST call KeyController::clearCtrl() to simulate a corresponding
 * key release event.
 *
 * 04-05-07,Nigel Brown(EMBL): changed pasteSequences() to insert at end of a
 * selected block, rather than inside it. pasteSequences() now uses
 * AlignmentViewerWidget::hasFocus() (instead of AVW::hasSeqNameWidgetFocus()).
 * 'no seqs loaded' messages turned into English.
 *
 * 16-5-07, Mark Larkin: Added a submenu to the tree menu to allow the selection of
 * NJ or UPGMA for tree construction. Also added support functions.
 * Also added a sub menu for iteration selection.  
 *
 * 22-5-2007, Mark Larkin: added function hideLowScoreSegs(), this is called
 * after each change of mode, cut and paste. 
 *
 * 23-5-2007, Mark Larkin: added resetAndHideLowScoreSeqs()
 *
 * 24-05-07,Nigel Brown(EMBL): simplified default parameter loading
 * mechanism in readInParamFile(); removed setPathToExecutable(),
 * removeExecutableNameFromPath(), executablePath members. Added
 * Resources class instead; added loadDefaultParamFile().
 *
 * 27-06-07,Nigel Brown(EMBL): For screen layout balancing: added two
 * connections to exchange AlignmentViewerWidget number-of-sequence changes.
 *
 * 1-9-2007, Mark Larkin: Setting UPGMA now makes "Bootstrap NJ tree"
 * inactive.
 *
 * 2007-12-03, Andreas Wilm (UCD): added optional file argument to
 * loadSequences
 * 
 * 26-11-07,Nigel Brown(EMBL): Fixed spacing typo in message in
 * readInParamFile(); changed WriteViewer1AsPostscript() and
 * WriteViewer2AsPostscript() to trigger error message on missing
 * colour parameter file via readInParamFile().
 *
 ****************************************************************************/

#include <QtGui>
#include <QPixmap>
#include <QFile>
#include <iostream>
#include <cstdlib>
#include <QCheckBox>
#include <QStringList>
#include "SecStructOptions.h"
#include "mainwindow.h"
#include "WritePostscriptFile.h"
#include "AlignOutputFileNames.h"
#include "TreeOutputFileNames.h"
#include "BootstrapTreeDialog.h"
#include "HelpDisplayWidget.h"
#include "FileDialog.h"
#include "KeyController.h"
//AW not needed anymore: #include "compile.h"
#include "Resources.h"

/**
 * This is the constructor for a MainWindow widget. It sets up all the initial 
 * values for a MainWindow.
 * It is not currently possible to change the default values.
 */
MainWindow::MainWindow()
 :   showExceptionalResidues(false),
     showLowScoreSegments(false),
     seqWeightCalculatedAlignmentViewer(false),
     seqWeightCalculatedProfile2Viewer(false),
     backGroundColors(true),
     searchBeginSeq(0),
     searchBeginRes(0)
{
    setAllPtrsToZero();
    QFont infoFont("Helvetica", 10, QFont::Bold);
     
    protColorFile = psColorFile;
    dnaColorFile = psDNAFile;
    printerColorFile = psPrintFile;
     
    bottomInfoLabel = new QLabel("");
    bottomInfoLabel->setFont(infoFont);
    
    lowScoreSeqWeightAlignmentViewer = new vector<int>;
    lowScoreResAlignmentViewer = new clustalw::Array2D<int>();
    lowScoreSeqWeightProfile2Viewer = new vector<int>;
    lowScoreResProfile2Viewer = new clustalw::Array2D<int>();
            
    alignmentParams = new AlignmentParameters;
    pairwiseParams = new PairwiseParams;
    colScoreParams = new ColumnScoreParams;
    lowScoreParams = new LowScoringSegParams;
    
    displayFont.setPixelSize(12);
    w = new QWidget;
    w->setFont(displayFont);
    setCentralWidget(w);
    
    /* 
     * Call the functions to setup the horizontal and grid layout boxes in the main window 
     */
    createHorizontalBox();
    alignmentViewer = new AlignmentViewerWidget();
    profile2Viewer = new AlignmentViewerWidget();
    profile2Viewer->setVisible(false);
    
    clustalObj = new clustalw::Clustal;
    alignmentViewer->setAlignmentView(clustalObj->getAlignmentPtr(), 0, 0);
    profile2Viewer->setAlignmentView(clustalObj->getAlignmentPtr(), 0, 0);
    alignPtr = clustalObj->getAlignmentPtr();
    /* 
     * The QVBoxLayout class lines up widgets vertically 
     */
    mainlayout = new QVBoxLayout;
    mainlayout->setMargin(5);
    mainlayout->addWidget(horizontalGroupBox);
    mainlayout->addWidget(alignmentViewer);
    mainlayout->addWidget(profile2Viewer);
    mainlayout->addWidget(bottomInfoLabel);

    w->setLayout(mainlayout);

    /*
     * Sets up the menus in the menubar. Also sets up the actions associated 
     * with the menu options.
     */
    createActions();
    createMenus();
    enableSequenceActions();
    /*
     * Sets initial size for the mainwindow
     */
    setWindowTitle(QString(CLUSTALX_NAME)+ " " + QString(CLUSTALX_VERSION));
    setMinimumSize(160, 160);
    repaint();
    resize(1000, 600);
    
    connect(fontMenu, SIGNAL(activated(const QString &)),
            alignmentViewer, SLOT(updateFontAndBoxes(const QString &)));
            
    connect(fontMenu, SIGNAL(activated(const QString &)),
            profile2Viewer, SLOT(updateFontAndBoxes(const QString &)));        
            
    currentMode = multiMode;
    
    setUpSearchForStringDialog();
}

void MainWindow::showEvent(QShowEvent* event)
{
    clustalw::utilityObject->setInfoLabelPtr(bottomInfoLabel);    
    
    loadDefaultParamFile(protColorFile, &proteinColors);
    loadDefaultParamFile(dnaColorFile, &dnaColors);
    loadDefaultParamFile(printerColorFile, &printerColors, false);  

    bool dnaFlag = clustalw::userParameters->getDNAFlag();
    if(dnaFlag)
    {
        alignmentViewer->setColorScheme(&dnaColors);
        profile2Viewer->setColorScheme(&dnaColors);        
    }
    else
    {
        alignmentViewer->setColorScheme(&proteinColors);
        profile2Viewer->setColorScheme(&proteinColors);    
    }
    
    alignmentViewer->setPrintoutColorScheme(printerColors);
    profile2Viewer->setPrintoutColorScheme(printerColors); 
}

void MainWindow::loadDefaultParamFile(const QString &fileName, ColorParameters* colorParam, 
                                      bool showMessage) {
    Resources *res = Resources::Instance();
    QString file = res->searchPathsForFile(fileName);
    //cout << "paramFile: " << file.toStdString().c_str() << endl;
    readInParamFile(file, colorParam, showMessage);
}

/**
 * The readInParamFile tries to read in the file "fileName" and store the info in the
 * colorParams object.
 * @param fileName the name of the color parameter file to be read.
 * @param colorParam the ColorParameters object to store the info from the file. 
 * @param showMessage show messages if there is something wrong??
 * @return true if successful, false if not.
 */
bool MainWindow::readInParamFile(QString fileName, ColorParameters* colorParam, 
                                 bool showMessage)
{
    // if we cant read in the file, xml error etc, clear the object
    // and set it to use the defaults.
    if(colorParam != 0)
    {
        QFile file(fileName);
        file.open(QFile::ReadOnly | QFile::Text);
        
        if (!file.isOpen()) 
        {
            if(showMessage)
            {
                QMessageBox::warning(this, tr("Color Parameter file"),
                                     tr("Cannot read color file %1:\n Using default colors.")
                                     .arg(fileName));
            }
            // Use hardcoded colors!!!
            colorParam->clear();
            colorParam->useHardCodedColors();
            colorParam->useDefaultColorRules();   
            return false;
        }
    
        bool success;    
        ColorFileXmlParser colorFileParser(showMessage);
        success = colorFileParser.read(&file);
    
        if(success)
        {
            *colorParam = *colorFileParser.getColorParametersObj();
            //ColorParameters(*colorFileParser.getColorParametersObj());
            return true;
        }
        else
        {
            colorParam->clear();
            colorParam->useHardCodedColors();
            colorParam->useDefaultColorRules();  
            return false;  
        }
    }
    return true;
}

/** 
 * The function createActions is used to set up the actions to be used in the menus. 
 */
void MainWindow::createActions()
{

    /* 
     * Clustal File options 
     */
    actionLoadSequences = new QAction(tr("Load Sequences"), this);
    actionLoadSequences->setShortcut(tr("Ctrl+O"));
    actionLoadSequences->setStatusTip(tr("Load Sequences from File"));
    connect(actionLoadSequences, SIGNAL(triggered()), this, SLOT(loadSequences()));

    actionAppendSequences = new QAction(tr("Append Sequences"), this);
    actionAppendSequences->setStatusTip(tr("Append Sequences from File"));
    connect(actionAppendSequences, SIGNAL(triggered()), this, SLOT(appendSequences()));

    actionSaveSeqAs = new QAction(tr("Save Sequences as..."), this);
    actionSaveSeqAs->setShortcut(tr("Ctrl+S"));
    actionSaveSeqAs->setStatusTip(tr("Save Sequences as..."));
    connect(actionSaveSeqAs, SIGNAL(triggered()), this, SLOT(saveSequencesAs()));
    
    actionLoadProfile1 = new QAction(tr("Load Profile 1"), this);
    actionLoadProfile1->setStatusTip(tr("Load Profile 1"));
    connect(actionLoadProfile1, SIGNAL(triggered()), this, SLOT(loadProfile1()));
    
    actionLoadProfile2 = new QAction(tr("Load Profile 2"), this);
    actionLoadProfile2->setStatusTip(tr("Load Profile 2"));
    connect(actionLoadProfile2, SIGNAL(triggered()), this, SLOT(loadProfile2()));
    
    actionSaveProfile1As = new QAction(tr("Save Profile 1 as..."), this);
    actionSaveProfile1As->setStatusTip(tr("Save Profile 1"));
    connect(actionSaveProfile1As, SIGNAL(triggered()), this, SLOT(saveProfile1As()));
    
    actionSaveProfile2As = new QAction(tr("Save Profile 2 as..."), this);
    actionSaveProfile2As->setStatusTip(tr("Save Profile 2"));
    connect(actionSaveProfile2As, SIGNAL(triggered()), this, SLOT(saveProfile2As()));
    
    actionWriteAlignmentAsPost = new QAction(tr("Write Alignment as Postscript"), this);
    actionWriteAlignmentAsPost->setStatusTip(tr("Write Alignment as Postscript"));
    actionWriteAlignmentAsPost->setShortcut(tr("Ctrl+P"));
    connect(actionWriteAlignmentAsPost, SIGNAL(triggered()), this,
            SLOT(writeAlignmentAsPostscript()));
    
    actionWriteProfile1AsPost = new QAction(tr("Write Profile 1 as Postscript"), this);
    actionWriteProfile1AsPost->setStatusTip(tr("Write Profile 1 as Postscript"));
    connect(actionWriteProfile1AsPost, SIGNAL(triggered()), this,
            SLOT(writeProfile1AsPostscript()));
            
    actionWriteProfile2AsPost = new QAction(tr("Write Profile 2 as Postscript"), this);
    actionWriteProfile2AsPost->setStatusTip(tr("Write Profile 2 as Postscript"));
    connect(actionWriteProfile2AsPost, SIGNAL(triggered()), this,
            SLOT(writeProfile2AsPostscript()));
            
    actionQuit = new QAction(tr("Quit"), this);
    actionQuit->setStatusTip(tr("Quit"));
    actionQuit->setShortcut(tr("Ctrl+Q"));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(quitProgram()));
    
    /* 
     * Clustal Edit actions 
     */
    actionCutSequences = new QAction(tr("Cut Sequences"), this);
    actionCutSequences->setShortcut(tr("Ctrl+X"));
    actionCutSequences->setStatusTip(tr("Cut Sequences from Alignment"));
    connect(actionCutSequences, SIGNAL(triggered()), this, SLOT(cutSequences()));
    
    actionPasteSequences = new QAction(tr("Paste Sequences"), this);
    actionPasteSequences->setStatusTip(tr("Paste Sequences"));
    actionPasteSequences->setShortcut(tr("Ctrl+V"));
    connect(actionPasteSequences, SIGNAL(triggered()), this, SLOT(pasteSequences()));
    
    actionSelectAllSequences = new QAction(tr("Select All Sequences"), this);
    actionSelectAllSequences->setShortcut(tr("Ctrl+A"));
    connect(actionSelectAllSequences, SIGNAL(triggered()), this, SLOT(selectAllSequences()));
    
    actionSelectProfile1 = new QAction(tr("Select Profile 1"), this);
    connect(actionSelectProfile1, SIGNAL(triggered()), this, SLOT(selectProfile1()));
    
    actionSelectProfile2 = new QAction(tr("Select Profile 2"), this);
    connect(actionSelectProfile2, SIGNAL(triggered()), this, SLOT(selectProfile2()));
    
    actionAddProfile2ToProfile1 = new QAction(tr("Add Profile 2 to Profile 1"), this);
    connect(actionAddProfile2ToProfile1, SIGNAL(triggered()), this,
            SLOT(addProfile2ToProfile1()));
    
    actionClearSequenceSelection = new QAction(tr("Clear Sequence Selection"), this);
    connect(actionClearSequenceSelection, SIGNAL(triggered()), this,
            SLOT(clearSequenceSelection()));
            
    actionClearRangeSelection = new QAction(tr("Clear Range Selection"), this);
    connect(actionClearRangeSelection, SIGNAL(triggered()), this,
            SLOT(clearRangeSelection()));
            
    actionSearchForString = new QAction(tr("Search for String"), this);
    actionSearchForString->setShortcut(tr("Ctrl+F"));
    connect(actionSearchForString, SIGNAL(triggered()), this, SLOT(searchForString()));
            
    actionRemoveAllGaps = new QAction(tr("Remove All Gaps"), this);
    connect(actionRemoveAllGaps, SIGNAL(triggered()), this, SLOT(removeAllGaps()));
    
    actionRemoveGapOnlyColumns = new QAction(tr("Remove Gap-Only Columns"), this);
    connect(actionRemoveGapOnlyColumns, SIGNAL(triggered()), this,
            SLOT(removeGapOnlyColumns()));
            
    /* 
     * Alignment Menu 
     */
    actionDoCompleteAlign = new QAction(tr("Do Complete Alignment"), this);
    actionDoCompleteAlign->setShortcut(tr("Ctrl+L"));
    connect(actionDoCompleteAlign, SIGNAL(triggered()), this, SLOT(doCompleteAlignment()));
    
    actionDoGuideTreeOnly = new QAction(tr("Do Guide Tree Only"), this);
    actionDoGuideTreeOnly->setShortcut(tr("Ctrl+G"));
    connect(actionDoGuideTreeOnly, SIGNAL(triggered()), this, SLOT(produceGuideTreeOnly()));
    
    actionDoAlignmentFromGuideTree = new QAction(tr("Do Alignment from Guide Tree"), this);
    connect(actionDoAlignmentFromGuideTree, SIGNAL(triggered()), this,
            SLOT(doAlignmentFromGuideTree()));
    
    actionRealignSelectSeq = new QAction(tr("Realign Selected Sequences"), this);
    connect(actionRealignSelectSeq, SIGNAL(triggered()), this,
            SLOT(realignSelectedSequences()));
    
    actionRealignSelectRange = new QAction(tr("Realign Selected Residue Range"), this);
    connect(actionRealignSelectRange, SIGNAL(triggered()), this,
            SLOT(realignSelectedResidueRange()));
    
    actionAlignProfile1ToProfile2 = new QAction(tr("Align Profile 2 to Profile 1"), this);
    connect(actionAlignProfile1ToProfile2, SIGNAL(triggered()), this,
            SLOT(alignProfile2ToProfile1()));
    
    actionAlignProfileFromTree = new QAction(tr("Align Profiles from Guide Trees"), this);
    connect(actionAlignProfileFromTree, SIGNAL(triggered()), this,
            SLOT(alignProfilesFromGuideTrees()));
    
    actionAlignSequencesToProfile1 = new QAction(tr("Align Sequences to Profile 1"), this);
    connect(actionAlignSequencesToProfile1, SIGNAL(triggered()), this,
            SLOT(alignSequencesToProfile1()));
    
    actionAlignSequencesToProfile1FromTree = 
                    new QAction(tr("Align Sequences to Profile 1 from Tree"), this);
    connect(actionAlignSequencesToProfile1FromTree, SIGNAL(triggered()), this,
            SLOT(alignSequencesToProfile1FromTree()));       

    actionSetDefaultParameters = 
                    new QAction(tr("Set All Parameters to default"), this);
    connect(actionSetDefaultParameters, SIGNAL(triggered()), this,
            SLOT(setDefaultParameters()));
                
    menuAlignParameters = new QMenu(tr("Alignment Parameters"));
        
    actionOutputFormatOptions = new QAction(tr("Output Format Options"), this);
    connect(actionOutputFormatOptions, SIGNAL(triggered()), this,
            SLOT(outputFormatOptions()));
    
    actionResetNewGapsBeforeAlignment = new QAction(tr("Reset New Gaps before Alignment"),
                                                    this);
    actionResetNewGapsBeforeAlignment->setCheckable(true);
    connect(actionResetNewGapsBeforeAlignment, SIGNAL(triggered()), this,
            SLOT(resetNewGapsBeforeAlignment()));
        
    actionResetAllGapsBeforeAlignment = new QAction(tr("Reset All Gaps before Alignment"),
                                                    this);
    actionResetAllGapsBeforeAlignment->setCheckable(true);
    connect(actionResetAllGapsBeforeAlignment, SIGNAL(triggered()), this,
            SLOT(resetAllGapsBeforeAlignment()));
        
    actionPairwiseAlignmentParameters = new QAction(tr("Pairwise Alignment Parameters"),
                                                    this);
    connect(actionPairwiseAlignmentParameters, SIGNAL(triggered()), this,
            SLOT(pairwiseAlignmentParameters()));
        
    actionMultipleAlignmentParameters = new QAction(tr("Multiple Alignment Parameters"),
                                                    this);
    connect(actionMultipleAlignmentParameters, SIGNAL(triggered()), this,
            SLOT(multipleAlignmentParameters()));
        
    actionProteinGapParameters = new QAction(tr("Protein Gap Parameters"), this);
    connect(actionProteinGapParameters, SIGNAL(triggered()), this, 
            SLOT(proteinGapParameters()));
        
    actionSecondaryStructureParameters = new QAction(tr("Secondary Structure Parameters"),
                                                     this);
    connect(actionSecondaryStructureParameters, SIGNAL(triggered()), this,
            SLOT(secondaryStructureParameters()));
        
    menuAlignParameters->addAction(actionResetNewGapsBeforeAlignment);
    menuAlignParameters->addAction(actionResetAllGapsBeforeAlignment);
    menuAlignParameters->addAction(actionPairwiseAlignmentParameters);
    menuAlignParameters->addAction(actionMultipleAlignmentParameters);
    menuAlignParameters->addAction(actionProteinGapParameters);
    menuAlignParameters->addAction(actionSecondaryStructureParameters);                   

    /**
     * Mark: 16-5-07 Iteration sub menu
     */ 
    iterationSubMenu = new QMenu(tr("Iteration"));
    
    actionNoIteration = new QAction(tr("None"), this);
    actionNoIteration->setCheckable(true);
    connect(actionNoIteration, SIGNAL(triggered()), this,
            SLOT(setNoIteration()));
            
    actionTreeIteration = new QAction(tr("Iterate each alignment step"), this);
    actionTreeIteration->setCheckable(true);
    connect(actionTreeIteration, SIGNAL(triggered()), this,
            SLOT(setTreeIteration()));
            
    actionAlignIteration = new QAction(tr("Iterate final alignment"), this);
    actionAlignIteration->setCheckable(true);
    connect(actionAlignIteration, SIGNAL(triggered()), this,
            SLOT(setAlignIteration()));            

    iterationSubMenu->addAction(actionNoIteration);
    iterationSubMenu->addAction(actionTreeIteration);
    iterationSubMenu->addAction(actionAlignIteration);
    
    if(clustalw::userParameters->getDoRemoveFirstIteration() == clustalw::TREE)
    {
        actionNoIteration->setChecked(false);
        actionTreeIteration->setChecked(true);
        actionAlignIteration->setChecked(false);
    }
    else if(clustalw::userParameters->getDoRemoveFirstIteration() == clustalw::ALIGNMENT)
    {
        actionNoIteration->setChecked(false);
        actionTreeIteration->setChecked(false);
        actionAlignIteration->setChecked(true);    
    }
    else
    {
        actionNoIteration->setChecked(true);
        actionTreeIteration->setChecked(false);
        actionAlignIteration->setChecked(false);    
    }                                
    /* 
     * Tree Menu 
     */
    actionDrawNJTree = new QAction(tr("Draw Tree"), this); // Mark change 21-5-07
    actionDrawNJTree->setShortcut(tr("Ctrl+R"));
    connect(actionDrawNJTree, SIGNAL(triggered()), this, SLOT(drawNJTree()));
    
    actionBootstrapNJTree = new QAction(tr("Bootstrap N-J Tree"), this);
    actionBootstrapNJTree->setShortcut(tr("Ctrl+B"));
    connect(actionBootstrapNJTree, SIGNAL(triggered()), this, SLOT(bootstrapNJTree()));
    
    actionExcludePosWithGap = new QAction(tr("Exclude Positions with Gaps"), this);
    actionExcludePosWithGap->setCheckable(true);
    connect(actionExcludePosWithGap, SIGNAL(triggered()), this,
            SLOT(excludePositionsWithGaps()));
    
    actionCorrectForMultiSub = new QAction(tr("Correct for Multiple Substitutions"), this);
    actionCorrectForMultiSub->setCheckable(true);
    connect(actionCorrectForMultiSub, SIGNAL(triggered()), this,
            SLOT(correctForMultipleSubstitutions()));
        
    actionTreeOutputFormatOptions = new QAction(tr("Output Format Options"), this);
    connect(actionTreeOutputFormatOptions, SIGNAL(triggered()), this,
            SLOT(treeOutputFormatOptions()));
    /**
     * Mark 16-5-07
     * Tree sub menu
     */
    treeSubMenu =  new QMenu(tr("Clustering Algorithm"));
    
    actionUseUPGMA = new QAction(tr("UPGMA"), this);
    actionUseUPGMA->setCheckable(true);
    connect(actionUseUPGMA, SIGNAL(triggered()), this,
            SLOT(setUPGMA()));
    
    actionUseNJ = new QAction(tr("NJ"), this);
    actionUseNJ->setCheckable(true);
    connect(actionUseNJ, SIGNAL(triggered()), this,
            SLOT(setNJ()));
            
    treeSubMenu->addAction(actionUseUPGMA);
    treeSubMenu->addAction(actionUseNJ);
    
    if(clustalw::userParameters->getClusterAlgorithm() == clustalw::UPGMA)
    {
        actionUseUPGMA->setChecked(true);
        actionUseNJ->setChecked(false);
    }
    else
    {
        actionUseUPGMA->setChecked(false);
        actionUseNJ->setChecked(true);    
    }
                           
    /* 
     * Color Menu 
     */
    actionBackColoring = new QAction(tr("Background Coloring"), this);
    actionBackColoring->setCheckable(true);
    actionBackColoring->setChecked(true);
    connect(actionBackColoring, SIGNAL(triggered()), this, SLOT(backgroundColoring()));
    
    actionBlackAndWhite = new QAction(tr("Black and White"), this);
    actionBlackAndWhite->setCheckable(true);
    connect(actionBlackAndWhite, SIGNAL(triggered()), this, SLOT(blackAndWhite()));
    
    actionDefaultColors = new QAction(tr("Default Colors"), this);
    actionDefaultColors->setCheckable(true);
    actionDefaultColors->setChecked(true);
    connect(actionDefaultColors, SIGNAL(triggered()), this, SLOT(defaultColors()));
    
    actionLoadColorParameterFile = new QAction(tr("Load Color Parameter File"), this);
    //actionLoadColorParameterFile->setCheckable(true);
    connect(actionLoadColorParameterFile, SIGNAL(triggered()), this,
            SLOT(loadColorParameterFile()));
            
    /* 
     * Quality Menu 
     */ 
        
    actionShowLowScoreSeg = new QAction(tr("Show Low-Scoring Segments"), this);
    actionShowLowScoreSeg->setCheckable(true);
    connect(actionShowLowScoreSeg, SIGNAL(triggered()), this, SLOT(showLowScoreSeg()));
        
    actionShowExceptionalRes = new QAction(tr("Show Exceptional Residues"), this);
    actionShowExceptionalRes->setCheckable(true);
    connect(actionShowExceptionalRes, SIGNAL(triggered()), this, SLOT(showExceptionalRes()));
        
    actionLowScoreSegParams = new QAction(tr("Low-Scoring Segment Parameters"), this);
    connect(actionLowScoreSegParams, SIGNAL(triggered()), this, SLOT(lowScoreSegParams()));
        
    actionColumnScoreParams = new QAction(tr("Column Score Parameters"), this);
    connect(actionColumnScoreParams, SIGNAL(triggered()), this, SLOT(columnScoreParams()));
        
    actionSaveColumnScoresToFile = new QAction(tr("Save Column Scores to File"), this);  
    connect(actionSaveColumnScoresToFile, SIGNAL(triggered()), this,
            SLOT(saveColumnScoresToFile()));
            
    /*
     * Help Menu
     */
    actionGeneralHelp = new QAction(tr("General"), this);
    connect(actionGeneralHelp, SIGNAL(triggered()), this, SLOT(generalHelp()));
    
    actionInputOutputHelp = new QAction(tr("Input && Output Files"), this);//&&
    connect(actionInputOutputHelp, SIGNAL(triggered()), this, SLOT(inputOutputHelp()));
    
    actionEditAlignHelp = new QAction(tr("Editing Alignments"), this);
    connect(actionEditAlignHelp, SIGNAL(triggered()), this, SLOT(editAlignHelp()));
    
    actionMultiAlignHelp = new QAction(tr("Multiple Alignments"), this);
    connect(actionMultiAlignHelp, SIGNAL(triggered()), this, SLOT(multiAlignHelp()));
    
    actionProfileAlignHelp = new QAction(tr("Profile Alignments"), this);
    connect(actionProfileAlignHelp, SIGNAL(triggered()), this, SLOT(profileAlignHelp()));
    
    actionSecondaryStructureHelp = new QAction(tr("Secondary Structures"), this);
    connect(actionSecondaryStructureHelp, SIGNAL(triggered()), this,
            SLOT(secondaryStructureHelp()));
    
    actionTreesHelp = new QAction(tr("Trees"), this);
    connect(actionTreesHelp, SIGNAL(triggered()), this, SLOT(treesHelp()));
    
    actionColorsHelp = new QAction(tr("Colors"), this);
    connect(actionColorsHelp, SIGNAL(triggered()), this, SLOT(colorsHelp()));
    
    actionAlignQualityHelp = new QAction(tr("Alignment Quality"), this);
    connect(actionAlignQualityHelp, SIGNAL(triggered()), this, SLOT(alignQualityHelp()));
    
    actionCommandLineHelp = new QAction(tr("Command Line Parameters"), this);
    connect(actionCommandLineHelp, SIGNAL(triggered()), this, SLOT(commandLineHelp()));
    
    actionReferencesHelp = new QAction(tr("References"), this);
    connect(actionReferencesHelp, SIGNAL(triggered()), this, SLOT(referencesHelp()));
}

/**
 * This function creates the menus and adds the actions to the appropriate menu.
 */
void MainWindow::createMenus()
{
    /* 
     * Set up the file menu 
     */
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(actionLoadSequences);
    fileMenu->addAction(actionAppendSequences);
    fileMenu->addAction(actionSaveSeqAs);
    fileMenu->addAction(actionLoadProfile1);
    fileMenu->addAction(actionLoadProfile2);
    fileMenu->addAction(actionSaveProfile1As);
    fileMenu->addAction(actionSaveProfile2As);
    fileMenu->addAction(actionWriteAlignmentAsPost);
    fileMenu->addAction(actionWriteProfile1AsPost);
    fileMenu->addAction(actionWriteProfile2AsPost);
    fileMenu->addAction(actionQuit);
    toplevelMenuList.push_back(fileMenu);
    
    /* 
     * Set up the Edit menu 
     */
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(actionCutSequences);
    editMenu->addAction(actionPasteSequences);
    editMenu->addAction(actionSelectAllSequences);
    editMenu->addAction(actionSelectProfile1);
    editMenu->addAction(actionSelectProfile2);
    editMenu->addAction(actionAddProfile2ToProfile1);
    editMenu->addAction(actionClearSequenceSelection);
    editMenu->addAction(actionClearRangeSelection);
    editMenu->addAction(actionSearchForString);
    editMenu->addAction(actionRemoveAllGaps);
    editMenu->addAction(actionRemoveGapOnlyColumns);
    toplevelMenuList.push_back(editMenu);

    /* 
     * Set up the Alignment Menu 
     */
    alignmentMenu = menuBar()->addMenu(tr("&Alignment"));
    alignmentMenu->addAction(actionDoCompleteAlign);
    alignmentMenu->addAction(actionDoGuideTreeOnly);
    alignmentMenu->addAction(actionDoAlignmentFromGuideTree);
    alignmentMenu->addAction(actionRealignSelectSeq);
    alignmentMenu->addAction(actionRealignSelectRange);
    alignmentMenu->addAction(actionAlignProfile1ToProfile2);
    alignmentMenu->addAction(actionAlignProfileFromTree);
    alignmentMenu->addAction(actionAlignSequencesToProfile1);
    alignmentMenu->addAction(actionAlignSequencesToProfile1FromTree);
    alignmentMenu->addMenu(menuAlignParameters);
    alignmentMenu->addMenu(iterationSubMenu); // Mark 16-5-07
    alignmentMenu->addAction(actionOutputFormatOptions);
    alignmentMenu->addAction(actionSetDefaultParameters);
    toplevelMenuList.push_back(alignmentMenu);

    /* 
     * Set up the Tree menu 
     */
    treeMenu = menuBar()->addMenu(tr("&Trees"));
    treeMenu->addAction(actionDrawNJTree);
    treeMenu->addAction(actionBootstrapNJTree);
    treeMenu->addAction(actionExcludePosWithGap);
    treeMenu->addAction(actionCorrectForMultiSub);
    treeMenu->addAction(actionTreeOutputFormatOptions);
    treeMenu->addMenu(treeSubMenu); // Mark 16-5-07
    toplevelMenuList.push_back(treeMenu);

    /*
     * Set up the color menu 
     */
    colorMenu = menuBar()->addMenu(tr("&Colors"));
    colorMenu->addAction(actionBackColoring);
    colorMenu->addAction(actionBlackAndWhite);
    colorMenu->addAction(actionDefaultColors);
    colorMenu->addAction(actionLoadColorParameterFile);
    toplevelMenuList.push_back(colorMenu);

    qualityMenu = menuBar()->addMenu(tr("&Quality"));
    qualityMenu->addAction(actionShowLowScoreSeg);
    qualityMenu->addAction(actionShowExceptionalRes);
    qualityMenu->addAction(actionLowScoreSegParams);
    qualityMenu->addAction(actionColumnScoreParams);
    qualityMenu->addAction(actionSaveColumnScoresToFile);
    toplevelMenuList.push_back(qualityMenu);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(actionGeneralHelp);
    helpMenu->addAction(actionInputOutputHelp);
    helpMenu->addAction(actionEditAlignHelp);
    helpMenu->addAction(actionMultiAlignHelp);
    helpMenu->addAction(actionProfileAlignHelp);
    helpMenu->addAction(actionSecondaryStructureHelp);
    helpMenu->addAction(actionTreesHelp);
    helpMenu->addAction(actionColorsHelp);
    helpMenu->addAction(actionAlignQualityHelp);
    helpMenu->addAction(actionCommandLineHelp);
    helpMenu->addAction(actionReferencesHelp);
    toplevelMenuList.push_back(helpMenu);
        
}

/**
 * This function sets up the Alignment mode drop down menu. 
 */
void MainWindow::createAlignModeMenu() 
{
    alignmentModeMenu->addItem("Multiple Alignment Mode");
    alignmentModeMenu->addItem("Profile Alignment Mode");
    alignmentModeMenu->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    profileMode = 1;
    multiMode = 0;
    connect(alignmentModeMenu, SIGNAL(activated(int)), this,
            SLOT(changeMode(int)));
}

void MainWindow::changeMode(int mode)
{
    if(mode != currentMode)
    {
        if(mode == profileMode)
        {
            profile2Viewer->setVisible(true);
            profile2Viewer->setProfileNum(2);
            alignmentViewer->setProfileNum(1);
            profileScrollLock->setVisible(true);
            enableProfileActions();
            if(alignPtr->getNumSeqs() > 0)
            {
                clustalw::userParameters->setProfileNum(1);
                clustalw::userParameters->setProfile1Empty(false);
                alignPtr->setProfile1NumSeqs(alignPtr->getNumSeqs());
                alignmentViewer->updateView(1, alignPtr->getNumSeqs()); // View all seqs
                profile2Viewer->updateView(0, 0);
            } 
        }
        else // sequence align mode!!
        {
            clustalw::userParameters->setProfileNum(0);
            alignPtr->setProfile1NumSeqs(0);
            clustalw::userParameters->setProfile1Empty(true);
            clustalw::userParameters->setProfile2Empty(true);
            alignmentViewer->setProfileNum(0);
            profile2Viewer->setProfileNum(0);

            if(alignPtr->getNumSeqs() > 0)
            {            
                alignmentViewer->updateView(1, alignPtr->getNumSeqs());
            }
            else
            {
                alignmentViewer->updateView(0, 0);
            }
            profile2Viewer->updateView(0, 0);
            profile2Viewer->setVisible(false);
            profileScrollLock->setVisible(false);
            enableSequenceActions();
            
            // If it is showing the low scoring segments!!!
            if(showLowScoreSegments)
            {
                // Stop the profile2Viewer from showing it.
                seqWeightCalculatedProfile2Viewer = false;
                lowScoreSeqWeightProfile2Viewer->clear();
                lowScoreResProfile2Viewer->clearArray();
                profile2Viewer->showLowScoringSegments(false, lowScoreResProfile2Viewer);
                // Recalculculate the alignment one.
                calculateLowScoreAlign();
            }
        }
        currentMode = mode;
        //hideLowScoreSegs(); // Mark 22-5-2007
        resetAndHideLowScoreSeqs(); // Mark 23-5-2007
        update();
    }
}

void MainWindow::setProfileModeCmd()
{
    alignmentViewer->setProfileNum(1);
    profile2Viewer->setProfileNum(2);
    profile2Viewer->setVisible(true);
    profileScrollLock->setVisible(true);
    enableProfileActions();
/*    if(alignPtr->getNumSeqs() > 0)
    {
        clustalw::userParameters->setProfile1Empty(false);
        clustalw::userParameters->setProfile2Empty(false);
        alignPtr->setProfile1NumSeqs(alignPtr->getNumSeqs());
        alignmentViewer->updateView(1, alignPtr->getNumSeqs()); // View all seqs

        profile2Viewer->updateView(alignPtr->getProfile1NumSeqs() + 1,
            alignPtr->getNumSeqs());    
    } 
    //hideLowScoreSegs(); // Mark 22-5-2007
    resetAndHideLowScoreSeqs(); // Mark 23-5-2007 */
    currentMode = profileMode;
    update();
}

/**
 * This function enables the profile actions and disables the sequence ones. 
 */
void MainWindow::enableProfileActions()
{
    // disabled
    actionLoadSequences->setEnabled(false);
    actionAppendSequences->setEnabled(false);
    actionSaveSeqAs->setEnabled(false);
    actionWriteAlignmentAsPost->setEnabled(false);
    actionSelectAllSequences->setEnabled(false);
    actionDoCompleteAlign->setEnabled(false);
    actionDoGuideTreeOnly->setEnabled(false);
    actionDoAlignmentFromGuideTree->setEnabled(false);
    actionRealignSelectSeq->setEnabled(false);
    actionRealignSelectRange->setEnabled(false);
    actionDrawNJTree->setEnabled(false);
    actionBootstrapNJTree->setEnabled(false);
    actionExcludePosWithGap->setEnabled(false);
    actionCorrectForMultiSub->setEnabled(false);
    actionTreeOutputFormatOptions->setEnabled(false);
    
    // Enabled     
    actionSecondaryStructureParameters->setEnabled(true);  
    actionLoadProfile1->setEnabled(true);
    actionLoadProfile2->setEnabled(true);
    actionSaveProfile1As->setEnabled(true);
    actionSaveProfile2As->setEnabled(true);
    actionWriteProfile1AsPost->setEnabled(true);
    actionWriteProfile2AsPost->setEnabled(true);
    actionSelectProfile1->setEnabled(true);
    actionSelectProfile2->setEnabled(true);
    actionAddProfile2ToProfile1->setEnabled(true);
    actionAlignProfile1ToProfile2->setEnabled(true);
    actionAlignProfileFromTree->setEnabled(true);
    actionAlignSequencesToProfile1->setEnabled(true);
    actionAlignSequencesToProfile1FromTree->setEnabled(true);        
}

/**
 * This function enables the sequence actions and disables the profile ones. 
 */
void MainWindow::enableSequenceActions()
{
    // enabled
    actionLoadSequences->setEnabled(true);
    actionAppendSequences->setEnabled(true);
    actionSaveSeqAs->setEnabled(true);
    actionWriteAlignmentAsPost->setEnabled(true);
    actionSelectAllSequences->setEnabled(true);    
    actionDoCompleteAlign->setEnabled(true);
    actionDoGuideTreeOnly->setEnabled(true);
    actionDoAlignmentFromGuideTree->setEnabled(true);
    actionRealignSelectSeq->setEnabled(true);
    actionRealignSelectRange->setEnabled(true);
    actionDrawNJTree->setEnabled(true);
    actionBootstrapNJTree->setEnabled(true);
    actionExcludePosWithGap->setEnabled(true);
    actionCorrectForMultiSub->setEnabled(true);
    actionTreeOutputFormatOptions->setEnabled(true);    
    
    // disabled
    actionLoadProfile1->setEnabled(false);
    actionLoadProfile2->setEnabled(false);
    actionSaveProfile1As->setEnabled(false);
    actionSaveProfile2As->setEnabled(false);
    actionWriteProfile1AsPost->setEnabled(false);
    actionWriteProfile2AsPost->setEnabled(false);
    actionSelectProfile1->setEnabled(false);
    actionSelectProfile2->setEnabled(false);
    actionAddProfile2ToProfile1->setEnabled(false);
    actionAlignProfile1ToProfile2->setEnabled(false);
    actionAlignProfileFromTree->setEnabled(false);
    actionAlignSequencesToProfile1->setEnabled(false);
    actionAlignSequencesToProfile1FromTree->setEnabled(false);
    actionSecondaryStructureParameters->setEnabled(false); 
}


/**
 * This function sets up the font size drop down menu. 
 */
void MainWindow::createFontMenu() 
{
    // Mark: 19-1-2007 added more items to the font combobox.
    // Nige: 13-2-2007 added more items to the font combobox.
    // Nige: 15-3-2007 changed to classical typographic scalings
    fontMenu->addItem("72");
    fontMenu->addItem("60");
    fontMenu->addItem("48");
    fontMenu->addItem("36");
    //fontMenu->addItem("30"); //-
    fontMenu->addItem("28");
    //fontMenu->addItem("26"); //-
    fontMenu->addItem("24");
    //fontMenu->addItem("22"); //-
    fontMenu->addItem("21");   //-20
    fontMenu->addItem("18");    
    fontMenu->addItem("16");
    fontMenu->addItem("14");
    fontMenu->addItem("12");
    fontMenu->addItem("11");
    fontMenu->addItem("10");
    fontMenu->addItem("9");
#if !OS_MAC
    fontMenu->addItem("8");
#endif
    fontMenu->setCurrentIndex(12);
    fontMenu->setSizeAdjustPolicy(QComboBox::AdjustToContents);
}

/**
 * This function creates the layout for the horizontal area containing the alignment mode
 * and font size drop down menu.
 */
void MainWindow::createHorizontalBox()
{
    /* 
     * Drop down Menus 
     */
    alignmentModeMenu = new QComboBox(); 
    createAlignModeMenu();
    fontMenu = new QComboBox();
    createFontMenu();
    
    profileScrollLock = new QCheckBox("Lock Scroll");
    profileScrollLock->setVisible(false);
    profileScrollLock->setChecked(false);
    connect(profileScrollLock, SIGNAL(clicked()), this, SLOT(scrollLockClicked()));    
    /* 
     * Labels for the menus 
     */
    fontLabel = new QLabel("&Font:", this);
    alignModeLabel = new QLabel("&Mode:", this);
    fontLabel->setMaximumWidth ( 40 );
    alignModeLabel->setMaximumWidth ( 40 ); 
    fontLabel->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum);
    alignModeLabel->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum); 
    fontLabel->setBuddy(fontMenu);
    alignModeLabel->setBuddy(alignmentModeMenu);

    /*
     * Fill the space not taken up by the other widgets.
     */
    sideFiller = new QWidget;
    sideFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    /* 
     * Create a horizontal group box and add the widgets to it. 
     */
    horizontalGroupBox = new QGroupBox();
    layoutHbox = new QHBoxLayout;
    layoutHbox->addWidget(alignModeLabel);
    layoutHbox->addWidget(alignmentModeMenu);
    layoutHbox->addWidget(fontLabel);
    layoutHbox->addWidget(fontMenu);
    layoutHbox->addWidget(profileScrollLock);
    layoutHbox->addWidget(sideFiller);
    layoutHbox->setDirection (QBoxLayout::LeftToRight);
    horizontalGroupBox->setLayout(layoutHbox);
}

void MainWindow::scrollLockClicked()
{
    QScrollBar* alignmentBar = alignmentViewer->getAlignmentScrollBar();
    QScrollBar* profileBar = profile2Viewer->getAlignmentScrollBar();
    
    if(profileScrollLock->isChecked())
    {
        // Connect the two scrollbars together, make the one with the shortest seqs invisible
        int alignmentLongest = alignmentViewer->getLengthLongestSeqInRange();
        int profileLongest = profile2Viewer->getLengthLongestSeqInRange();
        
        connect(alignmentBar, SIGNAL(valueChanged(int)),
                profileBar, SLOT(setValue(int)));
        connect(profileBar, SIGNAL(valueChanged(int)),
                alignmentBar,SLOT(setValue(int)));
                    
        if(alignmentLongest > profileLongest)
        {
            // Make the profile one invisible
            profileBar->setVisible(false);
            alignmentBar->setVisible(true);
        }
        else
        {
            // Make the alignment one invisble
            alignmentBar->setVisible(false);
            profileBar->setVisible(true);
        }
    }
    else
    {
        profileBar->setVisible(true);
        alignmentBar->setVisible(true);
        disconnect(alignmentBar, SIGNAL(valueChanged(int)),
                profileBar, SLOT(setValue(int)));
        disconnect(profileBar, SIGNAL(valueChanged(int)),
                alignmentBar,SLOT(setValue(int)));        
    }
}

void MainWindow::loadSequences(QString myFile)
{
    KeyController::Instance()->clearCtrl(); //- nige
    string offendingSeq; // needed in case one sequence is reported as troublemaker
    bool dnaFlagBefore = clustalw::userParameters->getDNAFlag();
    
    int errorCode;

    // Mark Jan 18th 2007: changes to remember working Dir
    FileDialog fd;



    /* Andreas Wilm (UCD):
     * if file provided on commandline didn't exist, clustalx 1.83
     * reported this via gui and exited after that.  v2 reports
     * missing file, but doesn't exit
     */
    if(myFile == "") {
        myFile = fd.getOpenDataFileName(this, tr("Load Sequence File"));
        if(myFile == "")
        {
            return;
        }
    }

    
    clustalw::userParameters->setSeqName(myFile.toStdString());
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    errorCode = clustalObj->sequenceInput(false, &offendingSeq);

    // The view is updated anyway, even if there has been an error.  
    seqWeightCalculatedAlignmentViewer = false;
    bool dnaFlagAfter = clustalw::userParameters->getDNAFlag();
    
    if(dnaFlagBefore != dnaFlagAfter)
    {
        if(dnaFlagAfter == true)
        {
            // Moved from protein to dna, use dna file
            alignmentViewer->setColorScheme(&dnaColors);
            profile2Viewer->setColorScheme(&dnaColors);
        }
        else
        {
            alignmentViewer->setColorScheme(&proteinColors);
            profile2Viewer->setColorScheme(&proteinColors);
        }
    }
    
    if(errorCode != clustalw::OK)
    {
        sequenceFileName = "";
        alignmentViewer->updateView(0, 0);
        seqWeightCalculatedAlignmentViewer = false;
        lowScoreSeqWeightAlignmentViewer->clear();
        lowScoreResAlignmentViewer->clearArray();        
        
        errorHandler(errorCode, offendingSeq);
    }
    else
    {
        sequenceFileName = myFile;
        alignmentViewer->updateView(1, alignPtr->getNumSeqs());
        if(showLowScoreSegments == true)
        {
            calculateLowScoreAlign();
        }     
    }
    searchBeginSeq = 0;
    searchBeginRes = 1;
    QApplication::restoreOverrideCursor();    
}

void MainWindow::appendSequences()
{
    int errorCode;
    QString myFile;
    string offendingSeq;
    // Mark Jan 18th 2007: changes to remember working Dir
    
    FileDialog fd;
    myFile = fd.getOpenDataFileName(this, tr("Append Sequence File"));    
    
    if(myFile == "")
    {
        return;
    }    
                                           
    clustalw::userParameters->setSeqName(myFile.toStdString());

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));    
    errorCode = clustalObj->sequenceInput(true, &offendingSeq);
    alignmentViewer->updateView(1, alignPtr->getNumSeqs());
    
    seqWeightCalculatedAlignmentViewer = false;
    
    if(errorCode != clustalw::OK)
    {
        errorHandler(errorCode, offendingSeq);
    }
    else
    {
        if(showLowScoreSegments == true)
        {        
            calculateLowScoreAlign();
        }
    }
    searchBeginSeq = 0;
    searchBeginRes = 1;
    QApplication::restoreOverrideCursor();        
}

void MainWindow::saveSequencesAs()
{
    KeyController::Instance()->clearCtrl(); //- nige

    alignmentViewer->saveSequences(sequenceFileName, Sequences);
}

void MainWindow::loadProfile1()
{
    bool dnaFlagBefore = clustalw::userParameters->getDNAFlag();
    
    int errorCode = 0;
    QString myFile = "";
    
    // Mark Jan 18th 2007: changes to remember working Dir
    FileDialog fd;
    myFile = fd.getOpenDataFileName(this, tr("Load Profile 1"));    
    
    if(myFile == "")
    {
        return;
    }       
                                       
    alignmentViewer->updateView(0, 0);
    profile2Viewer->updateView(0, 0);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));    
    errorCode = clustalObj->profile1Input(myFile.toStdString());    
    
    bool dnaFlagAfter = clustalw::userParameters->getDNAFlag();
    
    seqWeightCalculatedAlignmentViewer = false;
    
    // Check if we need to reread in 
    if(dnaFlagBefore != dnaFlagAfter)
    {
        // We have changed from dna to protein or vice versa
        // Need to reread the parameter file;
        if(dnaFlagAfter == true)
        {
            // Moved from protein to dna, use dna file
            alignmentViewer->setColorScheme(&dnaColors);
            profile2Viewer->setColorScheme(&dnaColors);
        }
        else
        {
            alignmentViewer->setColorScheme(&proteinColors);
            profile2Viewer->setColorScheme(&proteinColors);
        }
    }
        
    if(errorCode != clustalw::OK)
    {
        sequenceFileName = "";
        alignmentViewer->updateView(0, 0);
        seqWeightCalculatedAlignmentViewer = false;
        lowScoreSeqWeightAlignmentViewer->clear();
        lowScoreResAlignmentViewer->clearArray();        
        
        errorHandler(errorCode);
    }
    else
    {
        sequenceFileName = myFile;
        alignmentViewer->updateView(1, alignPtr->getNumSeqs());
        if(showLowScoreSegments == true)
        {
            calculateLowScoreAlign();
        }
    }
    searchBeginSeq = 0;
    searchBeginRes = 1; 
    QApplication::restoreOverrideCursor();                                          
}

void MainWindow::loadProfile2()
{
    int errorCode;
    if(!clustalw::userParameters->getProfile1Empty())
    {
        QString myFile;   
        
        // Mark Jan 18th 2007: changes to remember working Dir
        FileDialog fd;
        myFile = fd.getOpenDataFileName(this, tr("Load Profile 2"));        
            
        if(myFile == "")
        {
            return;
        }
                        
        profile2Viewer->updateView(0, 0);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));      
        errorCode = clustalObj->profile2Input(myFile.toStdString());   
        
        seqWeightCalculatedProfile2Viewer = false;                            
        
        if(errorCode != clustalw::OK)
        {
            profile2FileName = "";
            profile2Viewer->updateView(0, 0);
            if(errorCode == clustalw::ALLNAMESNOTDIFFERENT)
            {
                /* 26-03-07,nige: display Profile1 intact; reset Profile2 */
                alignmentViewer->updateView();     //- nige
                profile2Viewer->updateView(0, 0);  //- nige
                
                seqWeightCalculatedAlignmentViewer = false;
                lowScoreSeqWeightAlignmentViewer->clear();
                lowScoreResAlignmentViewer->clearArray();
            }
            errorHandler(errorCode);
            
            seqWeightCalculatedProfile2Viewer = false;
            lowScoreSeqWeightProfile2Viewer->clear();
            lowScoreResProfile2Viewer->clearArray();          
        }
        else
        {
            profile2FileName = myFile;
            profile2Viewer->updateView(alignPtr->getProfile1NumSeqs() + 1,
                                       alignPtr->getNumSeqs());
            // If we are showing the low scoring segments then calculate for the 2nd profile
            if(showLowScoreSegments == true)
            {
                calculateLowScoreProfile();
            }
        }
        QApplication::restoreOverrideCursor();        
    }
    else
    {
        QMessageBox::information(this, "", "You must load Profile 1 first", QMessageBox::Ok);
    }
    searchBeginSeq = 0;
    searchBeginRes = 1;    
}

void MainWindow::loadProfile1(string myFile)
{
    bool dnaFlagBefore = clustalw::userParameters->getDNAFlag();
    int errorCode = 0;
                                       
    alignmentViewer->updateView(0, 0);
    profile2Viewer->updateView(0, 0);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));    
    errorCode = clustalObj->profile1Input(myFile);    
    
    bool dnaFlagAfter = clustalw::userParameters->getDNAFlag();
    
    seqWeightCalculatedAlignmentViewer = false;
    
    // Check if we need to reread in 
    if(dnaFlagBefore != dnaFlagAfter)
    {
        // We have changed from dna to protein or vice versa
        // Need to reread the parameter file;
        if(dnaFlagAfter == true)
        {
            // Moved from protein to dna, use dna file
            alignmentViewer->setColorScheme(&dnaColors);
            profile2Viewer->setColorScheme(&dnaColors);
        }
        else
        {
            alignmentViewer->setColorScheme(&proteinColors);
            profile2Viewer->setColorScheme(&proteinColors);
        }
    }
        
    if(errorCode != clustalw::OK)
    {
        sequenceFileName = "";
        alignmentViewer->updateView(0, 0);
        seqWeightCalculatedAlignmentViewer = false;
        lowScoreSeqWeightAlignmentViewer->clear();
        lowScoreResAlignmentViewer->clearArray();        
        
        errorHandler(errorCode);
    }
    else
    {
        alignmentViewer->updateView(1, alignPtr->getNumSeqs());
        if(showLowScoreSegments == true)
        {
            calculateLowScoreAlign();
        }
    }
    searchBeginSeq = 0;
    searchBeginRes = 1; 
    QApplication::restoreOverrideCursor();                                          
}

void MainWindow::loadProfile2(string myFile)
{
    int errorCode;
    if(!clustalw::userParameters->getProfile1Empty())
    {
        profile2Viewer->updateView(0, 0);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));      
        errorCode = clustalObj->profile2Input(myFile);   
        
        seqWeightCalculatedProfile2Viewer = false;                            
        
        if(errorCode != clustalw::OK)
        {
            profile2FileName = "";
            profile2Viewer->updateView(0, 0);
            if(errorCode == clustalw::ALLNAMESNOTDIFFERENT)
            {
                /* 26-03-07,nige: display Profile1 intact; reset Profile2 */
                alignmentViewer->updateView();     //- nige
                profile2Viewer->updateView(0, 0);  //- nige
                
                seqWeightCalculatedAlignmentViewer = false;
                lowScoreSeqWeightAlignmentViewer->clear();
                lowScoreResAlignmentViewer->clearArray();
            }
            errorHandler(errorCode);
            
            seqWeightCalculatedProfile2Viewer = false;
            lowScoreSeqWeightProfile2Viewer->clear();
            lowScoreResProfile2Viewer->clearArray();          
        }
        else
        {
            profile2Viewer->updateView(alignPtr->getProfile1NumSeqs() + 1,
                                       alignPtr->getNumSeqs());
            // If we are showing the low scoring segments then calculate for the 2nd profile
            if(showLowScoreSegments == true)
            {
                calculateLowScoreProfile();
            }
        }
        QApplication::restoreOverrideCursor();        
    }
    else
    {
        QMessageBox::information(this, "", "You must load Profile 1 first", QMessageBox::Ok);
    }
    searchBeginSeq = 0;
    searchBeginRes = 1;    
}

void MainWindow::saveProfile1As()
{
    if(!clustalw::userParameters->getProfile1Empty())
    {
        alignmentViewer->saveSequences(sequenceFileName, Profile1);
    }
    else
    {
        QMessageBox::information(this, "", "Load Profile 1 first.", QMessageBox::Ok);
    }
}

void MainWindow::saveProfile2As()
{
    if(!clustalw::userParameters->getProfile2Empty())
    {
        profile2Viewer->saveSequences(profile2FileName, Profile2);
    }
    else
    {
        QMessageBox::information(this, "", "Load Profile 2 first.", QMessageBox::Ok);
    }
}

void MainWindow::writeAlignmentAsPostscript()
{
    KeyController::Instance()->clearCtrl(); //- nige

    writeViewer1AsPostscript(Sequences);   
}

void MainWindow::writeProfile1AsPostscript()
{
    writeViewer1AsPostscript(Profile1);
}

void MainWindow::writeViewer1AsPostscript(int type)
{
    if(alignmentViewer->seqsLoaded())
    {
        int lengthLongestSeq = alignmentViewer->getLengthLongestSeqInRange();
        
        WritePostscriptFile writePs(sequenceFileName, Sequences, lengthLongestSeq);
        writePs.exec(); 
    
        // Need to parse the color parameters file.
        if(writePs.result() == QDialog::Accepted)
        {
            PostscriptFileParams psParams = writePs.getParams();
            ColorParameters* ptrToCur;
            ptrToCur = alignmentViewer->getPtrToCurrentColorScheme();
            
            // We want to have the same rules as the current selection, but different
            // colors, as the colors may display differently when printed.

            //26-11-07,nige
            //readInParamFile(psParams.getPSColorsFile(), &printerColors, false);
            readInParamFile(psParams.getPSColorsFile(), &printerColors);
            //26-11-07,nige: end

            

            printerColors.addAllRulesTogether(ptrToCur->getColorRules());
            printerColors.addConsensusParams(*(ptrToCur->getConPar()));
            
            alignmentViewer->setPrintoutColorScheme(printerColors);
            profile2Viewer->setPrintoutColorScheme(printerColors);
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));             
            alignmentViewer->writeToPSFile(psParams);
            QApplication::restoreOverrideCursor();            
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), 
                tr("Error: No file loaded."));
    }
}

void MainWindow::writeProfile2AsPostscript()
{   
    if(profile2Viewer->seqsLoaded())
    {
        int lengthLongestSeq = profile2Viewer->getLengthLongestSeqInRange();
        
        WritePostscriptFile writePs(profile2FileName, Profile2, lengthLongestSeq);
        writePs.exec(); 
    
        // Need to parse the color parameters file.
        if(writePs.result() == QDialog::Accepted)
        {
            PostscriptFileParams psParams = writePs.getParams();
            ColorParameters* ptrToCur;
            ptrToCur = profile2Viewer->getPtrToCurrentColorScheme();
            // We want to have the same rules as the current selection, but different
            // colors, as the colors may display differently when
            // printed.
            
            //26-11-07,nige
            //readInParamFile(psParams.getPSColorsFile(), &printerColors, false);
            readInParamFile(psParams.getPSColorsFile(), &printerColors);
            //26-11-07,nige: end
            
            printerColors.addAllRulesTogether(ptrToCur->getColorRules());
            printerColors.addConsensusParams(*(ptrToCur->getConPar()));
            
            alignmentViewer->setPrintoutColorScheme(printerColors);
            profile2Viewer->setPrintoutColorScheme(printerColors);
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));             
            profile2Viewer->writeToPSFile(psParams);
            QApplication::restoreOverrideCursor();            
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), 
                tr("Error: No file loaded."));
    }
}

void MainWindow::quitProgram()
{
    exit(0);
}


bool MainWindow::anySeqsSelected()
{
    if(alignmentViewer->getNumberOfSeqsSelected() != 0)
    {
        return true;
    }
    if((currentMode == profileMode) && profile2Viewer->getNumberOfSeqsSelected() != 0)
    {
        return true;
    }
    return false;
}

void MainWindow::cutSequences()
{
    KeyController::Instance()->clearCtrl(); //- nige

    // Check if a file is loaded
    int numSeqs = alignPtr->getNumSeqs();
    if(numSeqs == 0)
    {
        QMessageBox::information(this, "", "No file loaded.", QMessageBox::Ok);
        return;
    }
    
    // check if some seqs are selected
    bool seqsSelected = anySeqsSelected();
    
    if(!seqsSelected)
    {
        QString message = "Select sequences to be cut by clicking on the names.";
        QMessageBox::information(this, "", message, QMessageBox::Ok);
        return;    
    }    
    
    // check if we have multi or profile mode
    std::vector<int> selected;
    selected = getAllSelectedSeqsVector();

    bool allSelected = allSeqsInAlignmentOrProfileSelected(&selected);
    if(allSelected)
    {
        QString message = "Cannot cut all sequences from an alignment or profile.";
        QMessageBox::information(this, "", message, QMessageBox::Ok);
        return;    
    }
    
    cutSeqs.clear();
    cutSeqs = alignPtr->cutSelectedSequencesFromAlignment(&selected);
    numSeqs = alignPtr->getNumSeqs();
    int profileNum = clustalw::userParameters->getProfileNum();
    
    if(currentMode == profileMode)
    {
        int numSeqsProfile1 = alignPtr->getProfile1NumSeqs();
        int prof1First = 0, prof1Last = 0, prof2First = 0, prof2Last = 0;
        
        if(numSeqs <= 0)
        {   
            clustalw::userParameters->setEmpty(true);
            prof1First = 0;
            prof1Last = 0;
            prof2First = 0;
            prof2Last = 0;            
        }        
        else if(numSeqsProfile1 == 0)
        {
            clustalw::userParameters->setProfile1Empty(true);
            prof1First = 0;
            prof1Last = 0;
            if(profileNum == 2)
            {
                prof2First = 1;
                prof2Last = numSeqs;
            }
        }
        else
        {
            prof1First = 1;
            prof1Last = numSeqsProfile1;
            if(profileNum == 2)
            {
                if(numSeqsProfile1 < numSeqs)
                {
                    prof2First = numSeqsProfile1 + 1;
                    prof2Last = numSeqs;
                }
                else // None left in profile 2
                {
                    prof2First = 0;
                    prof2Last = 0;
                }
            } 
        }
        alignmentViewer->updateView(prof1First, prof1Last);
        if(profileNum == 2)
        {
            profile2Viewer->updateView(prof2First, prof2Last); 
        }       
    }
    else
    {
        //int alignmentFirstSeq, alignmentLastSeq;
        if(numSeqs <= 0)
        {   
            clustalw::userParameters->setEmpty(true);
            alignmentViewer->updateView(0, 0);
        }
        else
        {
            alignmentViewer->updateView(1, numSeqs);
        }        
    }
    //hideLowScoreSegs(); // Mark 22-5-2007
    resetAndHideLowScoreSeqs();  // Mark 23-5-2007
}

bool MainWindow::allSeqsInAlignmentOrProfileSelected(std::vector<int>* seqsSelected)
{
    if(currentMode == profileMode)
    {
        int numProfile1 = alignPtr->getProfile1NumSeqs();
        int sizeSelected = seqsSelected->size();
        bool allSeqsInProf1Selected = false, allSeqsInProf2Selected = false;
        int numSelectedInProf1 = 0, numSelectedInProf2 = 0;
        
        // Mark Jan 17th 2007. This section of the code has changed to fix the problem in
        // bug 2.
        for(int i = 1; i <= numProfile1; i++)
        {
            if((*seqsSelected)[i] == 1)
            {
                numSelectedInProf1++;
            }
        }
        if(numSelectedInProf1 == numProfile1)
        {
            allSeqsInProf1Selected = true;
        }
        
        int numSeqsInProf2 = (sizeSelected - 1) - numProfile1;
        if(numSeqsInProf2 != 0)
        {
            for(int i = numProfile1 + 1; i < sizeSelected; i++)
            {
                if((*seqsSelected)[i] == 1)
                {
                    numSelectedInProf2++;
                }
            }
            if(numSelectedInProf2 == numSeqsInProf2)
            {
                allSeqsInProf2Selected = true;
            }
        }
        
        return (allSeqsInProf1Selected || allSeqsInProf2Selected);        
    }
    else
    {
        int sizeSelected = seqsSelected->size();
        bool allSeqsInAlignSelected = true;
        for(int i = 1; i < sizeSelected; i++)
        {
            if((*seqsSelected)[i] != 1)
            {
                allSeqsInAlignSelected = false;
            }
        } 
        return allSeqsInAlignSelected;           
    }
}

void MainWindow::pasteSequences()
{
    KeyController::Instance()->clearCtrl(); //- nige

    if(cutSeqs.size() == 0)
    {
        QString message = "No sequences available for pasting.\n";
        message += "Cut sequences first.";
        QMessageBox::information(this, "", message, QMessageBox::Ok);
        return;
    }
    
    int insert=0; 
    bool insertIntoProfile2 = false;   
    // Check if a seq has been selected!
    if(alignPtr->getNumSeqs() == 0)
    {
        insert = 0;
    }
    else
    {
        // Mark Jan 16th 2007. Bug 11
        // I have made some changes here to allow pasting into the empty profile 2.
        
        bool seqsSelected = anySeqsSelected();
        bool hasProfile2NamesFocus = profile2Viewer->hasFocus();
        
        // Find the point to paste the sequences
        vector<int> selected;
        selected = getAllSelectedSeqsVector(); 
        
        if(seqsSelected)
        {
            //paste after *first sequence in selected block*
            //for(int i = 1; i < selected.size(); i++) {

            //- nige: always paste *after selected block* regardless of
            //- length; this gives consistent behaviour for block of length 1
            //- or more.
            for (int i = selected.size()-1; i > 0; i--) {
                if (selected[i] == 1) {
                    //cout << "ins: " << selected.size() << "," << i << endl;
                    insert = i;
                    break;
                }
            }
        }
        else if(!seqsSelected && currentMode == profileMode && hasProfile2NamesFocus)
        {
            // Mark Jan 16th 2007
            // This is to allow pasting into the empty profile2
            insert = selected.size() - 1; // Insert at the end
            profile2FileName = "nofilepastedin";
            clustalw::userParameters->setProfileNum(2); 
            clustalw::userParameters->setProfile2Name("nofilepastedin");
            clustalw::userParameters->setProfile2Empty(false);
            insertIntoProfile2 = true;
        }
        else
        {
            QString message = "Select a sequence by clicking on the name.\n";
            message += "Cut sequences will be pasted after this one.";
            QMessageBox::information(this, "", message, QMessageBox::Ok);
            return;        
        }   
    }
    
    alignPtr->pasteSequencesIntoPosition(&cutSeqs, insert, insertIntoProfile2);
    cutSeqs.clear();    
    int numSeqs = alignPtr->getNumSeqs();
    int profileNum = clustalw::userParameters->getProfileNum();    
    
    if(currentMode == profileMode)
    {
        int numSeqsProfile1 = alignPtr->getProfile1NumSeqs();
        alignmentViewer->updateView(1, numSeqsProfile1);
        if(profileNum == 2 && numSeqsProfile1 < numSeqs)
        {
            profile2Viewer->updateView(numSeqsProfile1 + 1, numSeqs);
        }
        else
        {
            profile2Viewer->updateView(0, 0);
        }
    }
    else
    {
        alignmentViewer->updateView(1, numSeqs);
    }
    //hideLowScoreSegs(); // Mark 22-5-2007 
    resetAndHideLowScoreSeqs(); // Mark 23-5-2007
}

void MainWindow::selectAllSequences()
{
    KeyController::Instance()->clearCtrl(); //- nige

    alignmentViewer->selectAllSequences();
}

void MainWindow::selectProfile1()
{
    alignmentViewer->selectAllSequences();
}

void MainWindow::selectProfile2()
{
    profile2Viewer->selectAllSequences();
}

void MainWindow::addProfile2ToProfile1()
{
    /* AW corresponding 1,83 function is xmenu.c:MergeProfiles */
    if (clustalw::userParameters->getProfile2Empty())
    {
        QMessageBox::information(this, "", "Profile 2 not loaded", QMessageBox::Ok);
        return;
    }

        
    int numSeqs = alignPtr->getNumSeqs();
    clustalw::userParameters->setProfileNum(1);
    alignPtr->setProfile1NumSeqs(numSeqs);
    clustalw::userParameters->setProfile2Empty(true);
    /* AW: (related to bug 141)
     * shouldn't userParameters->getUseSS2() and
     * userParameters->getStructPenalties2() etc
     * be reset as well here?
     */
    profile2Viewer->updateView(0, 0);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));    
    alignmentViewer->updateView(1, numSeqs);
    QApplication::restoreOverrideCursor();

    
    /* bug 141: seqs are displayed correctly, but saving
       failes: seqs in profile 1 are used repeatedly because
       outputindex is broken
       e.g. for two profile of four sequences each:
       1:1 2:2 3:3 4:4
       5:1 6:2 7:3 8:4
       In 1.83 outputindex is okay at this stage. so maybe the error
       occurs earlier on, but the safest bet here to fix the bug was
       simply to reset the outputindex
    */
    alignPtr->setDefaultOutputIndex();
  
}

void MainWindow::clearSequenceSelection()
{
    alignmentViewer->clearSequenceSelection();
    profile2Viewer->clearSequenceSelection();
}

void MainWindow::clearRangeSelection()
{
    alignmentViewer->clearRangeSelection();
    profile2Viewer->clearRangeSelection();
}

void MainWindow::searchForString()
{
    KeyController::Instance()->clearCtrl(); //- nige

    bool seqsSelectedInAlignment = false;
    bool seqsSelectedInProfile2 = false;
    searchBeginSeq = 0;
    searchBeginRes = 0;
    searchString = "";
    if(alignmentViewer->getNumberOfSeqsSelected() != 0)
    {
        seqsSelectedInAlignment = true;
    }
    if((currentMode == profileMode) && profile2Viewer->getNumberOfSeqsSelected() != 0)
    {
        seqsSelectedInProfile2 = true;
    }
    
    if(seqsSelectedInAlignment || seqsSelectedInProfile2)
    {
        searchSeqs->exec();
    }
    else
    {
        QMessageBox::information(this, "", "You must select sequences first",
                                 QMessageBox::Ok);
    }
}

void MainWindow::removeAllGaps()
{
    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    
    bool seqsSelectedInAlignment = false, seqsSelectedInProfile2 = false;
    
    if(alignmentViewer->getNumberOfSeqsSelected() != 0)
    {
        seqsSelectedInAlignment = true;
    }
    if((currentMode == profileMode) && profile2Viewer->getNumberOfSeqsSelected() != 0)
    {
        seqsSelectedInProfile2 = true;
    }
    
    if(!seqsSelectedInAlignment && !seqsSelectedInProfile2)
    {
        QMessageBox::information(this, "", "Select sequences by clicking on names.",
                                 QMessageBox::Ok);
        return;                         
    }
    
    int answer;
    answer = QMessageBox::information(this, "", "Remove gaps from selected sequences?",
                                 QMessageBox::Yes, QMessageBox::No);
    if(answer == QMessageBox::No)
    {
        return;
    }
    
    std::vector<int> selected;
    selected = getAllSelectedSeqsVector();
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    alignPtr->removeGapsFromSelectedSeqs(&selected);

    if(currentMode == profileMode)
    {
        alignmentViewer->updateView();
        profile2Viewer->updateView();
    }
    else
    {
        alignmentViewer->updateView();
    }
    QApplication::restoreOverrideCursor();                                             
}

vector<int> MainWindow::getAllSelectedSeqsVector()
{
    int nSeqs = alignPtr->getNumSeqs();
    std::vector<int> selected;
    selected.resize(nSeqs + 1, 0);
    
    // Get a vector which contains a 1 for each sequences that is selected.
    // Not it must start at index 1 as this is the was the clustal code works.
    if(currentMode == profileMode)
    { 
        int size1, size2;
        const std::vector<int>* alignmentSelected = alignmentViewer->getSelectedRows();
        size1 = alignmentSelected->size();
        const std::vector<int>* profileSelected = profile2Viewer->getSelectedRows();
        size2 = profileSelected->size();
        
        //cout << "gas: " << size1 << "+" << size2 << "=" << size1+size2 << endl;

        for(int i = 0; i < size1 + size2; i++)
        {
            if(i < size1)
            {
                if((*alignmentSelected)[i] == 1)
                {
                    selected[i + 1] = 1;
                }
                else
                {
                    selected[i + 1] = 0;
                }
            }
            else
            {
                if((*profileSelected)[i - size1] == 1)
                {
                    selected[i + 1] = 1;
                }
                else
                {
                    selected[i + 1] = 0;
                }                            
            }
        }
    }
    else
    {
        const std::vector<int>* alignmentSelected = alignmentViewer->getSelectedRows();
        int size;
        size = alignmentSelected->size();
        for(int i = 0; i < size; i++)
        {
            if((*alignmentSelected)[i] == 1)
            {
                selected[i + 1] = 1;
            }
            else
            {
                selected[i + 1] = 0;
            }                   
        }
    }
    return selected;
}

void MainWindow::removeGapOnlyColumns()
{
    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));    
    if(currentMode == profileMode)
    {
        alignPtr->removeAllGapOnlyColumns(alignmentViewer->getFirstSeq(),
                                          alignmentViewer->getLastSeq(), 1);
        alignPtr->removeAllGapOnlyColumns(profile2Viewer->getFirstSeq(), 
                                          profile2Viewer->getLastSeq(), 2);
        alignmentViewer->updateView();
        profile2Viewer->updateView();                                          
    }
    else
    {
        alignPtr->removeAllGapOnlyColumns(1, nSeqs, 0); 
        alignmentViewer->updateView();   
    }
    QApplication::restoreOverrideCursor();
    clustalw::utilityObject->info("All the columns that contains only the gaps, are removed!");                 
}

void MainWindow::doCompleteAlignment()
{
    KeyController::Instance()->clearCtrl(); //- nige

    // Need to open all the files that are needed first!
    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
        
    FileNameDialogType type = CompleteAlign;
    AlignType alignType = Sequence;
    AlignOutputFileNames namesDialog(sequenceFileName, type, alignType);
    int code = namesDialog.exec();
    
    if(code == QDialog::Accepted)
    {
        clustalw::AlignmentFileNames names = namesDialog.getNames();
        string phylipName = names.treeFile;
        clustalObj->QTSetFileNamesForOutput(names);
        alignmentViewer->updateView(0, 0); 
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        toplevelMenusEnabled(false);// Andreas Wilm (UCD): needed because we keep the window
                               // responsive (window movement etc) during alignment process
            
        clustalObj->align(&phylipName);

        alignPtr->reloadAlignment();        
        alignmentViewer->updateView(1, alignPtr->getNumSeqs());

        if(showLowScoreSegments == true)
        {
            calculateLowScoreAlign();
        }     
        searchBeginSeq = 0;
        searchBeginRes = 1;

        toplevelMenusEnabled(true);
        QApplication::restoreOverrideCursor();         
    }   
    alignmentViewer->updateView();
}

void MainWindow::produceGuideTreeOnly()
{
    KeyController::Instance()->clearCtrl(); //- nige

    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    if(nSeqs < 2)
    {
        QMessageBox::information(this, "", "Cannot produce tree. Not enough sequences",
                                 QMessageBox::Ok);
        return;    
    }
    FileNameDialogType type = DoGuideTreeOnly;
    AlignType alignType = Sequence;
    AlignOutputFileNames namesDialog(sequenceFileName, type, alignType);
    
    int code = namesDialog.exec();
    
    if(code == QDialog::Accepted)
    {
        clustalw::AlignmentFileNames names = namesDialog.getNames();
        string phylipName = names.treeFile;
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));                
        clustalObj->doGuideTreeOnly(&phylipName);
        QApplication::restoreOverrideCursor();            
    }        
}

void MainWindow::doAlignmentFromGuideTree()
{
    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    FileNameDialogType type = AlignFromTree;
    AlignType alignType = Sequence;    
    AlignOutputFileNames namesDialog(sequenceFileName, type, alignType);
    
    int code = namesDialog.exec();
    
    if(code == QDialog::Accepted)
    {
        clustalw::AlignmentFileNames names = namesDialog.getNames();
        string phylipName = names.treeFile;
        clustalObj->QTSetFileNamesForOutput(names);
        alignmentViewer->updateView(0, 0);         
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));        
        clustalObj->doAlignUseOldTree(&phylipName);
        
        alignPtr->reloadAlignment(); 
        alignmentViewer->updateView(1, alignPtr->getNumSeqs());

        if(showLowScoreSegments == true)
        {
            calculateLowScoreAlign();
        }     
        searchBeginSeq = 0;
        searchBeginRes = 1; 
        QApplication::restoreOverrideCursor();                    
    }
    alignmentViewer->updateView();
}

void MainWindow::realignSelectedSequences()
{
    // Check if a file is loaded
    int numSeqs = alignPtr->getNumSeqs();
    if(numSeqs == 0)
    {
        QMessageBox::information(this, "", "No file loaded.", QMessageBox::Ok);
        return;
    }
    
    // check if some seqs are selected
    bool seqsSelected = anySeqsSelected();
    
    if(!seqsSelected)
    {
        QString message = "Select sequences to be realigned by clicking on the names.";
        QMessageBox::information(this, "", message, QMessageBox::Ok);
        return;    
    }    
    
    // check if we have multi or profile mode
    std::vector<int> selected;
    selected = getAllSelectedSeqsVector();

    bool allSelected = allSeqsInAlignmentOrProfileSelected(&selected);
    if(allSelected)
    {
        doCompleteAlignment(); // Dont cut anything, just do a complete realign.
        return;    
    }    
    
    FileNameDialogType type = RealignSelectedSeqs;
    AlignType alignType = Sequence;    
    AlignOutputFileNames namesDialog(sequenceFileName, type, alignType);
    
    int code = namesDialog.exec();
    if(code == QDialog::Accepted)
    {    
        alignmentViewer->updateView(0, 0);
        profile2Viewer->updateView(0, 0);        
        // Cut these sequences from the vector of sequences.
        vector<clustalw::Sequence> seqsToRealign;
        seqsToRealign = alignPtr->cutSelectedSequencesFromAlignment(&selected);
        
        // Now paste them in at the end.
        int nSeqs = alignPtr->getNumSeqs();
        alignPtr->setProfile1NumSeqs(nSeqs);
        
        // Now paste the other sequences in!    
        // Mark change jan 16 2007. added false into function call.
        alignPtr->pasteSequencesIntoPosition(&seqsToRealign, nSeqs, false); 
        clustalw::AlignmentFileNames names = namesDialog.getNames();
        string phylipName1 = names.treeFile;
        
        clustalObj->QTSetFileNamesForOutput(names); 
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));                
        clustalObj->sequencesAlignToProfile(&phylipName1);
        
        if(alignPtr->getNumSeqs() > 0)
        {
            alignPtr->reloadAlignment();             
            alignmentViewer->updateView(1, alignPtr->getNumSeqs());
        
            if(showLowScoreSegments == true)
            {
                calculateLowScoreAlign();
            }
        }     
        searchBeginSeq = 0;
        searchBeginRes = 1;
        QApplication::restoreOverrideCursor();                     
    }
    alignmentViewer->updateView();           
   
}

void MainWindow::realignSelectedResidueRange()
{
    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    
    int beginCol = alignmentViewer->getFirstSelectedCol();
    int endCol = alignmentViewer->getSecondSelectedCol();

    if(beginCol < 0 || endCol < 1 || beginCol > endCol)
    {
        QMessageBox::information(this, "", "No range selected", QMessageBox::Ok);
        return;
    }    
    
    FileNameDialogType type = RealignSeqRange;
    AlignType alignType = Sequence;
    AlignOutputFileNames namesDialog(sequenceFileName, type, alignType);
    int code = namesDialog.exec();
    
    if(code == QDialog::Accepted)
    {
        clustalw::AlignmentFileNames names = namesDialog.getNames();
        string phylipName = names.treeFile;
        clustalObj->QTSetFileNamesForOutput(names);
        alignmentViewer->updateView(0, 0); 
        bool realignEndGapPen = namesDialog.getRealignEndGapPen();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));        
        clustalObj->QTRealignSelectedRange(names, beginCol, endCol, realignEndGapPen);
        
        if(alignPtr->getNumSeqs() > 0)
        {
            alignmentViewer->updateView(1, alignPtr->getNumSeqs());
        
            if(showLowScoreSegments == true)
            {
                calculateLowScoreAlign();
            }
        }
        QApplication::restoreOverrideCursor();         
    }   
    alignmentViewer->updateView();         
}

void MainWindow::alignProfile2ToProfile1()
{
    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    if(clustalw::userParameters->getProfile1Empty())
    {
        QMessageBox::information(this, "", "Profile 1 empty", QMessageBox::Ok);
        return;    
    }
    if(clustalw::userParameters->getProfile2Empty())
    {
        QMessageBox::information(this, "", "Profile 2 empty", QMessageBox::Ok);
        return;    
    } 
    FileNameDialogType type = AlignProf2ToProf1;
    AlignType alignType = Profile;
    AlignOutputFileNames namesDialog(sequenceFileName, type, alignType, profile2FileName);
    int code = namesDialog.exec();
    
    if(code == QDialog::Accepted)
    {
        clustalw::AlignmentFileNames names = namesDialog.getNames();
        string phylipName1 = names.treeFile;
        string phylipName2 = names.profile2TreeFile;
        
        clustalObj->QTSetFileNamesForOutput(names);

        /* 26-03-07,nige: save the profile boundaries */
        int prof1First = alignmentViewer->getFirstSeq();
        int prof1Last  = alignmentViewer->getLastSeq();
        int prof2First = profile2Viewer->getFirstSeq();
        int prof2Last  = profile2Viewer->getLastSeq();

        alignmentViewer->updateView(0, 0);
        profile2Viewer->updateView(0, 0);

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));                 
        clustalObj->profileAlign(&phylipName1, &phylipName2);

        /* 26-03-07,nige: redisplay Profile1 and Profile2 */
        //alignPtr->reloadAlignment(); //- nige: not called in clustalx
        //alignmentViewer->updateView(1, alignPtr->getNumSeqs()); //- nige
        alignmentViewer->updateView(prof1First, prof1Last);       //- nige
        profile2Viewer->updateView(prof2First, prof2Last);        //- nige
        
        if(showLowScoreSegments == true)
        {
            calculateLowScoreAlign();
        }     
        searchBeginSeq = 0;
        searchBeginRes = 1; 
        QApplication::restoreOverrideCursor();                    
    }
    alignmentViewer->updateView();           
}

void MainWindow::alignProfilesFromGuideTrees()
{
    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    if(clustalw::userParameters->getProfile1Empty())
    {
        QMessageBox::information(this, "", "Profile 1 empty", QMessageBox::Ok);
        return;    
    }
    if(clustalw::userParameters->getProfile2Empty())
    {
        QMessageBox::information(this, "", "Profile 2 empty", QMessageBox::Ok);
        return;    
    } 
    FileNameDialogType type = AlignProf2ToProf1WithTree;
    AlignType alignType = Profile;    
    AlignOutputFileNames namesDialog(sequenceFileName, type, alignType, profile2FileName);
    int code = namesDialog.exec();
    
    if(code == QDialog::Accepted)
    {
        clustalw::AlignmentFileNames names = namesDialog.getNames();
        string phylipName1 = names.treeFile;
        string phylipName2 = names.profile2TreeFile;
        clustalObj->QTSetFileNamesForOutput(names);

        /* 26-03-07,nige: save the profile boundaries */
        int prof1First = alignmentViewer->getFirstSeq();
        int prof1Last  = alignmentViewer->getLastSeq();
        int prof2First = profile2Viewer->getFirstSeq();
        int prof2Last  = profile2Viewer->getLastSeq();

        alignmentViewer->updateView(0, 0);
        profile2Viewer->updateView(0, 0);
        
        // Need to use the old guide trees!
        if(phylipName1 != "")
        {
            clustalw::userParameters->setUseTree1File(true);
        }
        else
        {
            clustalw::userParameters->setUseTree1File(false);
        }
        if(phylipName2 != "")
        {
            clustalw::userParameters->setUseTree2File(true);
        }
        else
        {
            clustalw::userParameters->setUseTree2File(false);
        }
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));                         
        clustalObj->profileAlign(&phylipName1, &phylipName2);
        
        if(alignPtr->getNumSeqs() > 0)
        {
            /* 26-03-07,nige: redisplay Profile1 and Profile2 */
            //alignPtr->reloadAlignment(); //- nige: not called in clustalx
            //alignmentViewer->updateView(1, alignPtr->getNumSeqs()); //- nige
            alignmentViewer->updateView(prof1First, prof1Last);       //- nige
            profile2Viewer->updateView(prof2First, prof2Last);        //- nige
  
            if(showLowScoreSegments == true)
            {
                calculateLowScoreAlign();
            }
        }     
        searchBeginSeq = 0;
        searchBeginRes = 1;
        QApplication::restoreOverrideCursor();                     
    }
    alignmentViewer->updateView();     
}

void MainWindow::alignSequencesToProfile1()
{
    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    if(clustalw::userParameters->getProfile1Empty())
    {
        QMessageBox::information(this, "", "Profile 1 empty", QMessageBox::Ok);
        return;    
    }
    if(clustalw::userParameters->getProfile2Empty())
    {
        QMessageBox::information(this, "", "Profile 2 empty", QMessageBox::Ok);
        return;    
    } 
    FileNameDialogType type = AlignSeqsToProf1;
    AlignType alignType = Profile;    
    AlignOutputFileNames namesDialog(sequenceFileName, type, alignType, profile2FileName);
    int code = namesDialog.exec();
    
    if(code == QDialog::Accepted)
    {
        clustalw::AlignmentFileNames names = namesDialog.getNames();
        string phylipName1 = names.treeFile;
        
        clustalObj->QTSetFileNamesForOutput(names);

        /* 26-03-07,nige: save the profile boundaries */
        int prof1First = alignmentViewer->getFirstSeq();
        int prof1Last  = alignmentViewer->getLastSeq();
        int prof2First = profile2Viewer->getFirstSeq();
        int prof2Last  = profile2Viewer->getLastSeq();

        alignmentViewer->updateView(0, 0);
        profile2Viewer->updateView(0, 0);         

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));        
        clustalObj->sequencesAlignToProfile(&phylipName1);

        /* 26-03-07,nige: redisplay Profile1 and Profile2 */
        alignPtr->reloadAlignment();
        //alignmentViewer->updateView(1, alignPtr->getNumSeqs()); //- nige
        alignmentViewer->updateView(prof1First, prof1Last);       //- nige
        profile2Viewer->updateView(prof2First, prof2Last);        //- nige

        if(showLowScoreSegments == true)
        {
            calculateLowScoreAlign();
        }     
        searchBeginSeq = 0;
        searchBeginRes = 1;
        QApplication::restoreOverrideCursor();                     
    }
    alignmentViewer->updateView();           
    
}

void MainWindow::alignSequencesToProfile1FromTree()
{
    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    if(clustalw::userParameters->getProfile1Empty())
    {
        QMessageBox::information(this, "", "Profile 1 empty", QMessageBox::Ok);
        return;    
    }
    if(clustalw::userParameters->getProfile2Empty())
    {
        QMessageBox::information(this, "", "Profile 2 empty", QMessageBox::Ok);
        return;    
    } 
    
    FileNameDialogType type = AlignSeqsToProf1WithTree;
    AlignType alignType = Profile;    
    AlignOutputFileNames namesDialog(sequenceFileName, type, alignType, profile2FileName);
    int code = namesDialog.exec();
    
    if(code == QDialog::Accepted)
    {
        clustalw::AlignmentFileNames names = namesDialog.getNames();
        string phylipName1 = names.treeFile;
        // Need to use the old guide trees!
        if(phylipName1 != "")
        {
            clustalw::userParameters->setUseTree1File(true);
        }
        else
        {
            clustalw::userParameters->setUseTree1File(false);
        }
                
        clustalObj->QTSetFileNamesForOutput(names);

        /* 26-03-07,nige: save the profile boundaries */
        int prof1First = alignmentViewer->getFirstSeq();
        int prof1Last  = alignmentViewer->getLastSeq();
        int prof2First = profile2Viewer->getFirstSeq();
        int prof2Last  = profile2Viewer->getLastSeq();

        alignmentViewer->updateView(0, 0);
        profile2Viewer->updateView(0, 0);

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));                 
        clustalObj->sequencesAlignToProfile(&phylipName1);
        
        if(alignPtr->getNumSeqs() > 0)
        {
            /* 26-03-07,nige: redisplay Profile1 and Profile2 */
            alignPtr->reloadAlignment();
            //alignmentViewer->updateView(1, alignPtr->getNumSeqs()); //- nige
            alignmentViewer->updateView(prof1First, prof1Last);       //- nige
            profile2Viewer->updateView(prof2First, prof2Last);        //- nige
        
            if(showLowScoreSegments == true)
            {
                calculateLowScoreAlign();
            }
        }     
        searchBeginSeq = 0;
        searchBeginRes = 1;
        QApplication::restoreOverrideCursor();                     
    }
    alignmentViewer->updateView();           
}

void MainWindow::outputFormatOptions()
{
    AlignmentFormatOptions alignFormatOptions;
    alignFormatOptions.exec();
}

// Submenu
void MainWindow::resetNewGapsBeforeAlignment()
{
    // Andreas Wilm (UCD) 2008-03-10:
    // resetting gaps doesn't make sense if iteration is enabled
    if (clustalw::userParameters->IterationIsEnabled()) {
        QMessageBox::warning(this, tr("Error"), 
                             tr("Error: \"Reset gaps\" in combination with \"Iteration\" is not possible."));
        actionResetNewGapsBeforeAlignment->setChecked(false);
        return;
    }
    
    if(actionResetNewGapsBeforeAlignment->isChecked())
    {
        clustalw::userParameters->setResetAlignmentsNew(true);
    }
    else
    {
        clustalw::userParameters->setResetAlignmentsNew(false);
    }
    if(clustalw::userParameters->getResetAlignmentsNew() == true)
    {
        clustalw::userParameters->setResetAlignmentsAll(false);
        actionResetAllGapsBeforeAlignment->setChecked(false);
    }
}

void MainWindow::resetAllGapsBeforeAlignment()
{
    // Andreas Wilm (UCD) 2008-03-10:
    // resetting gaps doesn't make sense if iteration is enabled
    if (clustalw::userParameters->IterationIsEnabled()) {
        QMessageBox::warning(this, tr("Error"), 
                             tr("Error: \"Reset gaps\" in combination with \"Iteration\" is not possible."));
        actionResetAllGapsBeforeAlignment->setChecked(false);
        return;
    }
    
    if(actionResetAllGapsBeforeAlignment->isChecked())
    {
        clustalw::userParameters->setResetAlignmentsAll(true);
    }
    else
    {
        clustalw::userParameters->setResetAlignmentsAll(false);
    }
    if(clustalw::userParameters->getResetAlignmentsAll() == true)
    {
        clustalw::userParameters->setResetAlignmentsNew(false);
        actionResetNewGapsBeforeAlignment->setChecked(false); 
    }   
}

void MainWindow::pairwiseAlignmentParameters()
{
    pairwiseParams->exec();
}

void MainWindow::multipleAlignmentParameters()
{
    alignmentParams->exec();
}

void MainWindow::proteinGapParameters()
{
    ProteinGapParameters dialogProteinGapParameters;
    dialogProteinGapParameters.exec();
}

void MainWindow::secondaryStructureParameters()
{
    SecStructOptions dialogSecStructOptions;
    dialogSecStructOptions.exec();
}

void MainWindow::setDefaultParameters()
{
    clustalw::userParameters->setParamsToDefault();
    clustalw::subMatrix->setValuesToDefault();
    // Also need to reset the reset Gaps before Alignment !!!!
    actionResetAllGapsBeforeAlignment->setChecked(false);
    actionResetNewGapsBeforeAlignment->setChecked(false);    
}

void MainWindow::drawNJTree()
{
    KeyController::Instance()->clearCtrl(); //- nige

    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    if(nSeqs < 2)
    {
        QString message, num;
        num.setNum(nSeqs);
        message = "Alignment has only " + num + " sequences";
        QMessageBox::information(this, "", message, QMessageBox::Ok);
        return;
    }        

    TreeOutputFileNames treeNameDialog(sequenceFileName);
    int code = treeNameDialog.exec();

    if(code == QDialog::Accepted)
    {
        clustalw::TreeNames treeNames = treeNameDialog.getNames();
        alignmentViewer->updateView(0, 0);
        profile2Viewer->updateView(0, 0); 
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));                
        clustalObj->phylogeneticTree(&treeNames.phylipName, &treeNames.clustalName,
                                     &treeNames.distName, &treeNames.nexusName,
                                     treeNames.pimName);
        if(alignPtr->getNumSeqs() > 0)
        {           
            alignmentViewer->updateView(1, alignPtr->getNumSeqs());
        
            if(showLowScoreSegments == true)
            {
                calculateLowScoreAlign();
            }
        }     
        searchBeginSeq = 0;
        searchBeginRes = 1;
        QApplication::restoreOverrideCursor();                                             
    } 
    alignmentViewer->updateView();    
}

void MainWindow::bootstrapNJTree()
{
    KeyController::Instance()->clearCtrl(); //- nige

    int nSeqs = alignPtr->getNumSeqs();
    if(nSeqs == 0)
    {
        QMessageBox::information(this, "", "No sequences loaded", QMessageBox::Ok);
        return;
    }
    if(nSeqs < 2)
    {
        QString message, num;
        num.setNum(nSeqs);
        message = "Alignment has only " + num + " sequences";
        QMessageBox::information(this, "", message, QMessageBox::Ok);
        return;
    }        

    BootstrapTreeDialog bootNameDialog(sequenceFileName);
    int code = bootNameDialog.exec();

    if(code == QDialog::Accepted)
    {
        clustalw::TreeNames treeNames = bootNameDialog.getNames();
        alignmentViewer->updateView(0, 0);
        profile2Viewer->updateView(0, 0);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));                 
        clustalObj->bootstrapTree(&treeNames.phylipName, &treeNames.clustalName,
                                 &treeNames.nexusName);
        if(alignPtr->getNumSeqs() > 0)
        {            
            alignmentViewer->updateView(1, alignPtr->getNumSeqs());
        
            if(showLowScoreSegments == true)
            {
                calculateLowScoreAlign();
            }
        }     
        searchBeginSeq = 0;
        searchBeginRes = 1;
        QApplication::restoreOverrideCursor();                                              
    } 
    alignmentViewer->updateView();    

}

void MainWindow::excludePositionsWithGaps()
{
    if(actionExcludePosWithGap->isChecked())
    {    
        clustalw::userParameters->setTossGaps(true);
    }
    else
    {
        clustalw::userParameters->setTossGaps(false);
    }
}

void MainWindow::correctForMultipleSubstitutions()
{
    if(actionCorrectForMultiSub->isChecked())
    {    
        clustalw::userParameters->setKimura(true);
    }
    else
    {
        clustalw::userParameters->setKimura(false);
    }    
}

void MainWindow::treeOutputFormatOptions()
{
    TreeFormatOptions treeFormatOptions;
    treeFormatOptions.exec();
}

void MainWindow::backgroundColoring()
{
    if(backGroundColors)
    {
        backGroundColors = false;
    }
    else
    {
        backGroundColors = true;
    }
    
    alignmentViewer->setBackGroundColoring(backGroundColors);
    profile2Viewer->setBackGroundColoring(backGroundColors);    
}

void MainWindow::blackAndWhite()
{
    //int scheme = BlackAndWhite;
    
    actionDefaultColors->setChecked(false);
    //actionLoadColorParameterFile->setChecked(false);
    
    alignmentViewer->setBlackAndWhite();
    profile2Viewer->setBlackAndWhite();
}

void MainWindow::defaultColors()
{
    bool dnaFlag = clustalw::userParameters->getDNAFlag();

    actionBlackAndWhite->setChecked(false);
    actionDefaultColors->setChecked(true);
    //actionLoadColorParameterFile->setChecked(false);
    
    if(dnaFlag)
    {
        alignmentViewer->setColorScheme(&dnaColors);
        profile2Viewer->setColorScheme(&dnaColors);
    }
    else
    {
        alignmentViewer->setColorScheme(&proteinColors);
        profile2Viewer->setColorScheme(&proteinColors);    
    }
        
}

void MainWindow::loadColorParameterFile()
{
    bool success = false;
    
    QString userFile;
    // Mark Jan 18th 2007: changes to remember working Dir
    FileDialog fd;
    userFile = fd.getOpenParamFileName(this, tr("Load Color Parameter File"), 
				       tr("XBEL Files (*.xbel *.xml);;All Files (*)"));
        
    if(userFile == "")
    {
        return;
    }
                                        
    success = readInParamFile(userFile, &userColors);
    
    // If successful, load it!
    if(success)
    {
        // Use the file in the viewers
        actionBlackAndWhite->setChecked(false);
        actionDefaultColors->setChecked(false);
        //actionLoadColorParameterFile->setChecked(true);
        alignmentViewer->setColorScheme(&userColors);
        profile2Viewer->setColorScheme(&userColors);        
    }
    else
    {
        // use default colors
        actionBlackAndWhite->setChecked(false);
        actionDefaultColors->setChecked(true);
        //actionLoadColorParameterFile->setChecked(false);
        
        if(clustalw::userParameters->getDNAFlag())
        {
            alignmentViewer->setColorScheme(&dnaColors);
            profile2Viewer->setColorScheme(&dnaColors);
        }
        else
        {
            alignmentViewer->setColorScheme(&proteinColors);
            profile2Viewer->setColorScheme(&proteinColors);    
        }        
    }
}

void MainWindow::showLowScoreSeg()
{
    if(showLowScoreSegments)
    {
        hideLowScoreSegs();       
    }
    else
    {
        actionShowLowScoreSeg->setChecked(true);
        showLowScoreSegments = true;  
    }
    recalculateLowScoreSegments();    
}

void MainWindow::hideLowScoreSegs() // Mark 22-5-07
{
    actionShowLowScoreSeg->setChecked(false);
    showLowScoreSegments = false; 
}

void MainWindow::resetAndHideLowScoreSeqs() // Mark 23-5-07
{
    hideLowScoreSegs();
    seqWeightCalculatedAlignmentViewer = false;
    seqWeightCalculatedProfile2Viewer = false;
        
    alignmentViewer->showLowScoringSegments(false, lowScoreResAlignmentViewer);
    profile2Viewer->showLowScoringSegments(false, lowScoreResProfile2Viewer);  
}



void MainWindow::recalculateLowScoreSegments()
{
    if(!showLowScoreSegments)
    {        
        if(alignmentModeMenu->currentIndex() == 1) // If it is a profile
        {
            alignmentViewer->showLowScoringSegments(showLowScoreSegments,
                                                lowScoreResAlignmentViewer);
            profile2Viewer->showLowScoringSegments(showLowScoreSegments,
                                                lowScoreResProfile2Viewer);
        }
        else
        {
            alignmentViewer->showLowScoringSegments(showLowScoreSegments,
                                                lowScoreResAlignmentViewer);      
        }        
    }
    else
    {
        if(alignmentModeMenu->currentIndex() == 1) // If it is a profile
        {
            calculateLowScoreAlign();
            calculateLowScoreProfile();
        }
        else
        {
            calculateLowScoreAlign();       
        }    
    }
}

void MainWindow::calculateLowScoreAlign()
{
    int firstSeqAlign, lastSeqAlign, numSeqsAlign, nColsAlign;
        
    firstSeqAlign = alignmentViewer->getFirstSeq();
    lastSeqAlign = alignmentViewer->getLastSeq();
    numSeqsAlign = lastSeqAlign - firstSeqAlign + 1;
        
    if(firstSeqAlign != 0 && lastSeqAlign != 0)
    {
        nColsAlign = alignPtr->getLengthLongestSequence(firstSeqAlign, lastSeqAlign);
        
        // Now I need to set up the vectors to hold the weights and the low scoring segs
        // Check if the seqWeights need to be resized
        if(!seqWeightCalculatedAlignmentViewer)
        {
            lowScoreSeqWeightAlignmentViewer->clear();
            lowScoreSeqWeightAlignmentViewer->resize(numSeqsAlign + 1);
        }
        
        lowScoreResAlignmentViewer->clearArray();
        lowScoreResAlignmentViewer->ResizeRect(numSeqsAlign + 1, nColsAlign + 1);
        
        clustalw::LowScoreSegParams alignParams;
        alignParams.firstSeq = firstSeqAlign - 1;
        alignParams.nSeqs = numSeqsAlign;
        alignParams.lastSeq = lastSeqAlign - 1;
        alignParams.nCols = nColsAlign;
        alignParams.seqWeight = lowScoreSeqWeightAlignmentViewer;
        alignParams.lowScoreRes = lowScoreResAlignmentViewer;
        alignParams.seqWeightCalculated = seqWeightCalculatedAlignmentViewer;
        
        clustalObj->QTcalcLowScoreSegments(&alignParams);
        alignmentViewer->showLowScoringSegments(showLowScoreSegments,
                                                lowScoreResAlignmentViewer);
        seqWeightCalculatedAlignmentViewer = true;                                        
    }    
}

void MainWindow::calculateLowScoreProfile()
{
    int firstSeqProf, lastSeqProf, numSeqsProf, nColsProf;
        
    firstSeqProf = profile2Viewer->getFirstSeq();
    lastSeqProf = profile2Viewer->getLastSeq();
    numSeqsProf = lastSeqProf - firstSeqProf + 1;
    
    if(firstSeqProf != 0 && lastSeqProf != 0)
    {            
        nColsProf = alignPtr->getLengthLongestSequence(firstSeqProf, lastSeqProf);
        
        if(!seqWeightCalculatedProfile2Viewer)
        {
            lowScoreSeqWeightProfile2Viewer->clear();
            lowScoreSeqWeightProfile2Viewer->resize(numSeqsProf + 1);
        }
        
        lowScoreResProfile2Viewer->clearArray();
        lowScoreResProfile2Viewer->ResizeRect(numSeqsProf + 1, nColsProf + 1);

        clustalw::LowScoreSegParams profileParams;
        profileParams.firstSeq = firstSeqProf - 1;
        profileParams.nSeqs = numSeqsProf;
        profileParams.lastSeq = lastSeqProf - 1;
        profileParams.nCols = nColsProf;
        profileParams.seqWeight = lowScoreSeqWeightProfile2Viewer;
        profileParams.lowScoreRes = lowScoreResProfile2Viewer;
        profileParams.seqWeightCalculated = seqWeightCalculatedProfile2Viewer;
            
        clustalObj->QTcalcLowScoreSegments(&profileParams);
        profile2Viewer->showLowScoringSegments(showLowScoreSegments,
                                                lowScoreResProfile2Viewer);
        
        seqWeightCalculatedProfile2Viewer = true;
    }
}

void MainWindow::showExceptionalRes()
{
    if(showExceptionalResidues)
    {
        actionShowExceptionalRes->setChecked(false);
        showExceptionalResidues = false;
    }
    else
    {
        actionShowExceptionalRes->setChecked(true);
        showExceptionalResidues = true;
    }
    
    if(alignmentModeMenu->currentIndex() == 1) // If it is a profile
    {
        alignmentViewer->showExceptionalRes(showExceptionalResidues);
        profile2Viewer->showExceptionalRes(showExceptionalResidues);
    }
    else
    {
        alignmentViewer->showExceptionalRes(showExceptionalResidues);
    }
}

void MainWindow::lowScoreSegParams()
{
    lowScoreParams->exec();
    recalculateLowScoreSegments();
}

void MainWindow::columnScoreParams()
{
    colScoreParams->exec();
    // Now we need to recalculate the histogram.
    //alignmentViewer->updateHistogram(1, alignPtr->getNumSeqs());
    // Also need to recalculate the low scoring and exceptional residues.
    if(alignmentModeMenu->currentIndex() == 1) // If it is a profile
    {
        alignmentViewer->updateHistogram(alignmentViewer->getFirstSeq(),
                                         alignmentViewer->getLastSeq());
        alignmentViewer->showExceptionalRes(showExceptionalResidues);
        profile2Viewer->updateHistogram(profile2Viewer->getFirstSeq(),
                                        profile2Viewer->getLastSeq());
        profile2Viewer->showExceptionalRes(showExceptionalResidues);
    }
    else
    {
        alignmentViewer->updateHistogram(1, alignPtr->getNumSeqs());
        alignmentViewer->showExceptionalRes(showExceptionalResidues);        
    }    
}

void MainWindow::saveColumnScoresToFile()
{
    alignmentViewer->outputColumnScores();
}

void MainWindow::errorHandler(int errorCode, string offendingSeq)
{
    string info;
    if(errorCode == clustalw::CANNOTOPENFILE)
    {
        QMessageBox::information(this, "",
                                 "Cannot open file!", QMessageBox::Ok);
    }
    else if(errorCode == clustalw::BADFORMAT)
    {
        QMessageBox::information(this, "",
                                 "File is badly formatted", QMessageBox::Ok);
    }
    else if(errorCode == clustalw::SEQUENCETOOBIG)
    {
        info = "File contains sequences longer than allowed length: ";
        info += offendingSeq;
        info += ")";
        QMessageBox::information(this, "", info.c_str(), QMessageBox::Ok);
    }
    else if(errorCode == clustalw::NOSEQUENCESINFILE)
    {
        QMessageBox::information(this, "",
                                 "No sequences in file - bad format?", QMessageBox::Ok);
    }
    else if(errorCode == clustalw::ALLNAMESNOTDIFFERENT)
    {
        info =  "Conflicting sequence names: " ;
        info += offendingSeq;
        info += "; names must be unique!";
            QMessageBox::information(this, "", info.c_str(), QMessageBox::Ok);
    }
    else if(errorCode == clustalw::EMPTYSEQUENCE)
    {
        info = "There were empty sequences in the input file: ";
        info += offendingSeq;
        QMessageBox::information(this, "", info.c_str(), QMessageBox::Ok);
    }
    else if(errorCode == clustalw::MUSTREADINPROFILE1FIRST)
    {
        QMessageBox::information(this, "",
                                 "Must read in Profile 1 first!", QMessageBox::Ok);
    }
    else
    {
        QMessageBox::information(this, "",
                                 "An unknown error occured reading in the file!",
                                 QMessageBox::Ok);     
    }
}

void MainWindow::setUpSearchForStringDialog()
{
    searchString = "";
    searchSeqs = new SearchForString;
    connect(searchSeqs, SIGNAL(searchFromStartSignal()), this,
                             SLOT(searchFromStart()));
    connect(searchSeqs, SIGNAL(searchAgainSignal()), this, SLOT(searchAgain()));
}

void MainWindow::searchFromStart()
{
	bottomInfoLabel->setText(""); 
	// set both to start positions 
    searchBeginSeq = 0;
    searchBeginRes = 1;    
    //and then search again!!!!!
    searchAgain();
}

void MainWindow::searchAgain()
{
    bottomInfoLabel->setText("");
	bool seqsSelectedInAlignment = false;
    bool seqsSelectedInProfile2 = false;
    //searchString = searchSeqs->getSearchString().toStdString();
    searchString = searchSeqs->getSearchString().toUpper().toStdString(); //- nige: case-insensitive
    
    // If string is empty we should return. Dont search
    if(searchString.size() == 0)
    {
        return;
    }
    
    int numSeqsTopViewer;
    
    if(currentMode == profileMode)
    { 
        numSeqsTopViewer = alignPtr->getProfile1NumSeqs();
    }
    else
    {
        numSeqsTopViewer = alignPtr->getNumSeqs();
    }
    
    if(alignmentViewer->getNumberOfSeqsSelected() != 0)
    {
        seqsSelectedInAlignment = true;
    }
    if((currentMode == profileMode) && profile2Viewer->getNumberOfSeqsSelected() != 0)
    {
        seqsSelectedInProfile2 = true;
    }
    
    const std::vector<int>* seqsSelected;
    int firstSeqInViewer;
    bool found=false;        
    Viewer viewerToSearch;
    // Check seqs in top viewer   
    if(seqsSelectedInAlignment && searchBeginSeq < numSeqsTopViewer)
    {     
        seqsSelected = alignmentViewer->getSelectedRows();
        firstSeqInViewer = alignmentViewer->getFirstSeq();
        if(currentMode == profileMode)
        {
            viewerToSearch = Profile1Viewer;
        }
        else
        {
            viewerToSearch = SequenceViewer;
        }
        // Do the sequences in the alignment first
        found = findNextInstanceOfString(seqsSelected, firstSeqInViewer, viewerToSearch);
    }
    
    int numSeqs = alignPtr->getNumSeqs();
    
    // Check seqs in bottom viewer
    if(seqsSelectedInProfile2 && searchBeginSeq >= numSeqsTopViewer && 
       searchBeginSeq < numSeqs)
    {
        viewerToSearch = Profile2Viewer;
        seqsSelected = profile2Viewer->getSelectedRows();
        firstSeqInViewer = profile2Viewer->getFirstSeq();
        found = findNextInstanceOfString(seqsSelected, firstSeqInViewer, viewerToSearch);            
    } 
    
    if(found == false)
    {
        // print message to screen saying cannot find it!
        QString message = "Cannot find string " + QString(searchString.c_str());
        bottomInfoLabel->setText(message);        
    }       
}

bool MainWindow::findNextInstanceOfString(const std::vector<int>* seqsSelected, int firstSeq,
                                          Viewer viewerSearched)
{
    int size = seqsSelected->size();
    bool found = false;
           
    for(int i = searchBeginSeq; i < (size + firstSeq - 1); i++)
    {
        if((*seqsSelected)[i - firstSeq + 1] == 1)
        {
            // Then serch sequence i+1;
            int seq = i + 1;
                
            searchBeginRes = alignPtr->searchForString(&found, seq, searchBeginRes, 
                                                                  searchString);
            if(found == false)
            {
                searchBeginSeq++;
                searchBeginRes = 1;
            }
            else
            {
                // write message to screen saying where it was found
                QString seqName = QString(alignPtr->getName(seq).c_str());
                QString resFound = QString("%1").arg(searchBeginRes);
                QString message;
                if(viewerSearched == Profile1Viewer)
                {
                    message = "String " + QString(searchString.c_str()) + 
                                  " in profile1 sequence " + seqName;
                }
                else if(viewerSearched == Profile2Viewer)
                {
                    message = "String " + QString(searchString.c_str()) + 
                                  " in profile2 sequence " + seqName;
                }
                else
                {
                    message = "String " + QString(searchString.c_str()) + 
                                  " in sequence " + seqName;
                }
                message += ", column " + resFound;                  
                bottomInfoLabel->setText(message);
                searchBeginRes++;
                return true; // found it!!!!!
            }  
        }
        else
        {
            searchBeginSeq++;
            searchBeginRes = 1;
        }
    } 
    return false;
}

void MainWindow::generalHelp()
{
    HelpDisplayWidget help('G');
    help.exec();
}

void MainWindow::inputOutputHelp()
{
    HelpDisplayWidget help('F');
    help.exec();
}

void MainWindow::editAlignHelp()
{
    HelpDisplayWidget help('E');
    help.exec();
}

void MainWindow::multiAlignHelp()
{
    HelpDisplayWidget help('M');
    help.exec();
}

void MainWindow::profileAlignHelp()
{
    HelpDisplayWidget help('P');
    help.exec();
}

void MainWindow::secondaryStructureHelp()
{
    HelpDisplayWidget help('B');
    help.exec();
}

void MainWindow::treesHelp()
{
    HelpDisplayWidget help('T');
    help.exec();
}

void MainWindow::colorsHelp()
{
    HelpDisplayWidget help('C');
    help.exec();
}

void MainWindow::alignQualityHelp()
{
    HelpDisplayWidget help('Q');
    help.exec();
}

void MainWindow::commandLineHelp()
{
    HelpDisplayWidget help('9');
    help.exec();
}

void MainWindow::referencesHelp()
{
    HelpDisplayWidget help('R');
    help.exec();
}

MainWindow::~MainWindow()
{
    if(clustalObj != 0)
    {
        delete clustalObj;
    }                              
}

void MainWindow::setAllPtrsToZero()
{
    searchSeqs = 0;
    actionReferencesHelp = 0;
    actionCommandLineHelp = 0;
    actionAlignQualityHelp = 0;
    actionColorsHelp = 0;
    actionTreesHelp = 0;
    actionSecondaryStructureHelp = 0;
    actionProfileAlignHelp = 0;
    actionMultiAlignHelp = 0;
    actionEditAlignHelp = 0;
    actionInputOutputHelp = 0;
    actionGeneralHelp = 0;
    actionSaveColumnScoresToFile = 0;
    actionColumnScoreParams = 0;
    actionLowScoreSegParams = 0;
    actionShowExceptionalRes = 0;
    actionShowLowScoreSeg = 0;
    actionLoadColorParameterFile = 0;
    actionDefaultColors = 0;
    actionBlackAndWhite = 0;
    actionBackColoring = 0;
    actionTreeOutputFormatOptions = 0;
    actionCorrectForMultiSub = 0;
    actionExcludePosWithGap = 0;
    actionBootstrapNJTree = 0;
    actionDrawNJTree = 0;
    actionSecondaryStructureParameters = 0;
    actionProteinGapParameters = 0;
    actionMultipleAlignmentParameters = 0;
    actionPairwiseAlignmentParameters = 0;
    actionResetAllGapsBeforeAlignment = 0;
    actionResetNewGapsBeforeAlignment = 0;
    actionOutputFormatOptions = 0;
    menuAlignParameters = 0;
    actionSetDefaultParameters = 0;
    actionAlignSequencesToProfile1FromTree = 0;
    actionAlignSequencesToProfile1 = 0;
    actionAlignProfileFromTree = 0;
    actionAlignProfile1ToProfile2 = 0;
    actionRealignSelectRange = 0;
    actionRealignSelectSeq = 0;
    actionDoAlignmentFromGuideTree = 0;
    actionDoGuideTreeOnly = 0;
    actionDoCompleteAlign = 0;
    actionRemoveGapOnlyColumns = 0;
    actionRemoveAllGaps = 0;
    actionSearchForString = 0;
    actionClearRangeSelection = 0;
    actionClearSequenceSelection = 0;
    actionAddProfile2ToProfile1 = 0;
    actionSelectProfile2 = 0;
    actionSelectProfile1 = 0;
    actionSelectAllSequences = 0;
    actionPasteSequences = 0;
    actionCutSequences = 0;
    actionQuit = 0;
    actionWriteProfile2AsPost = 0;
    actionWriteProfile1AsPost = 0;
    actionWriteAlignmentAsPost = 0;
    actionSaveProfile2As = 0;
    actionSaveProfile1As = 0;
    actionLoadProfile2 = 0;
    actionLoadProfile1 = 0;
    actionSaveSeqAs = 0;
    actionAppendSequences = 0;
    actionLoadSequences = 0;
    profile2Viewer = 0;
    alignmentViewer = 0;
    horizontalGroupBox = 0;
    layoutHbox = 0;
    sideFiller = 0;
    alignModeLabel = 0;
    fontLabel = 0;
    profileScrollLock = 0;
    fontMenu = 0;
    alignmentModeMenu = 0;
    clustalObj = 0;
    lowScoreParams = 0;
    colScoreParams = 0;
    pairwiseParams = 0;
    alignmentParams = 0;
    lowScoreResProfile2Viewer = 0;
    lowScoreSeqWeightProfile2Viewer = 0;
    lowScoreResAlignmentViewer = 0;
    lowScoreSeqWeightAlignmentViewer = 0;
    bottomInfoLabel = 0;
    w = 0;
    mainlayout = 0;
}

/* nige 25-04-07: handler for the WHOLE application. */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    KeyController *kbd = KeyController::Instance();
    if (kbd->consumeKeyPress(event->key()))
        return;
    QWidget::keyPressEvent(event);        
}

/* nige 25-04-07: handler for the WHOLE application. */
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    KeyController *kbd = KeyController::Instance();
    if (kbd->consumeKeyRelease(event->key()))
        return;
    QWidget::keyReleaseEvent(event);
}

// Mark 16-5-07
void MainWindow::setUPGMA()
{
    actionUseUPGMA->setChecked(true);
    actionUseNJ->setChecked(false);
    clustalw::userParameters->setClusterAlgorithm(clustalw::UPGMA);
    actionBootstrapNJTree->setEnabled(false); // Mark 1-9-2007.
}

// Mark 16-5-07
void MainWindow::setNJ()
{
    actionUseUPGMA->setChecked(false);
    actionUseNJ->setChecked(true);
    clustalw::userParameters->setClusterAlgorithm(clustalw::NJ);
    actionBootstrapNJTree->setEnabled(true); // Mark 1-9-2007.
}

// Mark 16-5-07
void MainWindow::setNoIteration() 
{
    clustalw::userParameters->setDoRemoveFirstIteration(clustalw::NONE);
    actionNoIteration->setChecked(true);
    actionTreeIteration->setChecked(false);
    actionAlignIteration->setChecked(false);
}

// Mark 16-5-07
void MainWindow::setTreeIteration() 
{
    // Andreas Wilm (UCD) 2008-03-10:
    // warn if "resetting gaps" is enabled
    if (clustalw::userParameters->ResetGapsIsEnabled()) {
        QMessageBox::warning(this, tr("Error"), 
                             tr("Error: \"Iteration\" in combination with \"Reset gaps\" is not possible."));
        setNoIteration();
        return;
    }
    
    clustalw::userParameters->setDoRemoveFirstIteration(clustalw::TREE);
    actionNoIteration->setChecked(false);
    actionTreeIteration->setChecked(true);
    actionAlignIteration->setChecked(false);
}

// Mark 16-5-07
void MainWindow::setAlignIteration() 
{
    // Andreas Wilm (UCD) 2008-03-10:
    // warn if "resetting gaps" is enabled
    if (clustalw::userParameters->ResetGapsIsEnabled()) {
        QMessageBox::warning(this, tr("Error"), 
                             tr("Error: \"Iteration\" in combination with \"Reset gaps\" is not possible."));
        setNoIteration();
        return;
    }
    
    
    clustalw::userParameters->setDoRemoveFirstIteration(clustalw::ALIGNMENT);
    actionNoIteration->setChecked(false);
    actionTreeIteration->setChecked(false);
    actionAlignIteration->setChecked(true);
}


void MainWindow::toplevelMenusEnabled(bool b)
{
    list<QMenu *>::iterator i;
    QMenu *menu;
    for (i=toplevelMenuList.begin(); i != toplevelMenuList.end(); ++i)
    {
        menu = *i;
        menu->setEnabled(b);
    }
}


