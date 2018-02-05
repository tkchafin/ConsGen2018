#include "PostscriptFileParams.h"

PostscriptFileParams::PostscriptFileParams(QString _writeSeqsToFile, QString _psColorsFile,
                int _pageSize, int _orientation, bool _printHeader, bool _printQualityCurve, 
                bool _printRuler, bool _printResidueNumber, bool _resizeToFitPage, 
                int _printFromPos, int _printToPos, int _useBlockLen)
 : writeSeqsToFile(_writeSeqsToFile),
   psColorsFile(_psColorsFile),
   pageSize(_pageSize),
   orientation(_orientation),
   printHeader(_printHeader),
   printQualityCurve(_printQualityCurve),
   printRuler(_printRuler),
   printResidueNumber(_printResidueNumber),
   resizeToFitPage(_resizeToFitPage),
   printFromPos(_printFromPos),
   printToPos(_printToPos),
   useBlockLen(_useBlockLen)
{

}
