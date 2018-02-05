/**
 * @author Mark Larkin, Conway Institute, UCD. mark.larkin@ucd.ie
 * Changes:
 *
 * Mark 22-1-2007 The postscript files had spaces instead of gaps '-'. I made a
 * change to the printSeqLine function to fix this.
 *
 * 30-01-07,Nigel Brown(EMBL): Added 'statusdict begin a4 end' for A4
 * paper. Added CLU_LONG_REVISION_STRING in place of hardwired clustalw
 * revision level, included from "clustalW/versionw.h".
 *
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects.  
 ****************************************************************************/
#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <QtGui>
#include "PSPrinter.h"
#include "ColorFileXmlParser.h"
#include "clustalW/general/userparams.h"
#include "clustalW/general/utils.h"
#include "clustalW/clustalw_version.h"

PSPrinter::PSPrinter(psPrintParams* printParams)
 : ConstConvertRGB(255.0),
   blockHeight(0),
   blockLeft(0),
   blockTop(0),
   headerTop(0),
   seqTop(0),
   footerTop(0),
   curveTop(0)
{
    data = printParams;
}

void PSPrinter::writePSFile(PostscriptFileParams* postscriptParams)
{
    QString psFile = postscriptParams->getWriteSeqsToFile(); 
    QString parFile = postscriptParams->getPSColorsFile();
    int pageSize = postscriptParams->getPageSize();
    int orientation = postscriptParams->getOrientation(); 
    bool header = postscriptParams->getPrintHeader(); 
    bool ruler = postscriptParams->getPrintRuler();
    bool resNo = postscriptParams->getPrintResidueNumber();
    bool resize = postscriptParams->getResizeToFitPage();
    int firstPrintRes = postscriptParams->getPrintFromPos(); 
    int lastPrintRes = postscriptParams->getPrintToPos();
    int bLength = postscriptParams->getUseBlockLen();
    bool showCurve = postscriptParams->getPrintQualityCurve();    
    
    int i, j, bn, seq, numSeqs;
    int err;
    int blockLen, numPages;
    int fr, lr;
    int page, row;
    int psRotation = 0, psXTrans = 0, psYTrans = 0;
    float psScale, hScale, wScale;
    int maxSeq;
    int maxX = 0, maxY = 0;
    int scoreHeight = 0;
    int mainHeader = 0;
    int numELines, numECols;
    int nHead, nFoot;
    int pagePixWidth;    // width of the page in pixels
    int pageCharHeight;    // height of the page in chars for sequences
    int pagePixHeight;    // height of the page in pixels for sequences
    int blocksPerPage, numBlocks;
    std::vector<int> resNumber;
    const clustalw::SeqArray* seqArray = data->alignPtr->getSeqArray();
    int gapPos1 = clustalw::userParameters->getGapPos1(); // Mark: 22-1-2007 new local
    int gapPos2 = clustalw::userParameters->getGapPos2(); // Mark: 22-1-2007 new local
    
    time_t *tptr = 0, ttime;
    QString strTime;
    
    ofstream psOutFile;
    std::string fileName = psFile.toStdString();

    psOutFile.open(fileName.c_str());
    
    if(!psOutFile.is_open())
    {
        QMessageBox::information(this, "PS file",
                             "Cannot open PS file.");
        return;    
    }
                                  

    // get the page size parameters

    if (pageSize == A4)
    {
        if (orientation == PORTRAIT)
        {
            maxX = A4X;
            maxY = A4Y;
            psRotation = 0;
        }
        else
        {
            maxX = A4Y;
            maxY = A4X;
            psRotation = -90;
        }
    }
    else if (pageSize == A3)
    {
        if (orientation == PORTRAIT)
        {
            maxX = A3X;
            maxY = A3Y;
            psRotation = 0;
        }
        else
        {
            maxX = A3Y;
            maxY = A3X;
            psRotation = -90;
        }
    }
    else if (pageSize == USLETTER)
    {
        if (orientation == PORTRAIT)
        {
            maxX = USLETTERX;
            maxY = USLETTERY;
            psRotation = 0;
        }
        else
        {
            maxX = USLETTERY;
            maxY = USLETTERX;
            psRotation = -90;
        }
    }
    if(showCurve) 
    {
        scoreHeight = SCOREY;
    }
    if(header)
    {
        mainHeader = HEADER;
    }
    else
    {
        mainHeader = NOHEADER;
    }
    
    pagePixWidth = maxX - LEFTMARGIN * 2;
    pagePixHeight = maxY - mainHeader * CHARHEIGHT;


    numSeqs = data->nSeqs;
    nHead = data->seqHeader.size();
    
    if(ruler)
    {
        nFoot = 1;
    }
    else
    {
        nFoot = 0;
    }
    
    numELines = nHead + nFoot + scoreHeight + SEPARATION;
    
    int nCols = data->alignPtr->getLengthLongestSequence(data->firstSeq, data->lastSeq);

    // check the block length, residue range parameters
    if(firstPrintRes <= 0)
    {
        firstPrintRes = 1;
    }
    if((lastPrintRes <= 0) || (lastPrintRes > nCols))
    {
        lastPrintRes = nCols;
    }
    if(firstPrintRes > lastPrintRes)
    {
        QMessageBox::information(this, "PS file",
                             "Bad residue range - cannot write postscript");
        return;
    }
    if (bLength==0 || lastPrintRes - firstPrintRes + 1 < bLength)
    { 
        blockLen = lastPrintRes - firstPrintRes + 1;
    }
    else
    {
        blockLen = bLength;
    }
    
    QChar res;
    resNumber.resize(data->nSeqs + 1, 0);
    
    // NOTE need to be careful with this. We want to start at firstSeq not 0!!!!
    for(i = data->firstSeq; i < data->nSeqs + data->firstSeq; i++)
    {
        for(j = 1; j < firstPrintRes - 1; j++)
        {
            if(j < data->alignPtr->getSeqLength(i))
            {
                res = QChar((*seqArray)[i][j+1]);
                if(res.isLetter() && resNumber.size() > i - data->firstSeq) 
                {
                    resNumber[i - data->firstSeq]++;
                }
            }
        }
    }
    int maxNames = data->alignPtr->getMaxNames();
    if(resNo)
    {
        numECols = MAXRESNO + 1 + maxNames;
    }
    else
    {
        numECols = 1 + maxNames;
    }
    // print out the PS revision level etc.
    ttime = time(tptr);
    strTime = QString(ctime(&ttime));
    
    printPSInfo(&psOutFile, pageSize);
            
    if(ruler)
    {
        makeRuler();
    }
    char cRes;
    // calculate scaling factors, block sizes to fit the page etc.

    if (resize == false || blockLen == lastPrintRes - firstPrintRes + 1)
    {
    // split the alignment into blocks of sequences. If the blocks are too long
    // for the page - tough!
        if(resize == false)
        {
            psScale = 1.0;
        }
        else
        {
            psScale = static_cast<float>(pagePixWidth) / 
                      static_cast<float>((blockLen + numECols) * CHARWIDTH);
        }
        psXTrans = LEFTMARGIN * (1 - psScale);
        psYTrans = pagePixHeight * (1 - psScale);
        
        if (pageSize != A3 && orientation == LANDSCAPE)
        {
            psXTrans -= LEFTMARGIN;
        }
        
        if(psScale == 0.0)
        {
            return; 
        }        
        pageCharHeight = ((maxY / CHARHEIGHT) - mainHeader) / psScale;
        maxSeq = pageCharHeight - numELines;
        if(maxSeq == 0)
        {
            return; // NOTE this is to fix an arithmetic exception I was getting.
                    // I will need to figure out why this was.
        }
        blockHeight = (maxSeq + numELines) * CHARHEIGHT;
        numPages = (numSeqs / maxSeq) + 1;
        seq = 0;
        for (page = 0; page < numPages; page++)
        {
        // print the top of page header
            printPageHeader(&psOutFile, psRotation, maxX, maxY,
                            page, numPages, header, strTime,
                            psFile, psXTrans, psYTrans, psScale);

            blockTop = maxY - mainHeader * CHARHEIGHT;
            blockLeft = LEFTMARGIN + (1 + maxNames) * CHARWIDTH; 
            headerTop = blockTop;

            fr = firstPrintRes - 1;
            lr = lastPrintRes - 1;
            //  show the header lines
            for (i = 0; i < nHead; i++)
            {
                printHeaderLine(&psOutFile, i, fr, lr);
            }
            
            seqTop = blockTop - nHead * CHARHEIGHT;

            int resCode; // New local variable to improve readability
            //  show the sequence lines
            for (row = 0; row < maxSeq; row++)
            {
                if(resNo)
                {
                    for(i = fr; i <= lr; i++)
                    {                        
                        if(i < data->alignPtr->getSeqLength(seq + data->firstSeq))
                        {
                            resCode = (*seqArray)[seq + data->firstSeq][i+1];
                            cRes = clustalw::userParameters->getAminoAcidCode(resCode);
                            
                            res = QChar(cRes);
                            
                            if(res.isLetter() && resNumber.size() > seq) 
                            {
                                resNumber[seq]++;
                            }
                        }                        
                    }
                }
                printSeqLine(&psOutFile, row, seq, fr, lr, resNumber[seq]);
                seq++;
                if(seq >= numSeqs)
                {
                    row++;
                    break;
                }
            }

            footerTop = seqTop - row * CHARHEIGHT;
            //  show the footer lines
            if(nFoot != 0)
            {
                printFooterLine(&psOutFile, fr, lr);
            }
            curveTop = footerTop - nFoot * CHARHEIGHT;
            // show the quality curve
            if(showCurve)
            {
                printQualityCurve(&psOutFile, fr, lr, scoreHeight);
            }
            psOutFile << "\nshowpage\n";
            psOutFile << "restore\n";
        }
    }
    else
    {
    // split the alignment into blocks of residues, and scale the blocks to fit the page
        maxSeq = pagePixHeight / CHARHEIGHT - numELines - mainHeader;
        hScale = static_cast<float>(maxSeq) / static_cast<float>(numSeqs);
        wScale = static_cast<float>(pagePixWidth) /
                 static_cast<float>((blockLen+numECols)*CHARWIDTH);
                 
        psScale = clustalw::utilityObject->MIN(hScale, wScale);
        psXTrans = LEFTMARGIN * (1 - psScale);
        psYTrans = pagePixHeight * (1 - psScale);
        
        if (pageSize != A3 && orientation == LANDSCAPE)
        {
            psXTrans -= LEFTMARGIN;
        }
        
        pageCharHeight = ((maxY / CHARHEIGHT) - mainHeader) / psScale;
        maxSeq = pageCharHeight - numELines;
        blockHeight = (numSeqs + numELines) * CHARHEIGHT;
        blocksPerPage = pageCharHeight / (numSeqs + numELines);
        
        if (blocksPerPage == 0)
        {
            clustalw::utilityObject->error("illegal combination of print parameters");
            return;
        }
        
        numBlocks = (lastPrintRes - firstPrintRes) / blockLen + 1;
        
        if (numBlocks % blocksPerPage == 0)
        {
            numPages = numBlocks / blocksPerPage;
        }
        else
        {
            numPages = numBlocks / blocksPerPage + 1;
        }
        
        for (bn = 0; bn < numBlocks; bn++)
        {
            page = bn / blocksPerPage;
            // print the top of page header 
            if (bn % blocksPerPage == 0)
            {
                printPageHeader(&psOutFile, psRotation, maxX, maxY,
                                page, numPages, header, strTime,
                                psFile, psXTrans, psYTrans, psScale);                
            }
            
            blockTop = maxY - mainHeader * CHARHEIGHT - blockHeight * (bn % blocksPerPage);
            blockLeft = LEFTMARGIN + (1 + maxNames) * CHARWIDTH; 
            headerTop = blockTop;
            seqTop = blockTop - nHead * CHARHEIGHT;
            footerTop = blockTop - (nHead + numSeqs) * CHARHEIGHT;
            curveTop = blockTop - (nHead + numSeqs + nFoot) * CHARHEIGHT;

            fr = firstPrintRes - 1 + blockLen * bn;
            lr = fr + blockLen - 1;
            if(lr >= lastPrintRes)
            { 
                lr = lastPrintRes - 1;
            }
            //  show the header lines
            for (i = 0; i < nHead; i++)
            {
                printHeaderLine(&psOutFile, i, fr, lr);
            }
            
            //  show the sequence lines
            for (i = 0; i < numSeqs; i++)
            {
                row = i % maxSeq;
                if(resNo)
                {
                    for(j = fr; j <= lr; j++)
                    {
                        if(j < data->alignPtr->getSeqLength(i + data->firstSeq))
                        {
                            cRes = clustalw::userParameters->getAminoAcidCode((*seqArray)[i + data->firstSeq][j+1]);
                            
                            res = QChar(cRes);                            
                            if(res.isLetter() && resNumber.size() > i) 
                            {
                                resNumber[i]++;
                            }
                        }                         
                    }
                }
                printSeqLine(&psOutFile, row, i, fr, lr, resNumber[i]);
            }
            
            if(nFoot != 0)
            {
                printFooterLine(&psOutFile, fr, lr);
            }
            
            // show the quality curve
            if(showCurve)
            {
                printQualityCurve(&psOutFile, fr, lr, scoreHeight);
            }

            if ((bn == (numBlocks-1)) || ((bn % blocksPerPage == blocksPerPage-1)))
            {
                psOutFile << "\nshowpage\n";
                psOutFile << "restore\n";
            }
        }
    }
    psOutFile.close();
    return;
}        

