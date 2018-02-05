/**
 * Implements a singleton that maintains keyboard state. The single instance
 * is (re)instantiated on demand like:
 *     KeyController *kbd = KeyController::Instance();
 *
 * 25-04-07,Nigel Brown(EMBL): created.
 */

#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

#include <QObject>
#include <QKeyEvent>
#include <QEvent>

class KeyController : public QObject {
public:
    /* return the KeyController singleton */
    static KeyController *Instance();

    /* resetters */
    void clear()      { keyShift = keyCtrl = false; };
    void clearShift() { keyShift = false; };
    void clearCtrl()  { keyCtrl  = false; };

    /* setters */
    bool consumeKeyPress(int key);
    bool consumeKeyRelease(int key);

    /* testers */
    bool shiftPressed()  { return keyShift; }
    bool shiftReleased() { return !keyShift; }
    bool ctrlPressed()   { return keyCtrl; }
    bool ctrlReleased()  { return !keyCtrl; }

protected:
    /* hide the constructors */
    KeyController();
    KeyController(const KeyController&);
    KeyController& operator= (const KeyController&);

    //- bool eventFilter(QObject *o, QEvent *e);

private:
    /* keyboard state */
    bool keyShift;
    bool keyCtrl;
};

#endif //KEYCONTROLLER_H
