#ifndef HARDCODEDCOLORSCHEME_H
#define HARDCODEDCOLORSCHEME_H

#include <map>
#include <QColor>
#include <QString>
#include <vector>
#include "ClustalQtParams.h"

class HardCodedColorScheme
{
    public:
        HardCodedColorScheme();
        ~HardCodedColorScheme();
        const std::vector<ColorNamePair>* getHardCodedScheme(){return colors;}
    private:
        std::vector<ColorNamePair>* colors;
};
#endif

