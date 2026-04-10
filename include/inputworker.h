#ifndef INPUTTHREAD_H
#define INPUTTHREAD_H

#include <QThread>
#include <QObject>
#include "controller.hpp"
#include <iostream>


class InputThread : public QThread
{
    Q_OBJECT

private:
    Controller* controller;

public:
    InputThread(Controller* c, QObject* parent = nullptr)
        : QThread(parent), controller(c) {
    }
    ~InputThread() {
        controller = nullptr;
    }

signals:
    void ValidInputDetected(PressedButton pressed_button);

protected:
    void run() override {
        PressedButton pressedButton;
        while (!isInterruptionRequested()) {
           
            pressedButton = controller->CheckControllerEvent();

            if (pressedButton.button != Buttons::INVALID_BUTTON) {
                emit ValidInputDetected(pressedButton);

          
            }
        }
        
        std::cout << "finished checking:::::::::::::::\n";
    }
};

#endif // INPUTTHREAD_H