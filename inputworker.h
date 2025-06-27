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

signals:
    void validInputDetected(int button_value);

protected:
    void run() override {
        int button_value;
        while (!isInterruptionRequested()) {
           
            button_value = controller->CheckControllerEvent();
            if (button_value != -1) {
                emit validInputDetected(button_value);
            }
            msleep(100); // Delay to avoid CPU overload
        }
        std::cout << "finished checking:::::::::::::::\n";
    }
};

#endif // INPUTTHREAD_H