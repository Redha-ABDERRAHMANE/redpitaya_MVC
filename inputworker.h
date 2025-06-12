
#ifndef INPUTWORKER_H
#define INPUTWORKER_H

#include <QObject>
#include "controller.hpp"
#include <QThread>
class InputWorker : public QObject {
    Q_OBJECT

private:
    Controller* controller;

    bool run = true;

public:
    InputWorker(Controller* c)
        : controller(c) {}

signals:
    void finished();
    void validInputDetected(int button_value);

public slots:
    void runCheckInput() {
        int button_value ;
        while (run) {
             button_value = controller->CheckControllerEvent();
            if (button_value != -1) {
                emit validInputDetected(button_value);
            }
            QThread::msleep(100); // Delay to avoid CPU overload
        }
        emit finished();
    }

    void stopWorker() {
        run = false;
    }
};

#endif // INPUTWORKER_H
