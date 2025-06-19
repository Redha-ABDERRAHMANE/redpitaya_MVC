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

public:
    ApplyInputWorker(MVC_Model* m)
        : model(m) {}

signals:
    void finished();
    void validInputDetected(int button_value);

public slots:
    void apply_GUIInput(ButtonCombination button_combination) {
        model->get_and_applyPreset_MVC( button_combination.nextButton, button_combination.currentHat);
    }
    void apply_ControllerInput(const int& button_value){
        model->get_and_applyPreset(button_value);
    }

    void apply_FrequencyShift(const int& frequencyValue) {
        model->apply_FrequencyValue(frequencyValue);
    }

    void apply_PhaseShift(const int& card,const int& frequencyValue) {
        model->apply_PhaseValue(frequencyValue);
    }



};

#endif // GUIINPUTWORKER_H
