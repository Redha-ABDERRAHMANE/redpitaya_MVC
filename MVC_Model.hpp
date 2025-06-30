#pragma once
#include "RpSignalGn.hpp"
#include "waveGnPresets.hpp"
#include <QObject>
#include <QThread>
#include <QDebug>
typedef std::array<float, 6> preset_array_t ;
class MVC_Model: public QObject
{
    Q_OBJECT


private:

    const char* IP_PRIMARY = "127.0.0.1"; // Master board "169.254.112.159"
    const char* IP_SECONDARY = "169.254.9.76";     // Slave board
    RpSignalGn SignalGn;
    waveGnPresets GnPresets;
    Controller& controller ;
  

    
    preset_array_t nextPreset;
    preset_array_t currentPreset;
    

public:
    
    MVC_Model(Controller& c) : SignalGn(IP_PRIMARY, IP_SECONDARY), GnPresets(), controller(c), nextPreset({}), currentPreset({}) {

    }
    virtual ~MVC_Model() = default;


    void setup_MVCModel() {
        
        std::cout << "status : " << SignalGn.get_connectionStatus() << std::endl;

        if (SignalGn.get_connectionStatus()!=0) {
            currentPreset = GnPresets.get_currentPreset();
            nextPreset = GnPresets.get_currentPreset();
            emit rpBoards_connectionSuccess();

        }
        else { emit  rpBoards_connectionFailed(); }

    }


signals:
    void rpBoards_connectionFailed();
    void rpBoards_connectionSuccess();


public slots:



    void get_and_applyPreset(const int& button_value) {


        GnPresets.set_nextPreset(button_value);

        nextPreset = GnPresets.get_nextPreset();

        currentPreset = GnPresets.get_currentPreset();

        for (auto& v : nextPreset)    std::cout << ' ' << v;
        std::cout << "\n";


        for (auto& v : currentPreset) std::cout << ' ' << v;
        std::cout << "\n";

        if (SignalGn.apply_preset_values(nextPreset, currentPreset)) {

            GnPresets.update_currentAndPreviousPreset();

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void retry_connectRpBoards() {
        std::cout << "entered retry mode" << std::endl;
        if (!SignalGn.connect_configure_rpBoards()) {
            emit rpBoards_connectionFailed();
        }
            
    }


    void get_and_applyPreset_MVC(const int& button_next_value,const int& button_current_value=-1){
        //In this function current_preset should not be updated before the call of SignalGn.apply_preset_values()
        //Because we want the amplitude to gradually shift from the last amplitudes used and not the ones given to the function to circumvent coherence problems


        // lastDpadUsed must be coherent to user input to avoid problems

        // Change this by signal emition 
        // Let the MVC_Controller handle the lastDpasUsed update
        controller.set_lastDpadUsed(button_current_value != -1 ? button_current_value : button_next_value);


        GnPresets.set_nextPreset(button_next_value,button_current_value);
        nextPreset = GnPresets.get_nextPreset();
        currentPreset = GnPresets.get_currentPreset();

            for (auto& v : nextPreset)    std::cout << ' ' << v;
            std::cout << "\n";


            for (auto& v : currentPreset) std::cout << ' ' << v;
            std::cout << "\n";


        if (SignalGn.apply_preset_values(nextPreset, currentPreset)) {

            GnPresets.update_currentAndPreviousPreset();

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }


    
        


    }

    void apply_FrequencyValue(const int& frequencyValue) {
        SignalGn.apply_frequency_values(frequencyValue);
    }

    void apply_PhaseValue(const int& card,const int& phaseValue) {
        int phaseIndex = card == 1 ? 2 : 5;
        int currentPhase = GnPresets.get_currentPreset()[phaseIndex];
        SignalGn.apply_phase_values(card,phaseValue,currentPhase);
        preset_array_t newPreset = GnPresets.get_currentPreset();
        newPreset[phaseIndex] = phaseValue;
        GnPresets.set_currentPreset(newPreset);

        
    }










};
