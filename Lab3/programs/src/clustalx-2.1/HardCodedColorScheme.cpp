#include "HardCodedColorScheme.h"

HardCodedColorScheme::HardCodedColorScheme()
{
    // Set up the colors!
    colors = new std::vector<ColorNamePair>;
    colors->resize(8);
    
    QColor colorToAdd;
    QString name;
    ColorNamePair pairToAdd;
    
    colorToAdd.setRgb(229, 25, 25);
    name = "RED";
    pairToAdd.color = colorToAdd;
    pairToAdd.name = name;
    (*colors)[0] = pairToAdd;

    colorToAdd.setRgb(25, 25, 178);
    name = "BLUE";
    pairToAdd.color = colorToAdd;
    pairToAdd.name = name;
    (*colors)[1] = pairToAdd; 
    
    colorToAdd.setRgb(25, 229, 25);
    name = "GREEN";
    pairToAdd.color = colorToAdd;
    pairToAdd.name = name;
    (*colors)[2] = pairToAdd;   
    
    colorToAdd.setRgb(229, 153, 76);
    name = "ORANGE";
    pairToAdd.color = colorToAdd;
    pairToAdd.name = name;
    (*colors)[3] = pairToAdd;
    
    colorToAdd.setRgb(25, 229, 229);
    name = "CYAN";
    pairToAdd.color = colorToAdd;
    pairToAdd.name = name;
    (*colors)[4] = pairToAdd; 
    
    colorToAdd.setRgb(229, 127, 127);
    name = "PINK";
    pairToAdd.color = colorToAdd;
    pairToAdd.name = name;
    (*colors)[5] = pairToAdd;  
    
    colorToAdd.setRgb(229, 25, 229);
    name = "MAGENTA";
    pairToAdd.color = colorToAdd;
    pairToAdd.name = name;
    (*colors)[6] = pairToAdd;
    
    colorToAdd.setRgb(229, 229, 0);
    name = "YELLOW";
    pairToAdd.color = colorToAdd;
    pairToAdd.name = name;
    (*colors)[7] = pairToAdd;                         
}

HardCodedColorScheme::~HardCodedColorScheme()
{
    delete colors;
}