void PSPrinter::printPageHeader(ofstream *psOutFile, int psRotation,int maxX,int maxY,
            int page, int numPages, bool header, QString strTime,
            QString psFile, int psXTrans, int psYTrans, float psScale)
{
    int psX,psY;
    QString tempStr;
    // NOTE this is in clustalw!!!!!!
    //QString revisionLevel = "ClustalW 2.1"; //nige
    // QString revisionLevel = CLU_LONG_VERSION_STRING;
    //
    // Andreas: changes according to autogenerated config.h in
    // clustalw. Why is ClustalW instead of X needed here?
    // 
    QString revisionLevel =  CLUSTALW_VERSION;

    (*psOutFile) << "%%Page: " << page << " " << page << "\n";
    (*psOutFile) << "save\n\n";

    if (psRotation == -90)
    {
        (*psOutFile) << "0 " << maxX << " translate\n";
        (*psOutFile) << psRotation << " rotate\n";
    }

    if (header)
    {
        tempStr = "CLUSTAL " + revisionLevel + " MULTIPLE SEQUENCE ALIGNMENT";
        psX = (maxX - tempStr.length() * 10) / 2;
        psY = maxY - 2 * CHARHEIGHT;
        (*psOutFile) << psX << " "<< psY << " moveto\n";
        (*psOutFile) << "/Times-Bold findfont 14 scalefont setfont\n";
        (*psOutFile) << "(" << tempStr.toStdString() << ") show\n\n";

        psX = 20;
        psY = maxY - 4 * CHARHEIGHT;
        (*psOutFile) << psX << " " << psY << " moveto\n";
        (*psOutFile) << "(File: " << psFile.toStdString() << ") show\n\n";

        tempStr = "Date: " + strTime;
        psX = maxX - tempStr.length() * 8 - 20;
        psY = maxY - 4 * CHARHEIGHT;
        (*psOutFile) << psX << " " << psY << " moveto\n";
        (*psOutFile) << "(" << tempStr.toStdString() << ") show\n\n";

        QString message = "Page ";
        QString num;
        num.setNum(page+1);
        message += num + " of ";
        num.setNum(numPages);
        message += num;
        psX = 20;
        psY = maxY - 5 * CHARHEIGHT - 4;
        (*psOutFile) << psX << " " << psY << " moveto\n";
        (*psOutFile) << "(" << message.toStdString() << ") show\n\n";
    }
    (*psOutFile) << psXTrans << " " << psYTrans << " translate\n";
    (*psOutFile) << std::setprecision(2) << psScale << " " 
                 << std::setprecision(2) << psScale << " scale\n";
    (*psOutFile) << "/Courier-Bold findfont 10 scalefont setfont\n";
}            


