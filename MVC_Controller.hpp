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
    InputThread workerThread_controllerInput;

    QThread thread_GUIInput;
    ApplyInputWorker worker_ApplyInput;

   
    CameraInputWorkerThread workerThread_cameraInput;

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
    void startMainGUI();
    void controllerInput_Direction(const int& button_value, const int& directionIndex);
    void rpBoards_connectionFailed();
    void startCameraInput();
    void change_exposureTimeValue(int value);
    void workerThreads_shutdown();

public slots:

    void SendSignalstartCheckInput() {
        std::cout << "received signal\n";
        emit startCheckInput();
    }

    void startWorkerThreads() {
        workerThread_cameraInput.start();
        workerThread_controllerInput.start();
        thread_GUIInput.start();
       /* emit startCheckInput();*/
    }


    void setCameraExposureTime(int value) {
        workerThread_cameraInput.setExposureTimeValue(value);
        

    }

    void setCameraSaturation(int value) {
        workerThread_cameraInput.setSaturationValue(value);
        

    }
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

        std::cout << "=== Starting shutdown ===" << std::endl;

        // Stop workers
        
        

        workerThread_controllerInput.requestInterruption();
        thread_GUIInput.requestInterruption();
        workerThread_cameraInput.requestInterruption();

        emit workerThreads_shutdown();


        std::cout << "yolo 1" << '\n';

        if (workerThread_controllerInput.isRunning()) {
            workerThread_controllerInput.quit();
            workerThread_controllerInput.wait();
        }
        std::cout << "yolo 2" << '\n';

        if (thread_GUIInput.isRunning()) {
            thread_GUIInput.quit();
            thread_GUIInput.wait();
        }
        std::cout << "yolo 3" << '\n';

        if (workerThread_cameraInput.isRunning()) {
            workerThread_cameraInput.quit();
            workerThread_cameraInput.wait();
        }
        std::cout << "yolo 4" << '\n';

        if ((workerThread_cameraInput.isRunning() || thread_GUIInput.isRunning() || workerThread_controllerInput.isRunning())) {
            if (workerThread_cameraInput.isRunning()) { std::cout << " camera still running \n"; }
            if (thread_GUIInput.isRunning()) { std::cout << "GUI Inputstill running \n"; }
            if (workerThread_controllerInput.isRunning()) { std::cout << " controller input still running \n"; }
        }
        std::cout << "done" << std::endl;
    }

public:
    MVC_Controller(View* v) :
        controller(),
        model(controller),  // Pass controller to model constructor
        view(v),
        workerThread_controllerInput(&controller),
        worker_ApplyInput(&model)
    {


     
        // Move workers to their respective threads
        worker_ApplyInput.moveToThread(&thread_GUIInput);
        



        // Connect controller signals
       
        //connect(this, &MVC_Controller::startCheckInput, &workerThread_cameraInput, &CameraInputWorkerThread::getDisplayFrame);

        connect(this, &MVC_Controller::workerThreads_shutdown, &workerThread_cameraInput, &CameraInputWorkerThread::endOfWork);

        connect(this, &MVC_Controller::startMainGUI, view.get(), &View::trigger_initialization);

        connect(this, &MVC_Controller::controllerInput_Direction, view.get(), &View::handleInputReceived);
    
        connect(&workerThread_cameraInput, &CameraInputWorkerThread::ImageReceived, view.get(), &View::get_refresh_imageReceived);

        connect(&workerThread_cameraInput, &CameraInputWorkerThread::cameraReady, this, &MVC_Controller::SendSignalstartCheckInput);
        


        // Connect view signals
        

        // Connect worker signals
        connect(&workerThread_controllerInput, &InputThread::validInputDetected, &worker_ApplyInput, &ApplyInputWorker::apply_ControllerInput);

        connect(&workerThread_controllerInput, &InputThread::validInputDetected, this, &MVC_Controller::send_ControllerInput_Direction);
        

        // Connect view to workers
        connect(view.get(), &View::buttonDirection_pressed, &worker_ApplyInput, &ApplyInputWorker::apply_GUIInput);

        connect(view.get(), &View::frequencyChange_pressed, &worker_ApplyInput, &ApplyInputWorker::apply_FrequencyShift);

        connect(view.get(), &View::exposureTimeChange_pressed, this, &MVC_Controller::setCameraExposureTime);

        connect(view.get(), &View::saturationChange_pressed, this, &MVC_Controller::setCameraSaturation);

        connect(view.get(), &View::phaseChange_pressed, &worker_ApplyInput, &ApplyInputWorker::apply_PhaseShift);

        connect(view.get(), &View::initialize_MVCModel, this, &MVC_Controller::initialize_MVCModel);

        connect(view.get(), &View::retryButton_pressed, &model, &MVC_Model::retry_connectRpBoards);

        connect(view.get(), &View::programShutdown, this, &MVC_Controller::shutDownProgram);

        connect(view.get(), &View::GUIReady, this, &MVC_Controller::startWorkerThreads);


        // Connect model signals
        connect(&model, &MVC_Model::rpBoards_connectionFailed, view.get(), &View::connectionFailedPopUp);

        connect(&model, &MVC_Model::rpBoards_connectionSuccess, view.get(), &View::trigger_initialization);
      
        
        //model.setup_MVCModel();


        emit startMainGUI();
        
        
    }

    ~MVC_Controller() {

        
    }
};