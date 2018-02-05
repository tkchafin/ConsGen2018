#ifndef TREEFORMATOPTIONS_H
#define TREEFORMATOPTIONS_H

#include <QDialog>
class QPushButton;
class QGroupBox;
class QComboBox;
class QLabel;
class QCheckBox;

class TreeFormatOptions : public QDialog
{
    Q_OBJECT
public:
    TreeFormatOptions();

protected:


private slots:
    void accept();

private:
    void setUpLayout();
    void setUpRadioButtons(); 
    void setUpOtherParams();   
    
    QPushButton* okButton;
    QGroupBox* fileGridBox;
    QGroupBox* otherParams;
    QGroupBox* verticalBox;
    
    QCheckBox* clustal;
    QCheckBox* percIdentMat;
    QCheckBox* phylip;
    QCheckBox* phylipDistMat;  
    QCheckBox* nexus; 
    
    QComboBox* bootStrap;
    
    QLabel* bootStrapLabel;
};

#endif