void PSPrinter::printPSInfo(ofstream *psOutFile, int pageSize)
{       
    (*psOutFile) << "%!PS-Adobe-1.0\n";
    (*psOutFile) << "%%Creator: Julie Thompson\n";
    (*psOutFile) << "%%Title:ClustalX Alignment\n";
    (*psOutFile) << "%%EndComments\n";
    (*psOutFile) << "/box { newpath\n";
    (*psOutFile) << "\t-0 -3 moveto\n";
    (*psOutFile) << "\t-0 " << CHARHEIGHT-3 << " lineto\n";
    (*psOutFile) << "\t" << CHARWIDTH << " " << CHARHEIGHT-3 << " lineto\n";
    (*psOutFile) << "\t" << CHARWIDTH << " -3 lineto\n";
    (*psOutFile) << "\tclosepath\n";
    (*psOutFile) << "      } def\n\n";

    (*psOutFile) << "/color_char { gsave\n";
    (*psOutFile) << "\tsetrgbcolor\n";
    (*psOutFile) << "\tmoveto\n";
    (*psOutFile) << "\tshow\n";
    (*psOutFile) << "\tgrestore\n";
    (*psOutFile) << "      } def\n\n";

    (*psOutFile) << "/cbox { gsave\n";
    (*psOutFile) << "\ttranslate\n";
    (*psOutFile) << "\tnewpath\n";
    (*psOutFile) << "\t0 0 moveto\n";
    (*psOutFile) << "\tlineto\n";
    (*psOutFile) << "\tlineto\n";
    (*psOutFile) << "\tlineto\n";
    (*psOutFile) << "\tclosepath\n";
    (*psOutFile) << "\tfill\n";
    (*psOutFile) << "\tgrestore\n";
    (*psOutFile) << "      } def\n\n";

    (*psOutFile) << "/color_inv { gsave\n";
    (*psOutFile) << "\tsetrgbcolor\n";
    (*psOutFile) << "\ttranslate\n";
    (*psOutFile) << "\tbox fill\n";
    (*psOutFile) << "\tgrestore\n";
    (*psOutFile) << "\tmoveto\n";
    (*psOutFile) << "\tshow\n";
    (*psOutFile) << "      } def\n\n";

    (*psOutFile) << "/white_inv { gsave\n";
    (*psOutFile) << "\tsetrgbcolor\n";
    (*psOutFile) << "\ttranslate\n";
    (*psOutFile) << "\tbox fill\n";
    (*psOutFile) << "\tgrestore\n";
    (*psOutFile) << "\tgsave\n";
    (*psOutFile) << "\tsetrgbcolor\n";
    (*psOutFile) << "\tmoveto\n";
    (*psOutFile) << "\tshow\n";
    (*psOutFile) << "\tgrestore\n";
    (*psOutFile) << "      } def\n\n";

    if (pageSize==A3)
        (*psOutFile) <<"statusdict begin a3 end\n\n";
    else if (pageSize==A4)
        (*psOutFile) <<"statusdict begin a4 end\n\n";
/* For canon color printer, use a3tray instead of a3!! */
}

