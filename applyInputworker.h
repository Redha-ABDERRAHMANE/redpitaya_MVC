#ifndef APPLYINPUTWORKER_H
#define APPLYINPUTWORKER_H
#pragma once

#include <QObject>

#include "MVC_Model.hpp"
#include <QThread>
class ApplyInputWorker : public QObject {
    Q_OBJECT

private:
    MVC_Model* model;


private:
    void ApplyLinearStageMoveForward() {
        model->LinearStageMoveForward();
    }
    void ApplyLinearStageMoveBackward() {
        model->LinearStageMoveBackward();
    }
    void ApplyLinearStageJogForward() {
        model->LinearStageJogForward();
    }
    void ApplyLinearStageJogBackward() {
        model->LinearStageJogBackward();
    }
    void ApplyLinearStageStopMotion() {
        model->LinearStageStopMotion();
    }
    void ApplyLinearStageHome() {
        model->LinearStageHome();
    }

public:
    ApplyInputWorker(MVC_Model* m)
        : model(m) {}

signals:
    void finished();
    void ValidInputDetected(int button_value);

public slots:
    void apply_GUIInput(ButtonCombination button_combination) {
        model->GetAndApplyPresetMVC( button_combination.nextButton, button_combination.currentHat);
    }
    void apply_ControllerInput(const int& button_value){
        model->GetAndApplyPreset(button_value);
    }

    void apply_FrequencyShift(const int& frequencyValue) {
        model->ApplyFrequencyValue(frequencyValue);
    }

    void apply_PhaseShift(const int& card,const int& phaseValue) {
        model->ApplyPhaseValue( card, phaseValue);
    }

    void ApplyLinearStageMotion(const LinearStageMotion motionState ) {
        std::cout << "linear stage signal called\n";
        switch (motionState) {
        case LinearStageMotion::MOVEFORWARD:    ApplyLinearStageMoveForward();  break;
        case LinearStageMotion::MOVEBACKWARD:   ApplyLinearStageMoveBackward(); break;
        case LinearStageMotion::JOGFORWARD:     ApplyLinearStageJogForward();   break;
        case LinearStageMotion::JOGBACKWARD:    ApplyLinearStageJogBackward();  break;
        case LinearStageMotion::STOPMOTION:     ApplyLinearStageStopMotion();   break;
        case LinearStageMotion::HOME:           ApplyLinearStageHome();         break;
        }

    }




};

#endif // GUIINPUTWORKER_H
