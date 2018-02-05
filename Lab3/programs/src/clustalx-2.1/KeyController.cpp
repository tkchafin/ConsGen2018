/**
 * Implements a singleton that maintains keyboard state. The single instance
 * is (re)instantiated on demand like:
 *     KeyController *kbd = KeyController::Instance();
 *
 * 25-04-07,Nigel Brown(EMBL): created.
 */

#include "KeyController.h"
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <iostream>
using namespace std;

//return the sole instance
KeyController *KeyController::Instance() {
    static KeyController instance;
    return &instance;
}

KeyController::KeyController() :
    keyShift(false), keyCtrl(false)
{
//- qApp->installEventFilter(this);
}

bool KeyController::consumeKeyPress(int key) {
    switch(key) {
    case Qt::Key_Shift:
        //printf("KeyController: shift down\n");
        return keyShift = true;
    case Qt::Key_Control:
        //printf("KeyController: ctrl down\n");
        return keyCtrl = true;
    }
    return false;
}

bool KeyController::consumeKeyRelease(int key) {
    switch(key) {
    case Qt::Key_Shift:
        //printf("KeyController: shift up\n");
        return !(keyShift = false);
    case Qt::Key_Control:
        //printf("KeyController: ctrl up\n");
        return !(keyCtrl = false);
    }
    return false;
}

/* nige: another approach - overkill! */
//-
//- bool KeyController::eventFilter(QObject *, QEvent *e)
//- {
//-     //printf("KeyController::eventFilter(%0x, %d)\n", (int)this, e->type());
//- 
//-     /* nige: detect shift and ctrl keys BEFORE this widget gets focus */
//-     if (e->type() == QEvent::KeyPress) {
//-         QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
//-         if (keyEvent->key() == Qt::Key_Shift) {
//-             cout << "shift pressed\n";
//-             keyShift = true;
//-         }
//-         if (keyEvent->key() == Qt::Key_Control) {
//-             cout << "ctrl pressed\n";
//-             keyCtrl = true;
//-         }
//-     }
//- 
//-     /* nige: detect shift and ctrl keys BEFORE this widget gets focus */
//-     if (e->type() == QEvent::KeyRelease) {
//-         QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
//-         if (keyEvent->key() == Qt::Key_Shift) {
//-             cout << "shift released\n";
//-             keyShift = false;
//-         }
//-         if (keyEvent->key() == Qt::Key_Control) {
//-             cout << "ctrl released\n";
//-             keyCtrl = false;
//-         }
//-     }
//- 
//-     return false;
//- }
