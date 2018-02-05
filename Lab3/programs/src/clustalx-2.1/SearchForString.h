/**
 * Changes: 
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. 
 */
#ifndef SEARCHFORSTRING_H
#define SEARCHFORSTRING_H

#include <QDialog>
class QPushButton;
class QGroupBox;
class QLineEdit;
class QVBoxLayout;

class SearchForString : public QDialog
{
    Q_OBJECT
public:
    SearchForString();
    QString getSearchString();
signals:
    void searchFromStartSignal();
    void searchAgainSignal();    
    
protected:


private slots:
    void cancel();
    
private: 
    
    QVBoxLayout* mainLayout;
    
    QPushButton* searchFromStart;
    QPushButton* searchAgain;
    QPushButton* closeButton;
    
    QLineEdit* searchString;
};

#endif
