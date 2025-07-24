#ifndef INPUTTHREAD_H
#define INPUTTHREAD_H

#include <QThread>
#include <QObject>
#include "controller.hpp"
#include <iostream>
#include <QProcess>
#include<QCoreApplication>

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
    void ValidInputDetected(int button_value);

protected:
    void run() override {
        int button_value;
        while (!isInterruptionRequested()) {
           
            button_value = controller->CheckControllerEvent();
            if (button_value != Buttons::INVALID_BUTTON) {
                emit ValidInputDetected(button_value);
            }
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            
        }
        
        std::cout << "finished checking:::::::::::::::\n";
    }
};

#endif // INPUTTHREAD_H