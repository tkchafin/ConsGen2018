#ifndef COLORFILEXMLPARSER_H
#define COLORFILEXMLPARSER_H

#include <QDomDocument>
#include <QHash>
#include <QIcon>
#include <QTreeWidget>

#include "ColorParameters.h"

class ColorFileXmlParser : public QWidget
{
    Q_OBJECT

public:
    ColorFileXmlParser(bool showMess = true);
    bool read(QIODevice *device);
    ColorParameters* getColorParametersObj(){return &colorParams;}
private slots:


private:
    int parseRGBIndex(const QDomElement &element,
                            QTreeWidgetItem *parentItem = 0);
    void parseConsensus(const QDomElement &element,
                            QTreeWidgetItem *parentItem = 0);
    int parseColorRule(const QDomElement &element,
                            QTreeWidgetItem *parentItem = 0);                        

    QDomDocument domDocument;
    ColorParameters colorParams;
    bool showMessages;
};

#endif
