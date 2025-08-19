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
    void ApplyLinearStageMoveForward(const LinearStageAxis axis) {
        model->LinearStageMoveForward(axis);
    }
    void ApplyLinearStageMoveBackward(const LinearStageAxis axis) {
        model->LinearStageMoveBackward(axis);
    }
    void ApplyLinearStageJogForward(const LinearStageAxis axis) {
        model->LinearStageJogForward(axis);
    }
    void ApplyLinearStageJogBackward(const LinearStageAxis axis) {
        model->LinearStageJogBackward(axis);
    }
    void ApplyLinearStageStopMotion(const LinearStageAxis axis) {
        model->LinearStageStopMotion(axis);
    }
    bool ApplyLinearStageHome(const LinearStageAxis axis) {
        return model->LinearStageHome(axis);
    }

public:
    ApplyInputWorker(MVC_Model* m)
        : model(m) {}
    ~ApplyInputWorker() {
        model = nullptr;
    }

signals:
    void finished();
    void ValidInputDetected(int button_value);
    void HomingComplete();

public slots:
    void apply_GUIInput(ButtonCombination button_combination) {
        std::cout << "GUI called to apply new preset\n";
        model->GetAndApplyPresetMVC( button_combination.nextButton, button_combination.currentHat);
    }
    void apply_ControllerInput(const int& button_value,const bool is_trigger){
        if (is_trigger) {
            model->ChangeDirectionDimension(button_value);
        }
        else {
            model->GetAndApplyPreset(button_value);
        }
    }

    void apply_FrequencyShift(const int& frequencyValue) {
        model->ApplyFrequencyValue(frequencyValue);
        model->CapacitiveBankManagerFrequencyChange(frequencyValue);
    }

    void apply_PhaseShift(const int& card,const int& phaseValue) {
        model->ApplyPhaseValue( card, phaseValue);
    }

    void ApplyDimensionChange(const int dimensionIndex, const bool GUI_button = false) {

        model->ChangeDirectionDimension(dimensionIndex, GUI_button);

    }

    void ApplyLinearStageMotion(const LinearStageAxis axis,const LinearStageMotion motionState ) {
        std::cout << "linear stage signal called with axis"<<(int) axis<<"\n";
        switch (motionState) {
        case LinearStageMotion::MOVEFORWARD:    ApplyLinearStageMoveForward(axis);  break;
        case LinearStageMotion::MOVEBACKWARD:   ApplyLinearStageMoveBackward(axis); break;
        case LinearStageMotion::JOGFORWARD:     ApplyLinearStageJogForward(axis);   break;
        case LinearStageMotion::JOGBACKWARD:    ApplyLinearStageJogBackward(axis);  break;
        case LinearStageMotion::STOPMOTION:     ApplyLinearStageStopMotion(axis);   break;
        case LinearStageMotion::HOME:           if (ApplyLinearStageHome(axis)) { emit HomingComplete(); };         break;
        }

    }




};

#endif // GUIINPUTWORKER_H
