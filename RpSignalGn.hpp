#pragma once
//#include "waveGnPresets.hpp"
#include <array>
#include <thread>
#include <chrono>
#include <ctime>
#include "RedpitayaCard.hpp"
#include <future>
#include "commonValues.h"

#define DELAY 0.3f
#define STEPS 20
#define lessOrEqualValue(value,x) ((value)<=(x))


#define check_ValueBounds(value,x,y) ((x)<=(value) && (value)<=(y))


//STRUCT TO USE FOR apply_preset_value method
struct TaskConfig {
    const int board;
    const int presetValueIndex;
    const int source;
    const char* param;
};




class RpSignalGn
{
    typedef std::array<float, 9> preset_array_t;
    typedef std::pair<preset_array_t, preset_array_t> pair_p_array_t;
private:
	
	//waveGnPresets presetFactory;

	RedpitayaCards rpBoards;
    int currentFrequency=5;
    std::array<TaskConfig, 10> taskConfigs = {{
        {PRIMARY_BOARD  ,   PRIMARY_BOARD_COMMON_PHASE_INDEX    ,  SOURCE_1, "PHAS"},
        {PRIMARY_BOARD  ,   PRIMARY_BOARD_COMMON_PHASE_INDEX    ,  SOURCE_2, "PHAS"},
        {SECONDARY_BOARD,   SECONDARY_BOARD_COMMON_PHASE_INDEX  ,  SOURCE_1, "PHAS"},
        {SECONDARY_BOARD,   SECONDARY_BOARD_COMMON_PHASE_INDEX  ,  SOURCE_2, "PHAS"},
        {TERTIARY_BOARD,    TERTIARY_BOARD_COMMON_PHASE_INDEX   ,  SOURCE_1, "PHAS"},
        {TERTIARY_BOARD,    TERTIARY_BOARD_COMMON_PHASE_INDEX   ,  SOURCE_2, "PHAS"},
        {PRIMARY_BOARD  ,   PRIMARY_BOARD_FIRST_AMP_INDEX       ,  SOURCE_1, "VOLT"},
        {PRIMARY_BOARD  ,   PRIMARY_BOARD_SECOND_AMP_INDEX      ,  SOURCE_2, "VOLT"},
        {SECONDARY_BOARD,   SECONDARY_BOARD_FIRST_AMP_INDEX     ,  SOURCE_1, "VOLT"},
        {SECONDARY_BOARD,   SECONDARY_BOARD_SECOND_AMP_INDEX    ,  SOURCE_2, "VOLT"},

        }};
        // REMOVED THE LAST TWO TO TEST Z UP AND DOWN WITH TWO BUTTONS WITHOUT THE NEED TO CHANGE THE PRESET
        // IT RUNS INDEPENDENTLY FROM THE XY PRESET FOR NOW
        // 
        //{TERTIARY_BOARD,    TERTIARY_BOARD_FIRST_AMP_INDEX      ,  SOURCE_1, "VOLT"},
        //{TERTIARY_BOARD,    TERTIARY_BOARD_SECOND_AMP_INDEX     ,  SOURCE_2, "VOLT"}
    



public:
	
	RpSignalGn(const char* primaryBoardIP, std::array<const char*,SLAVE_BOARDS> slaveBoardsIP= arraySlaveBoardsIP) :
        rpBoards(primaryBoardIP, slaveBoardsIP, DEFAULT_FREQUENCY),currentFrequency(DEFAULT_FREQUENCY){



        


	}

    bool connect_configure_rpBoards() {
        return rpBoards.ConnectConfigureRpBoards();
    }



    void ramp_up_and_down(const int& card, const int& source, const float& current_value, const float& target_value, const std::string& V_P = "VOLT") {
		float step_size = (target_value - current_value) / STEPS;
		const std::string command = "SOUR" + std::to_string(source) + ":" + V_P + " ";
		float new_value = current_value;

        if (V_P == "PHAS") {
            rpBoards.send_txt(card, command + std::to_string(target_value));
            std::cout << '\n' << command + std::to_string(target_value) << std::endl;
            return; 

        }

        for (uint8_t i = 0; i < STEPS; i++) {
			
            new_value += step_size;
            rpBoards.send_txt(card, command + std::to_string(new_value));
            //std::cout << command + std::to_string(new_value) << std::endl;
	
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout <<'\n' << command + std::to_string(target_value) << std::endl;

        rpBoards.send_txt(card, command + std::to_string(target_value));

	}

	void DetectRampUpOrDown(const int& card, const float& target_value, const float& current_value,const int& source, const std::string& V_P = "VOLT"){
		if (target_value != current_value){
			ramp_up_and_down(card, source, current_value, target_value, V_P);
		}

	}
	//TODO COMPLETE THIS METHOD
    bool ApplyPresetValues(preset_array_t& nextPreset, const preset_array_t& currentPreset){
		

        std::array<std::future<void>, 10> threadArray;
        for (size_t i = 0; i < taskConfigs.size(); ++i) {  
            const auto& config = taskConfigs[i];
            threadArray[i] = std::async(
                std::launch::async,
                &RpSignalGn::DetectRampUpOrDown, this,
                config.board,
                nextPreset[config.presetValueIndex],
                currentPreset[config.presetValueIndex],
                config.source,
                config.param
                );
        }

		for (std::future<void>& thread : threadArray) {thread.get();}

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return true;
	}


    void  ApplyFrequencyValues(const float& nextFrequency){
        std::cout<<"currentFrequency:"<<currentFrequency<<std::endl;
        std::array<std::future<void>, 6> threadArray={



        std::async(std::launch::async,&RpSignalGn::DetectRampUpOrDown,this, PRIMARY_BOARD, nextFrequency, currentFrequency, SOURCE_1, "FREQ:FIX:Direct "),
        std::async(std::launch::async,&RpSignalGn::DetectRampUpOrDown,this, PRIMARY_BOARD, nextFrequency, currentFrequency, SOURCE_2, "FREQ:FIX:Direct "),
        std::async(std::launch::async,&RpSignalGn::DetectRampUpOrDown,this, SECONDARY_BOARD, nextFrequency, currentFrequency, SOURCE_1, "FREQ:FIX:Direct "),
        std::async(std::launch::async,&RpSignalGn::DetectRampUpOrDown,this, SECONDARY_BOARD, nextFrequency, currentFrequency, SOURCE_2, "FREQ:FIX:Direct "),
        std::async(std::launch::async,&RpSignalGn::DetectRampUpOrDown,this, TERTIARY_BOARD, nextFrequency, currentFrequency, SOURCE_1, "FREQ:FIX:Direct "),
        std::async(std::launch::async,&RpSignalGn::DetectRampUpOrDown,this, TERTIARY_BOARD, nextFrequency, currentFrequency, SOURCE_2, "FREQ:FIX:Direct ")


        };

        for (std::future<void>& thread : threadArray) {thread.get();}

        currentFrequency= static_cast<int> (nextFrequency);


    }
    void  ApplyPhaseValues(const int& card,const int& nextPhase, const int& currentPhase) {
        
   
        std::array<std::future<void>, 2> threadArray = {
        std::async(std::launch::async,&RpSignalGn::DetectRampUpOrDown,this, card, nextPhase, currentPhase, SOURCE_1, "PHAS"),
        std::async(std::launch::async,&RpSignalGn::DetectRampUpOrDown,this, card, nextPhase, currentPhase, SOURCE_2, "PHAS"),
        };

        for (std::future<void>& thread : threadArray) { thread.get(); }


    }

    bool GetConnectionStatus()const {
        return rpBoards.GetConnectionStatus();
    }



		







};