void PSPrinter::printHeaderLine(ofstream *psOutFile, int ix, int fr, int lr)
{
    int i;
    int psX, psY;
    int maxNames = data->alignPtr->getMaxNames();
    
    psX = LEFTMARGIN;
    psY = headerTop - (ix * CHARHEIGHT);
    
    (*psOutFile) << psX << " " << psY << " moveto\n";

    if(data->nameHeader.size() > ix)
    {
        (*psOutFile) << "( "<< setw(maxNames) << data->nameHeader[ix].toStdString() 
                 << " ) show\n";
    }
    
    for(i = fr; i <= lr && i < data->seqHeader[ix].size(); i++)
    {
        psX = blockLeft + (i - fr) * CHARWIDTH; 
        (*psOutFile) << "(";
        (*psOutFile) << data->seqHeader[ix][i].toAscii();
        (*psOutFile) << ") ";
        (*psOutFile) << psX << " " << psY << " " << psX << " " 
                     << psY << " 1.0 1.0 1.0 color_inv\n";
    }
    (*psOutFile) << "\n";
}

/**
 * Mark: Change 22-1-2007. I made some changes to this function as the gaps were being 
 * printed out as spaces instead of '-'
 *
 */
void PSPrinter::printSeqLine(ofstream *psOutFile, int row, int seq, int fr, 
                             int lr, int resNumber)
{
    if(seq + 1 <= data->alignPtr->getNumSeqs())
    {
        int i;
        int psX, psY;
        float red, green, blue;
        int maxNames = data->alignPtr->getMaxNames();
        psX = LEFTMARGIN;
        psY = seqTop - (row * CHARHEIGHT);
        int firstSeq = data->firstSeq;
    
        (*psOutFile) << psX << " " << psY << " moveto\n";
        (*psOutFile) << setw(maxNames) << "(" << data->alignPtr->getName(seq + firstSeq) 
                 << " ) show\n";
        QColor color;
        clustalw::Array2D<QColor>* colorMask = data->colorMask;
        const clustalw::SeqArray* seqArray = data->alignPtr->getSeqArray();
        int resCode = 0; // Mark Change 22-1-2007 added local variable
        int gapPos1 = clustalw::userParameters->getGapPos1(); // Added local
        int gapPos2 = clustalw::userParameters->getGapPos2(); // Added local
        char resToPrint = ' ';
        
        for(i = fr; i <= lr; i++)
        {
            if(seq < colorMask->getRowSize() && i < colorMask->getColSize() &&
               i < data->alignPtr->getSeqLength(seq + firstSeq))
            {
                color = (*colorMask)[seq][i];
                red = static_cast<float>(color.red()) / ConstConvertRGB;
                green = static_cast<float>(color.green()) / ConstConvertRGB;
                blue = static_cast<float>(color.blue()) / ConstConvertRGB;
                
                if(!data->backGroundColors && red == 1 && green == 1 && blue == 1)
                {
                    red = blue = green = 0;
                }
                psX = blockLeft + (i - fr) * CHARWIDTH;
                resCode = (*seqArray)[seq + firstSeq][i + 1];
                
                /**
                 * Mark change 22-1-2007
                 * Put in the change here to print out '-' instead of gaps.
                 */
                if(resCode == gapPos1 || resCode == gapPos2)
                {
                    resToPrint = '-';    
                }
                else
                {
                    resToPrint = clustalw::userParameters->getAminoAcidCode(resCode);
                }
                
                (*psOutFile) << "(";
                (*psOutFile) << resToPrint;
                (*psOutFile) << ") ";
            
                // I think segment exception is the low scoring segs  
                // NOTE make sure that we check if we are in range!!!!!!!!  
                if(data->showLowScoreRes && data->lowScoreRes.getRowSize() > seq &&
                   data->lowScoreRes.getColSize() > i && data->lowScoreRes[seq][i] > 0)
                {
                    (*psOutFile) << psX << " " << psY << " " << setprecision(1) << 1.0 
                                 << " " << setprecision(1) << 1.0 
                                 << " " << setprecision(1) << 1.0 << " " << psX << " " 
                                 << psY << " " << setprecision(1) << 0.1 
                                 << " " << setprecision(1) << 0.1 << " " 
                                 << setprecision(1) << 0.1 << " white_inv\n";
                }
                else if(data->showExRes && data->exRes.getRowSize() > seq &&
                        data->exRes.getColSize() > i && data->exRes[seq][i] == 1)
                {
                    (*psOutFile) << psX << " " << psY << " " << setprecision(1) << 1.0 
                                 << " " << setprecision(1) << 1.0 
                                 << " " << setprecision(1) << 1.0 << " " << psX << " " 
                                 << psY << " " << setprecision(1) << 0.4 
                                 << " " << setprecision(1) << 0.4 << " " 
                                 << setprecision(1) << 0.4 << " white_inv\n";                
                }
                else
                {
                    if(data->backGroundColors)
                    {
                        (*psOutFile) << psX << " " << psY << " " << psX << " " << psY 
                                     << " " << setprecision(1) << red << " " << setprecision(1) << green << " " 
                                     << setprecision(1) << blue << " color_inv\n";
                    }
                    else
                    {
                        (*psOutFile) << psX << " " << psY << " " << setprecision(1) 
                                     << red << " " 
                                     << setprecision(1) << green << " " << setprecision(1) 
                                     << blue << " color_char\n";
                    }
                }
            }
        }

        if(resNumber > 0)
        {
            psX = blockLeft + (lr - fr + 1) * CHARWIDTH; 
            psY = seqTop - (row * CHARHEIGHT);
            (*psOutFile) << psX << " " << psY << " moveto\n";
            (*psOutFile) << "(" << setw(MAXRESNO) << resNumber << ") show\n";
        }
        (*psOutFile) << "\n";
    }
}

