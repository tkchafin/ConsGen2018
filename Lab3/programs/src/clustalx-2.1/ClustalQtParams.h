/**
 * Changes:
 *
 * 30-03-07,Nigel Brown(EMBL): renamed variable 'AlignProf1ToProf2WithTree' to
 * 'AlignProf2ToProf1WithTree' for consistency with clustalx and user
 * interface menu naming.
 */

#ifndef CLUSTALQTPARAMS_H
#define CLUSTALQTPARAMS_H
#include <QColor>
#include <QString>
#include "clustalW/alignment/Alignment.h"
#include <vector>

static QString psColorFile = "colprot.xml";
static QString psDNAFile = "coldna.xml";
static QString psPrintFile = "colprint.xml";

static const int BlackAndWhite = 1;
static const int ColorsBackGround = 2;
static const int ColorsLetters = 3;
//static const int UserColors = 4;

static const int MaxColors = 16;
static const int Simple = 0;
static const int Compound = 1;

static const int Sequences = 0;
static const int Profile1 = 1;
static const int Profile2 = 2;

static const int A4 = 0;
static const int A3 = 1;
static const int USLETTER = 2;
        
       
static const int PORTRAIT = 1;
static const int LANDSCAPE = 0;

enum FileNameDialogType
{
    CompleteAlign, AlignFromTree, RealignSelectedSeqs, RealignSeqRange, DoGuideTreeOnly,
    AlignProf2ToProf1, AlignSeqsToProf1, AlignProf2ToProf1WithTree, AlignSeqsToProf1WithTree
};

enum Viewer
{
    SequenceViewer, Profile1Viewer, Profile2Viewer
};

enum AlignType
{
    Sequence, Profile
};
                
struct ColorNamePair
{
    QColor color;
    QString name;
}; 

struct rangeParams
{
    int firstSeq;
    int lastSeq;
    int firstRes;
    int lastRes;    
};

struct residueColors
{
    int type;
    QChar residue;
    QString colorName;
    int length;
    QColor color;
    QString consensusConditionList;
};
        
struct consensusParams
{
    QChar consensusName; // Name of the rule
    int cutOffPercent;
    int length;
    QString cutOffList;
};

struct psPrintParams
{
    int firstSeq;
    int lastSeq;
    int nSeqs;
    clustalw::Alignment* alignPtr;
    bool showLowScoreRes;
    clustalw::Array2D<int> lowScoreRes;
    bool showExRes;
    clustalw::Array2D<int> exRes;
    std::vector<int>* histogram;
    clustalw::Array2D<QColor>* colorMask;
    std::vector<QString> nameHeader; // Should have 2 elements
    std::vector<QString> seqHeader; // Should have 2 elements
    bool backGroundColors;
};        
#endif
