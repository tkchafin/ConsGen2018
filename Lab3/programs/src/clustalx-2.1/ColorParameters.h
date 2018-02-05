#ifndef COLORPARAMETERS_H
#define COLORPARAMETERS_H

#include <string>
#include <vector>
#include <map>

#include <QString>
#include <QChar>
#include <QColor>
#include "HardCodedColorScheme.h"

class ColorParameters
{   
    public:
        ColorParameters();
        void addColor(QString colorName, int red, int green, int blue);
        void addConsensusCondition(QChar name, int cutOff, QString res);
        void addColorRule(QChar res, QString colorName, QString conditions);
        void clear();
        int getNumColors(){return colors.size();};
        void useHardCodedColors();
        void useDefaultColorRules();
        QColor getResidueColor(QChar res, QChar consensus);
        int getResidueColorNum(QChar res, QChar consensus);
        QColor getColorFromNum(int colorNum);
        QColor getDefaultColor(){return defaultColor;}
        int getNumConsensusParams();
        int getDefaultIndex(){return 0;}
        std::vector<consensusParams>* getConPar(){return &conPar;}
        std::vector<residueColors> getColorRules(){return colorPar;}
        void addAllRulesTogether(std::vector<residueColors> colPar);
        void addConsensusParams(std::vector<consensusParams> _conPar){conPar = _conPar;}
        void printColorInfo();
    private:
        int getIndexOfColor(QColor col);
        //std::map<QString, QColor> colors; // Stores the colors from @rgbindex OR hard coded!!
        std::vector<std::pair<QString, QColor> >colors;
        std::vector<residueColors> colorPar; 
        std::vector<consensusParams> conPar;
        QColor defaultColor;
};

#endif
