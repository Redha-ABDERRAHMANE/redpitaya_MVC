#pragma once
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QApplication>
#include "MVC_Model.hpp"
#include "inputworker.h"

#include "applyInputworker.h"
#include "cameraInputworker.hpp"
#include "videoRecorderWorker.hpp"
#include "view.h"

class MVC_Controller : public QObject {
    Q_OBJECT

private:
    Controller controller;
    MVC_Model model;
    
    InputThread workerThread_controllerInput;

    QThread thread_GUIInput;
    ApplyInputWorker worker_ApplyInput;

   
    CameraInputWorkerThread workerThread_cameraInput;
    QThread  workerThread_videoRecorder;
    VideoRecorderThread worker_videoRecorder;

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
    void StartCamera();
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

        emit StartCamera();
       /* emit startCheckInput();*/
    }


    void setCameraExposureTime(int value) {
        workerThread_cameraInput.SetExposureTimeValue(value);
        

    }

    void setCameraSaturation(int value) {
        workerThread_cameraInput.SetSaturationValue(value);
        

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
        workerThread_videoRecorder.requestInterruption();

        emit workerThreads_shutdown();

        disconnect();

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
        if (workerThread_videoRecorder.isRunning()) {
            workerThread_videoRecorder.quit();
            workerThread_videoRecorder.wait();
        }
           
        std::cout << "yolo 3" << '\n';

        std::cout << "Waiting for camera thread cleanup..." << std::endl;
        if (workerThread_cameraInput.isRunning()) {
            // Give the camera thread time to complete its cleanup
            if (!workerThread_cameraInput.wait(5000)) { // 5 second timeout
                std::cout << "Camera thread cleanup timeout, forcing termination..." << std::endl;
                workerThread_cameraInput.terminate();
                workerThread_cameraInput.wait();
            }
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
    MVC_Controller(View* view) :
        controller(),
        model(controller),  // Pass controller to model constructor
        workerThread_controllerInput(&controller),
        worker_ApplyInput(&model),
        workerThread_videoRecorder()
    {


     
        // Move workers to their respective threads
        worker_ApplyInput.moveToThread(&thread_GUIInput);
        worker_videoRecorder.moveToThread(&workerThread_videoRecorder);
        



        // Connect controller signals
       
        //connect(this, &MVC_Controller::startCheckInput, &workerThread_cameraInput, &CameraInputWorkerThread::getDisplayFrame);

        connect(this, &MVC_Controller::StartCamera, &workerThread_cameraInput, &CameraInputWorkerThread::StartCamera);
        connect(view, &View::CameraRetryButtonPressed, &workerThread_cameraInput, &CameraInputWorkerThread::StartCamera);
        connect(this, &MVC_Controller::workerThreads_shutdown, &workerThread_cameraInput, &CameraInputWorkerThread::CleanUpCameraRessources);

        connect(this, &MVC_Controller::startMainGUI, view, &View::StartGUIComponentsInitialization);

        connect(this, &MVC_Controller::controllerInput_Direction, view, &View::HandleInputReceived);
    
        connect(&workerThread_cameraInput, &CameraInputWorkerThread::CameraNotFound, view, &View::CameraFailedPopUp);
        connect(&workerThread_cameraInput, &CameraInputWorkerThread::ImageReceived, view, &View::SetNewFrameToDisplay);

        connect(&workerThread_cameraInput, &CameraInputWorkerThread::CameraReady, this, &MVC_Controller::SendSignalstartCheckInput);
        


        // Connect view signals
        

        // Connect worker signals
        connect(&workerThread_controllerInput, &InputThread::validInputDetected, &worker_ApplyInput, &ApplyInputWorker::apply_ControllerInput);

        connect(&workerThread_controllerInput, &InputThread::validInputDetected, this, &MVC_Controller::send_ControllerInput_Direction);
        

        // Connect view to workers
        connect(view, &View::PressedButtonDirection, &worker_ApplyInput, &ApplyInputWorker::apply_GUIInput);

        connect(view, &View::PressedFrequencyChange, &worker_ApplyInput, &ApplyInputWorker::apply_FrequencyShift);

        connect(view, &View::PressedExposureTimeChange, this, &MVC_Controller::setCameraExposureTime);

        connect(view, &View::PressedSaturationChange, this, &MVC_Controller::setCameraSaturation);

        connect(view, &View::PressedPhaseChange, &worker_ApplyInput, &ApplyInputWorker::apply_PhaseShift);

        connect(view, &View::InitializeMVCModel, this, &MVC_Controller::initialize_MVCModel);

        connect(view, &View::PressedRetryButton, &model, &MVC_Model::retry_connectRpBoards);

        connect(view, &View::ProgramShutdown, this, &MVC_Controller::shutDownProgram);

        connect(view, &View::GUIReady, this, &MVC_Controller::startWorkerThreads);


        // Connect model signals
        connect(&model, &MVC_Model::rpBoards_connectionFailed, view, &View::ConnectionToBoardsFailedPopUp);

        connect(&model, &MVC_Model::rpBoards_connectionSuccess, view, &View::StartGUIComponentsInitialization);


        

        /////////////////////DEBUG//
        workerThread_videoRecorder.start();


        connect(view, &View::StartCameraRecord, &worker_videoRecorder, &VideoRecorderThread::startRecording, Qt::QueuedConnection);
        connect(view, &View::StopCameraRecord, &worker_videoRecorder, &VideoRecorderThread::stopRecording, Qt::QueuedConnection);

        connect(view, &View::StartCameraRecord, &workerThread_cameraInput, &CameraInputWorkerThread::StartRecording);
        connect(view, &View::StopCameraRecord, &workerThread_cameraInput, &CameraInputWorkerThread::StopRecording);
        connect(&workerThread_cameraInput, &CameraInputWorkerThread::SendImageToCapture, &worker_videoRecorder, &VideoRecorderThread::ReceiveImageToCapture, Qt::QueuedConnection);
        
        /////////////////////

      
        
        //model.setup_MVCModel();


        emit startMainGUI();
        
        
    }

    ~MVC_Controller() {

        
    }
};