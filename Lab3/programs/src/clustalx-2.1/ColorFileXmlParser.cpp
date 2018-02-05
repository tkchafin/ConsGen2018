#include <QtGui>

#include "ColorFileXmlParser.h"
#include "ClustalQtParams.h"

ColorFileXmlParser::ColorFileXmlParser(bool showMess)
{
    showMessages = showMess;
}

bool ColorFileXmlParser::read(QIODevice *device)
{
    colorParams.clear();
    
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!domDocument.setContent(device, true, &errorStr, &errorLine,
                                &errorColumn)) 
    {
        if(showMessages)
        {
            QMessageBox::information(window(), tr("Color File Parser"),
                                 tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        }
        return false;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "colorparam") 
    {
        if(showMessages)
        {        
            QMessageBox::information(window(), tr("Color File"),
                                 tr("The file is not a color parameter file."));
        }
        return false;
    }

    int numColors = 0;
    QDomElement rgbIndex = root.firstChildElement("rgbindex");
    if (!rgbIndex.isNull()) 
    {
        numColors = parseRGBIndex(rgbIndex);
    }
    if(numColors == 0)
    {
        // We use the hard coded color index!!!!!
        colorParams.useHardCodedColors();
    }

    QDomElement consensus = root.firstChildElement("consensus");
    if (!consensus.isNull()) // NOTE optional!
    {
        parseConsensus(consensus);
    }
    
    QDomElement colorRules = root.firstChildElement("colorrules");
    int numOfRules = 0;
    if (!colorRules.isNull()) 
    {
        numOfRules = parseColorRule(colorRules);
    }
    if(numOfRules == 0)
    {
        // So we need to use the default colors specified!!!!!!!!!!
        if(showMessages)
        {        
            //QMessageBox::warning(this, tr("Warning"),
            //                     tr("colorrules not found in the parameter file, using\
            //                      defaults!\n"));
        }
        colorParams.useDefaultColorRules();
    }     
    
    return true;
}


int ColorFileXmlParser::parseRGBIndex(const QDomElement &element,
                            QTreeWidgetItem *parentItem)
{
    int numColors = 0;
    QString colorName;
    QString redString, greenString, blueString;
    int red, green, blue;
    bool ok1, ok2, ok3;
    int numColorsSoFar = 0;
    
    // parse the colors!!!!!!!
    QDomElement color = element.firstChildElement("color");
    while (!color.isNull()) 
    {
        numColorsSoFar = colorParams.getNumColors();
                    
        if(numColorsSoFar == MaxColors)
        {
            if(showMessages)
            {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Max number of colors is %1. Not using the rest!\n")
                                     .arg(MaxColors));
            }
            return numColors;
        }        
        // get each colors information!!!!!!!
        if(color.hasAttribute("name") && color.hasAttribute("red") && 
           color.hasAttribute("green") && color.hasAttribute("blue"))
        {
            colorName = color.attribute("name");            
            redString = color.attribute("red");
            greenString = color.attribute("green");
            blueString = color.attribute("blue");
            
            if(colorName.length() > 0 && redString.length() > 0 && greenString.length() > 0
               && blueString.length() > 0)
            {
                red = redString.toInt(&ok1);
                green = greenString.toInt(&ok2);
                blue = blueString.toInt(&ok3);
                
                if(ok1 && ok2 && ok3)
                {
                    colorParams.addColor(colorName, red, green, blue);
                    numColors++;
                }
            }
        }
        color = color.nextSiblingElement("color");
    }
    return numColors;
}                            

void ColorFileXmlParser::parseConsensus(const QDomElement &element, QTreeWidgetItem *parentItem)
{
    QString conditionNameString;
    QChar name;
    int cutOffPercent;
    QString residues;
    QString temp;
    bool ok;
    
    QDomElement condition = element.firstChildElement("condition");
    while (!condition.isNull()) 
    {
        // get each colors information!!!!!!!
        if(condition.hasAttribute("name") && condition.hasAttribute("cutoffpercent") && 
           condition.hasAttribute("residues"))
        {
            conditionNameString = condition.attribute("name");           
            temp = condition.attribute("cutoffpercent");
            residues = condition.attribute("residues");
            
            if(conditionNameString.length() > 0 && temp.length() > 0 && 
               residues.length() > 0)
            {
                
                name = conditionNameString[0];
                cutOffPercent = temp.toInt(&ok);
                
                if(ok)
                { 
                    colorParams.addConsensusCondition(name, cutOffPercent, residues);
                }
            }
        }
        condition = condition.nextSiblingElement("condition");
    }    
}                            

int ColorFileXmlParser::parseColorRule(const QDomElement &element, QTreeWidgetItem *parentItem)
{
    int numOfRules = 0;
    QString colorName;
    QChar res;
    QString conditions;
    
    QString temp;
    
    QDomElement rule = element.firstChildElement("resrule");
    while (!rule.isNull()) 
    {
        // get each colors information!!!!!!!
        if(rule.hasAttribute("residue") && rule.hasAttribute("colorname") && 
           rule.hasAttribute("conditions"))
        {
            colorName = rule.attribute("colorname");           
            temp = rule.attribute("residue");
            conditions = rule.attribute("conditions");
            
            if(colorName.length() > 0 && temp.length() > 0)
            {
                res = temp[0];
                colorParams.addColorRule(res, colorName, conditions);
                numOfRules++;
            }
        }
        rule = rule.nextSiblingElement("resrule");
    }  
    return numOfRules;  
}