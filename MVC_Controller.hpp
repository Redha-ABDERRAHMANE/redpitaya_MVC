#pragma once
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QApplication>
#include "MVC_Model.hpp"
#include "inputworker.h"

#include "applyInputworker.h"
#include "cameraInputworker.hpp"
#include "view.h"

class MVC_Controller : public QObject {
    Q_OBJECT

private:
    Controller controller;
    MVC_Model model;
    std::unique_ptr<View> view;
    QThread thread_controllerInput;
    InputWorker worker_controllerInput;

    QThread thread_GUIInput;
    ApplyInputWorker worker_ApplyInput;

    QThread thread_cameraInput;
    CameraInputWorker worker_cameraInput;

    bool isDefaultDirectionButton(const int& button_value) {
        return WithInInterval(Buttons::A, button_value, Buttons::B);
    }

    //Template : {button_value,directionIndexToSend}
    int get_DirectionIndex(const int& button_value) {
        int directionIndexToSend;
        int button_valueToSend = Controller::isButton(button_value) ? controller.get_lastDpadUsed() : button_value;

        directionIndexToSend = dictionary_ButtonDirection.at(button_valueToSend);

        if (Controller::isButton(button_value) && !isDefaultDirectionButton(button_value)) {
            directionIndexToSend += 1;
        }
        return directionIndexToSend;
    }

signals:
    void startCheckInput();
    void controllerInput_Direction(const int& button_value, const int& directionIndex);
    void rpBoards_connectionFailed();
    void startCameraInput();

public slots:
    void send_ControllerInput_Direction(const int& button_value) {
        //Send signal to GUI to change the visuals then applying the preset
        //Template : {button_value,directionIndexToSend}
        int directionIndexToSend = get_DirectionIndex(button_value);
        qDebug() << "sending signal";
        emit controllerInput_Direction(button_value, directionIndexToSend);
    }

    void initialize_MVCModel() {
        qDebug() << "here initialize";
        model.setup_MVCModel();
    }

    void shutDownProgram() {
        // Clean shutdown of threads
        thread_controllerInput.requestInterruption();
        thread_GUIInput.requestInterruption();
        thread_cameraInput.requestInterruption();

        if (thread_controllerInput.isRunning()) {
            thread_controllerInput.quit();
            thread_controllerInput.wait();
        }

        if (thread_GUIInput.isRunning()) {
            thread_GUIInput.quit();
            thread_GUIInput.wait();
        }

        if (thread_cameraInput.isRunning()) {
            thread_cameraInput.quit();
            thread_cameraInput.wait();
        }
        QApplication::quit();
    }

public:
    MVC_Controller(View* v) :
        controller(),
        model(controller),  // Pass controller to model constructor
        view(v),
        worker_controllerInput(&controller),
        worker_ApplyInput(&model)
    {
        // Move workers to their respective threads
        worker_controllerInput.moveToThread(&thread_controllerInput);
        worker_ApplyInput.moveToThread(&thread_GUIInput);
        worker_cameraInput.moveToThread(&thread_cameraInput);

        // Connect thread finished signals
        connect(&thread_controllerInput, &QThread::finished, &worker_controllerInput, &QObject::deleteLater);

        // Connect controller signals
        connect(this, &MVC_Controller::startCheckInput, &worker_controllerInput, &InputWorker::runCheckInput);
        connect(this, &MVC_Controller::startCheckInput, &worker_cameraInput, &CameraInputWorker::getDisplayFrame);
        connect(this, &MVC_Controller::startCameraInput, &worker_cameraInput, &CameraInputWorker::startCamera);

        // Connect view signals
        connect(this, &MVC_Controller::controllerInput_Direction, view.get(), &View::handleInputReceived);

        // Connect worker signals
        connect(&worker_controllerInput, &InputWorker::validInputDetected, &worker_ApplyInput, &ApplyInputWorker::apply_ControllerInput);
        connect(&worker_controllerInput, &InputWorker::validInputDetected, this, &MVC_Controller::send_ControllerInput_Direction);

        // Connect view to workers
        connect(view.get(), &View::buttonDirection_pressed, &worker_ApplyInput, &ApplyInputWorker::apply_GUIInput);
        connect(view.get(), &View::frequencyChange_pressed, &worker_ApplyInput, &ApplyInputWorker::apply_FrequencyShift);
        connect(view.get(), &View::phaseChange_pressed, &worker_ApplyInput, &ApplyInputWorker::apply_PhaseShift);
        connect(view.get(), &View::initialize_MVCModel, this, &MVC_Controller::initialize_MVCModel);
        connect(view.get(), &View::retryButton_pressed, &model, &MVC_Model::retry_connectRpBoards);
        connect(view.get(), &View::programShutdown, this, &MVC_Controller::shutDownProgram);


        // Connect model signals
        connect(&model, &MVC_Model::rpBoards_connectionFailed, view.get(), &View::connectionFailedPopUp);
        connect(&model, &MVC_Model::rpBoards_connectionSuccess, view.get(), &View::trigger_initialization);

        // Connect camera worker
        connect(&worker_cameraInput, &CameraInputWorker::ImageReceived, view.get(), &View::get_refresh_imageReceived);

        // Start camera thread first
        thread_cameraInput.start();
        emit startCameraInput();
        //model.setup_MVCModel();


        // Initialize view
        view->trigger_initialization();

        // Start other threads
        thread_controllerInput.start();
        thread_GUIInput.start();

        emit startCheckInput();
    }

    ~MVC_Controller() {

    }
};