#pragma once
#include "RpSignalGn.hpp"
#include "waveGnPresets.hpp"
#include "linearStage.hpp"
#include "capacitiveBankManager.hpp"
#include <QObject>
#include <QThread>
#include <QDebug>
#include <commonValues.h>

typedef std::array<float, 6> preset_array_t ;
class MVC_Model: public QObject
{
    Q_OBJECT


private:

    const char* IP_PRIMARY = "169.254.139.169"; // Master board
    const char* IP_SECONDARY = "127.0.0.1";     // Slave board
    RpSignalGn signalGn;
    waveGnPresets presetsGn;
    Controller& controller ;
    LinearStage linearStage;
    CapacitiveBankManager capacitiveBankManager;
    
  

    
    preset_array_t nextPreset;
    preset_array_t currentPreset;
    

public:
    
    MVC_Model(Controller& c) : signalGn(IP_PRIMARY, IP_SECONDARY), presetsGn(), controller(c),linearStage(),capacitiveBankManager(this), nextPreset({}), currentPreset({}) {
        if (!capacitiveBankManager.ConnectToDevice()) {
            std::cout << "Could not connect to Serial Device\n";
        }
        if (!linearStage.ConnectToDevice()) {
            std::cout << "Could not connect to linear stage\n";

        }


    }
    MVC_Model() = default;


    void SetupMVCModel() {
        
        std::cout << "status : " << signalGn.GetConnectionStatus() << std::endl;

        if (signalGn.GetConnectionStatus()!=0) {
            currentPreset = presetsGn.GetCurrentPreset();
            nextPreset = presetsGn.GetCurrentPreset();
            emit RpConnectionSuccess();

        }
        else { emit  RpConnectionFailed(); }

    }


signals:
    void RpConnectionFailed();
    void RpConnectionSuccess();


public slots:



    void GetAndApplyPreset(const int& button_value) {


        presetsGn.SetNextPreset(button_value);

        nextPreset = presetsGn.GetNextPreset();

        currentPreset = presetsGn.GetCurrentPreset();

        for (auto& v : nextPreset)    std::cout << ' ' << v;
        std::cout << "\n";


        for (auto& v : currentPreset) std::cout << ' ' << v;
        std::cout << "\n";

        if (signalGn.ApplyPresetValues(nextPreset, currentPreset)) {

            presetsGn.UpdateCurrentAndPreviousPreset();

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void RpReconnect() {
        std::cout << "entered retry mode" << std::endl;
        if (!signalGn.connect_configure_rpBoards()) {
            emit RpConnectionFailed();
        }
            
    }


    void GetAndApplyPresetMVC(const int& button_next_value,const int& button_current_value=-1){
        //In this function current_preset should not be updated before the call of SignalGn.apply_preset_values()
        //Because we want the amplitude to gradually shift from the last amplitudes used and not the ones given to the function to circumvent coherence problems


        // lastDpadUsed must be coherent to user input to avoid problems

        // Change this by signal emition 
        // Let the MVC_Controller handle the lastDpasUsed update
        controller.set_lastDpadUsed(button_current_value != -1 ? button_current_value : button_next_value);


        presetsGn.SetNextPreset(button_next_value,button_current_value);
        nextPreset = presetsGn.GetNextPreset();
        currentPreset = presetsGn.GetCurrentPreset();

            for (auto& v : nextPreset)    std::cout << ' ' << v;
            std::cout << "\n";


            for (auto& v : currentPreset) std::cout << ' ' << v;
            std::cout << "\n";


        if (signalGn.ApplyPresetValues(nextPreset, currentPreset)) {

            presetsGn.UpdateCurrentAndPreviousPreset();

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }


    
        


    }

    void ApplyFrequencyValue(const int& frequencyValue) {
        signalGn.ApplyFrequencyValues(frequencyValue);
    }

    void ApplyPhaseValue(const int& board,const int& phaseValue) {
        int phaseIndex = board == PRIMARY_BOARD ? PRIMARY_BOARD_COMMON_PHASE_INDEX : SECONDARY_BOARD_COMMON_PHASE_INDEX;
        int currentPhase = presetsGn.GetCurrentPreset()[phaseIndex];
        signalGn.ApplyPhaseValues(board,phaseValue,currentPhase);
        preset_array_t newPreset = presetsGn.GetCurrentPreset();
        newPreset[phaseIndex] = phaseValue;
        presetsGn.SetCurrentPreset(newPreset);

        
    }

    bool LinearStageMoveForward() {
        return linearStage.MoveForward();
    }
    bool LinearStageMoveBackward() {
        return linearStage.MoveBackward();
    }
    bool LinearStageJogForward(){
        return linearStage.JogForward();

    }
    bool LinearStageJogBackward(){
        return linearStage.JogBackward();

    }

    bool LinearStageStopMotion() {
        return linearStage.StopMotion();
    }

    bool LinearStageHome() {
        return linearStage.Home();
    }

    bool CapacitiveBankManagerFrequencyChange(const int frequency) {
        bool result = false;
        QMetaObject::invokeMethod(&capacitiveBankManager, [&]() {
            result = capacitiveBankManager.sendFrequencyChange(frequency);
            }, Qt::BlockingQueuedConnection);

        return result;

    }












};
