/**
 * Changes: 
 * 12-4-07, Mark Larkin, Removed destructor. No need to delete QObjects. 
 */
#include "SearchForString.h"
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QVBoxLayout>

SearchForString::SearchForString()
{
    searchString = new QLineEdit("");
    searchFromStart = new QPushButton(tr("Search From Start"));
    searchFromStart->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    searchAgain = new QPushButton(tr("Search Again"));
    searchAgain->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    closeButton = new QPushButton(tr("Close"));
    closeButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    
    connect(searchFromStart, SIGNAL(clicked()), this, SIGNAL(searchFromStartSignal()));
    connect(searchAgain, SIGNAL(clicked()), this, SIGNAL(searchAgainSignal()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(cancel()));
    
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(searchString);
    mainLayout->addWidget(searchFromStart);
    mainLayout->addWidget(searchAgain);
    mainLayout->addWidget(closeButton);
    setLayout(mainLayout);
}

QString SearchForString::getSearchString()
{
    return searchString->text();
}

void SearchForString::cancel()
{
    setResult(QDialog::Rejected);
    close();
}
