#include "ColorParameters.h"
#include "ClustalQtParams.h"
#include <QColor>
#include <QString>
#include <iostream>
#include "clustalW/general/userparams.h"

ColorParameters::ColorParameters()
{   
    defaultColor = QColor(Qt::white);
}

void ColorParameters::useDefaultColorRules()
{
    bool isDNA = clustalw::userParameters->getDNAFlag();
    
    // Clear the colorPar map
    colorPar.clear();
    QString redRes;
    QString orangeRes;
    QString blueRes;
    QString greenRes;
    
    if(!isDNA)
    {
        // Default simple protein colors!!!!!
        redRes = "krh";
        orangeRes = "gpst";
        blueRes = "fwy";
        greenRes = "ilmv";
    }
    else
    {
        // Default simple DNA colors!!!
        redRes = "a";
        orangeRes = "c";
        blueRes = "tu";
        greenRes = "g";
    }

    // Now add the rules!!!!!!!!!!!!    
    for(int i = 0; i < redRes.length(); i++)
    {
        addColorRule(redRes[i], "RED", "");
    }
    for(int i = 0; i < orangeRes.length(); i++)
    {
        addColorRule(orangeRes[i], "ORANGE", "");
    }
    for(int i = 0; i < blueRes.length(); i++)
    {
        addColorRule(blueRes[i], "BLUE", "");
    }  
    for(int i = 0; i < greenRes.length(); i++)
    {
        addColorRule(greenRes[i], "GREEN", "");
    }          
}

void ColorParameters::addColor(QString colorName, int red, int green, int blue)
{      
    QColor colorToAdd(red, green, blue);
    colors.push_back(std::pair<QString, QColor>(colorName, colorToAdd));
}

void ColorParameters::addConsensusCondition(QChar name, int cutOff, QString res)
{
    consensusParams paramToAdd;
    paramToAdd.consensusName = name;
    paramToAdd.cutOffPercent = cutOff;
    paramToAdd.cutOffList = res;
    paramToAdd.length = res.length();
    conPar.push_back(paramToAdd);
}

void ColorParameters::addColorRule(QChar res, QString colorName, QString conditions)
{
    residueColors paramToAdd;
    paramToAdd.residue = res;
    paramToAdd.consensusConditionList = conditions;
    paramToAdd.length = conditions.length();
    paramToAdd.colorName = colorName;
    
    if(paramToAdd.length > 0)
    {
        paramToAdd.type = Compound;
    }
    else
    {
        paramToAdd.type = Simple;
    }
    
    //std::map<QString, QColor>::iterator colorIterator;
    std::vector<std::pair<QString, QColor> >::iterator colorIterator;
    colorIterator = colors.begin();//colors.find(colorName);
    
    while(colorIterator != colors.end())
    {
        if(colorIterator->first == colorName)
        {
            break;
        }
        colorIterator++;
    }
    
    if(colorIterator == colors.end())
    {
        // If it didnt find the color name in the map!!!!
        paramToAdd.color = QColor(defaultColor);
    }
    else
    {
        paramToAdd.color = QColor(colorIterator->second);
    }
    colorPar.push_back(paramToAdd);
}

void ColorParameters::addAllRulesTogether(std::vector<residueColors> colPar)
{
    // Clear old rules
    colorPar.clear();
    
    // add each color
    std::vector<residueColors>::iterator begin = colPar.begin();
    std::vector<residueColors>::iterator end = colPar.end();
    
    for(; begin != end; begin++)
    {
        addColorRule(begin->residue, begin->colorName, begin->consensusConditionList);
    }
}

/**
 * If we cannot find the @rgbindex tag in the file, we use the default colors.
 * These colors are specified in the HardCodedColorScheme class.
 */
void ColorParameters::useHardCodedColors()
{
    HardCodedColorScheme hardCodedColors;
    colors.clear();
    
    const std::vector<ColorNamePair>* colorScheme = hardCodedColors.getHardCodedScheme();
    
    int numOfColors = colorScheme->size();
    
    for(int i = 0; i< numOfColors; i++)
    {
        colors.push_back(std::pair<QString, QColor>(QString((*colorScheme)[i].name), 
                                                  QColor((*colorScheme)[i].color)));
    }
}

QColor ColorParameters::getResidueColor(QChar res, QChar consensus)
{
    int i, j;
    QChar colorParRes;
    char showRes = res.toAscii();
    char showCon = consensus.toAscii();
    
    for(i = 0; i < colorPar.size(); i++)
    {
        colorParRes = colorPar[i].residue;
        if(res.toLower() == colorParRes.toLower())
        {
            if(colorPar[i].type == Simple)
            {
                return colorPar[i].color;
            }
            else if(colorPar[i].type == Compound)
            {
                
                for(j = 0; j < colorPar[i].consensusConditionList.size(); j++)
                {
                    char temp = colorPar[i].consensusConditionList[j].toAscii();
                    if(consensus == colorPar[i].consensusConditionList[j])
                    {
                        return colorPar[i].color;
                    }
                }
            }
            else
            {
                return defaultColor;
            }
        }
    }
    return defaultColor;
}

int ColorParameters::getResidueColorNum(QChar res, QChar consensus)
{
    int i, j;
    QChar colorParRes;
    char showRes = res.toAscii();
    char showCon = consensus.toAscii();
    
    for(i = 0; i < colorPar.size(); i++)
    {
        colorParRes = colorPar[i].residue;
        if(res.toLower() == colorParRes.toLower())
        {
            if(colorPar[i].type == Simple)
            {
                return getIndexOfColor(colorPar[i].color);
            }
            else if(colorPar[i].type == Compound)
            {
                
                for(j = 0; j < colorPar[i].consensusConditionList.size(); j++)
                {
                    char temp = colorPar[i].consensusConditionList[j].toAscii();
                    if(consensus == colorPar[i].consensusConditionList[j])
                    {
                        return getIndexOfColor(colorPar[i].color);
                    }
                }
            }
            else
            {
                return 0;
            }
        }
    }
    return 0;    
}

void ColorParameters::printColorInfo()
{
    for(int i = 0; i < colors.size(); i++)
    {
        cout << "Color index = " << i + 1 << " Color name is " 
             << colors[i].first.toStdString() << "\n";
    }
}

int ColorParameters::getIndexOfColor(QColor col)
{
    for(int i = 0; i < colors.size(); i++)
    {
        if(colors[i].second == col)
        {
            //cout << "the color is " << colors[i].first.toStdString() << "\n";
            return i + 1;
        }
    }
    return 0; // Not found
}

QColor ColorParameters::getColorFromNum(int colorNum)
{
    if(colorNum > 0 && colorNum - 1 < colorPar.size())
    {
        //cout << "color " << colorNum << " is " 
        //     << colors[colorNum - 1].second
        return colors[colorNum - 1].second;
    }
    else
    {
        return defaultColor;
    }
}

int ColorParameters::getNumConsensusParams()
{
    return conPar.size();
}

void ColorParameters::clear()
{
    colors.clear();
    colorPar.clear();
    conPar.clear();
}
