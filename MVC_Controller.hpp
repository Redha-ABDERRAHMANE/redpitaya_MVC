#pragma once
#include <QDebug>
#include <QObject>
#include <QThread>
#include "MVC_Model.hpp"
#include "inputworker.h"

#include "applyInputworker.h"
#include "view.h"
class MVC_Controller : public QObject {
	Q_OBJECT

private:

    Controller controller;
    std::unique_ptr<MVC_Model> model;
    std::unique_ptr<View> view;
    std::unique_ptr<QThread> thread_controllerInput;
    std::unique_ptr<InputWorker> worker_controllerInput;

    std::unique_ptr<QThread> thread_GUIInput;
    std::unique_ptr<ApplyInputWorker> worker_ApplyInput;

    bool isDefaultDirectionButton(const int& button_value){
        return WithInInterval(Buttons::A,button_value,Buttons::B);
    }

    //Template : {button_value,directionIndexToSend}
    int get_DirectionIndex(const int& button_value){

        int directionIndexToSend;
        int button_valueToSend =Controller::isButton(button_value)?controller.get_lastDpadUsed():button_value;

        directionIndexToSend =dictionary_ButtonDirection.at(button_valueToSend);

        if(Controller::isButton(button_value) && ! isDefaultDirectionButton(button_value)){
            directionIndexToSend += 1;
        }
        return directionIndexToSend;


    }




signals:
    void startCheckInput();

    void controllerInput_Direction(const int& button_value , const int& directionIndex);
    void rpBoards_connectionFailed();

public slots:

    void send_ControllerInput_Direction(const int& button_value){
        //Send signal to GUI to change the visuals then applying the preset

        //Template : {button_value,directionIndexToSend}
        int directionIndexToSend =get_DirectionIndex(button_value);
        qDebug()<<"sending signal";
        emit controllerInput_Direction(button_value,directionIndexToSend);


    }
    void initialize_MVCModel() {
        qDebug() << "here initialize";
        model->setup_MVCModel();
    }




public:

    MVC_Controller( View* v):
        controller(),
        model(std::make_unique<MVC_Model>(controller)),
        view(v),
        thread_controllerInput(std::make_unique<QThread>()),
        thread_GUIInput(std::make_unique<QThread>())
    {

        worker_controllerInput = std::make_unique<InputWorker>(&controller);
        worker_controllerInput->moveToThread(thread_controllerInput.get());
        worker_ApplyInput = std::make_unique<ApplyInputWorker>(model.get());
        worker_ApplyInput->moveToThread(thread_GUIInput.get());

        connect(thread_controllerInput.get(), &QThread::finished, worker_controllerInput.get(), &QObject::deleteLater);
        connect(this, &MVC_Controller::startCheckInput, worker_controllerInput.get(), &InputWorker::runCheckInput);


        connect(this, &MVC_Controller::controllerInput_Direction, view.get(), &View::handleInputReceived);


        connect(worker_controllerInput.get(),&InputWorker::validInputDetected,worker_ApplyInput.get(),&ApplyInputWorker::apply_ControllerInput);
        connect(worker_controllerInput.get(),&InputWorker::validInputDetected,this,&MVC_Controller::send_ControllerInput_Direction);
        connect(view.get(),&View::buttonDirection_pressed,worker_ApplyInput.get(),&ApplyInputWorker::apply_GUIInput);
        connect(view.get(), &View::frequencyChange_pressed, worker_ApplyInput.get(), &ApplyInputWorker::apply_FrequencyShift);
        connect(view.get(), &View::phaseChange_pressed, worker_ApplyInput.get(), &ApplyInputWorker::apply_PhaseShift);
        connect(view.get(), &View::initialize_MVCModel, this, &MVC_Controller::initialize_MVCModel);
        connect(model.get(), & MVC_Model::rpBoards_connectionFailed, view.get(), &View::connectionFailedPopUp);
        connect(model.get(), &MVC_Model::rpBoards_connectionSuccess, view.get(), &View::trigger_initialization);
        model->setup_MVCModel();
        //view->trigger_initialization();
        thread_controllerInput->start();
        thread_GUIInput->start();
        emit startCheckInput();

        //TO DO FIXXXXXXXX POP UP GUI NOT SHOWING UP WHEN RECONNECTING
    }
    ~MVC_Controller(){
    }














};