void PSPrinter::printFooterLine(ofstream *psOutFile, int fr,int lr)
{
    int i;
    int psX, psY;

    psX = LEFTMARGIN;
    psY = footerTop; //- (ix * CHARHEIGHT);
    (*psOutFile) << psX << " " << psY << " moveto\n";
    (*psOutFile) << "(" << setw(data->alignPtr->getMaxNames()) << " ) show\n";
    
    for(i = fr; i <= lr && i < ruler.length(); i++)
    {
        psX = blockLeft + (i - fr) * CHARWIDTH; 
        (*psOutFile) <<  "(";
        (*psOutFile) << ruler[i];
        (*psOutFile) << ") ";
        (*psOutFile) << psX << " " << psY << " " << psX << " " << psY 
                     << " 1.0 1.0 1.0 color_inv\n";
    }
    
    (*psOutFile) << "\n";
}

void PSPrinter::makeRuler()
{
    int i,j;
    string marker;
    int markerLen;
    int length = data->alignPtr->getLengthLongestSequence(data->firstSeq, data->lastSeq);
    ruler = "";
    ruler.resize(length + 1, ' ');
    ruler[0] = '1';
    
    for (i = 1; i < length; i++)
    {
        if ((i + 1) % 10 > 0)
        {
            ruler[i] = '.';
        }
        else
        {
            marker = "";
            
            std::stringstream ss;
            ss << ((i + 1) / 10) * 10;
            ss >> marker;
            
            markerLen = marker.length();
            
            for (j = 0; j < markerLen && (i + 1 + j - markerLen) < length; j++)
            {
                ruler[i + 1 + j - markerLen] = marker[j];
            }
        }
    }
    cout << ruler << "\n";
}

void PSPrinter::printQualityCurve(ofstream *psOutFile, int fr, int lr, int scoreHeight)
{
    int i, w, h;
    int psX, psY, curveBottom;

    w = CHARWIDTH;
    psX = blockLeft + CHARWIDTH;
    curveBottom = curveTop - scoreHeight * CHARHEIGHT;
    
    (*psOutFile) << "0.3 0.3 0.3 setrgbcolor\n";
    
    for(i = fr + 1; i <= lr && i < data->histogram->size(); i++)
    {   
        (*psOutFile) << psX << " " << curveBottom << " moveto\n";
        
        h = scoreHeight * CHARHEIGHT * ((float)(*data->histogram)[i] / 100.0);
        if(h < 1) 
        {
            h = 1;
        }
        (*psOutFile) << w << " 0 " << w << " " << h << " 0 " << h << " " << psX 
                     << " " << curveBottom << " cbox\n";
        psX += CHARWIDTH;
    }
    (*psOutFile) << "0.0 0.0 0.0 setrgbcolor\n";
}
