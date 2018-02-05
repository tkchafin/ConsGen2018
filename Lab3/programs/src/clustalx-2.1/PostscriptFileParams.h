#ifndef POSTSCRIPTFILEPARAMS_H
#define POSTSCRIPTFILEPARAMS_H
#include <QString>
#include "ClustalQtParams.h"

class PostscriptFileParams
{
    public:
        PostscriptFileParams(QString _writeSeqsToFile, QString _psColorsFile, int _pageSize, 
                             int _orientation, bool _printHeader, bool _printQualityCurve, 
                             bool _printRuler, bool _printResidueNumber, 
                             bool _resizeToFitPage, int _printFromPos, int _printToPos, 
                             int _useBlockLen);
        QString getWriteSeqsToFile(){return writeSeqsToFile;}
        QString getPSColorsFile(){return psColorsFile;}
        int getPageSize(){return pageSize;}
        int getOrientation(){return orientation;}
        bool getPrintHeader(){return printHeader;}
        bool getPrintQualityCurve(){return printQualityCurve;}
        bool getPrintRuler(){return printRuler;}
        bool getPrintResidueNumber(){return printResidueNumber;}
        bool getResizeToFitPage(){return resizeToFitPage;}
        int getPrintFromPos(){return printFromPos;}
        int getPrintToPos(){return printToPos;}
        int getUseBlockLen(){return useBlockLen;}
    private:    
        QString writeSeqsToFile;
        QString psColorsFile;
        int pageSize;
        int orientation;
        bool printHeader;
        bool printQualityCurve;
        bool printRuler;
        bool printResidueNumber;
        bool resizeToFitPage;
        int printFromPos;
        int printToPos;
        int useBlockLen;         
               
};
#endif
