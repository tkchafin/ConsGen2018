/**
 * Changes: 
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. 
 */
#ifndef PSPRINTER_H
#define PSPRINTER_H
#include <QWidget>
#include <QString>
#include <QFile>
#include <fstream>
#include "ColorParameters.h"
#include "ClustalQtParams.h"
#include "PostscriptFileParams.h"

class PSPrinter : public QWidget
{
    Q_OBJECT
    
    public:
        PSPrinter(psPrintParams* printParams);
        void writePSFile(PostscriptFileParams* postscriptParams);
    private:
        void printPSInfo(ofstream *psOutFile, int pageSize);
        void printPageHeader(ofstream *psOutFile, int psRotation,int maxX,int maxY,
            int page, int numPages, bool header, QString strTime,
            QString psFile, int psXTrans, int psYTrans, float psScale);
        void printHeaderLine(ofstream *psOutFile, int ix,int fr,int lr);
        void printFooterLine(ofstream *psOutFile, int fr,int lr);
        void printQualityCurve(ofstream *psOutFile, 
            int fr,int lr,int scoreHeight);
        void printSeqLine(ofstream *psOutFile, 
            int row,int seq,int fr,int lr,int resNumber);
        void makeRuler(); 
 
        static const int LEFTMARGIN = 20;
        static const int SEPARATION = 2;
        static const int CHARHEIGHT = 10;
        static const int CHARWIDTH = 6;
        static const int A4X = 564;
        static const int A4Y = 800;
        static const int A3X = 832;
        static const int A3Y = 1159;
        static const int USLETTERX = 564;
        static const int USLETTERY = 750;
        static const int SCOREY = 3;
        static const int HEADER = 7;
        static const int NOHEADER = 0;
        static const int MAXRESNO = 6;
        float ConstConvertRGB;
        
        ColorParameters printerColors;
        psPrintParams* data; 
        int blockHeight, blockLeft, blockTop;
        int headerTop, seqTop, footerTop, curveTop;
        std::string ruler; // Will be made by makeRuler.         
};

#endif 
