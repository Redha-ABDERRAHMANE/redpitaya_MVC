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
    const int nextPresetIndex;
    const int currentPresetIndex;
    const int source;
    const char* param;
};


typedef std::array<float, 6> preset_array_t ;

class RpSignalGn
{
private:
	
	//waveGnPresets presetFactory;
	RedpitayaCards rp_boards;
    int currentFrequency=5;
    const std::array<TaskConfig, 8> taskConfigs = {{
        {PRIMARY_BOARD,   2, 2, SOURCE_1, "PHAS"},
        {PRIMARY_BOARD,   2, 2, SOURCE_2, "PHAS"},
        {SECONDARY_BOARD, 5, 5, SOURCE_1, "PHAS"},
        {SECONDARY_BOARD, 5, 5, SOURCE_2, "PHAS"},
        {PRIMARY_BOARD,   0, 0, SOURCE_1, "VOLT"},
        {PRIMARY_BOARD,   1, 1, SOURCE_2, "VOLT"},
        {SECONDARY_BOARD, 3, 3, SOURCE_1, "VOLT"},
        {SECONDARY_BOARD, 4, 4, SOURCE_2, "VOLT"}
    }};



public:
	
	RpSignalGn(const char* primaryBoardIP, const char* secondaryBoardIP) :
        rp_boards(primaryBoardIP, secondaryBoardIP, 5),currentFrequency(5){

		std::cout << "in" << std::endl;





	}



    void ramp_up_and_down(const int& card, const int& source, const float& current_value, const float& target_value, const std::string& V_P = "VOLT") {
		float step_size = (target_value - current_value) / STEPS;
		const std::string command = "SOUR" + std::to_string(source) + ":" + V_P + " ";
		float new_value = current_value;

        if (V_P == "PHAS") {
            rp_boards.send_txt(card, command + std::to_string(target_value));
            return; 

        }

        for (uint8_t i = 0; i < STEPS; i++) {
			
            new_value += step_size;
            rp_boards.send_txt(card, command + std::to_string(new_value));
            //std::cout << command + std::to_string(new_value) << std::endl;
	
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << command + std::to_string(target_value) << std::endl;

        rp_boards.send_txt(card, command + std::to_string(target_value));

	}

	void detect_ramp_up_or_down(const int& card, const float& target_value, const float& current_value,const int& source, const std::string& V_P = "VOLT"){
		if (target_value != current_value){
			ramp_up_and_down(card, source, current_value, target_value, V_P);
		}

	}
	//TODO COMPLETE THIS METHOD
    bool apply_preset_values(preset_array_t& nextPreset, const preset_array_t& currentPreset){
		// In the experiment source 1 and 2 of the secondary board are inverted 
		//WARNING : THE SOURCES IN THE PRESET ARRAY ARE INVERTED FOR THE SECONDARY BOARD : nextPreset[3] -> GO TO SOURCE_2 ,nextPreset[4] -> GO TO SOURCE_1

        std::array<std::future<void>, 8> threadArray;
        for (size_t i = 0; i < taskConfigs.size(); ++i) {  //size_t to avoid getting warnings about i < taskConfigs.size() because .size() returns size_t
            const auto& config = taskConfigs[i];
            threadArray[i] = std::async(
                std::launch::async,
                &RpSignalGn::detect_ramp_up_or_down, this,
                config.board,
                nextPreset[config.nextPresetIndex],
                currentPreset[config.currentPresetIndex],
                config.source,
                config.param
                );
        }

		for (std::future<void>& thread : threadArray) {thread.get();}

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return true;
	}


    void  apply_frequency_values(const float& nextFrequency){
        std::cout<<"currentFrequency:"<<currentFrequency<<std::endl;
        std::array<std::future<void>, 4> threadArray={



        std::async(std::launch::async,&RpSignalGn::detect_ramp_up_or_down,this, PRIMARY_BOARD, nextFrequency, currentFrequency, SOURCE_1, "FREQ:FIX:Direct "),
        std::async(std::launch::async,&RpSignalGn::detect_ramp_up_or_down,this, PRIMARY_BOARD, nextFrequency, currentFrequency, SOURCE_2, "FREQ:FIX:Direct "),
        std::async(std::launch::async,&RpSignalGn::detect_ramp_up_or_down,this, SECONDARY_BOARD, nextFrequency, currentFrequency, SOURCE_1, "FREQ:FIX:Direct "),
        std::async(std::launch::async,&RpSignalGn::detect_ramp_up_or_down,this, SECONDARY_BOARD, nextFrequency, currentFrequency, SOURCE_2, "FREQ:FIX:Direct ")


        };

        for (std::future<void>& thread : threadArray) {thread.get();}

        currentFrequency= nextFrequency;


    }
    void  apply_phase_values(const int& card,const int& nextPhase, const int& currentPhase) {
        
   
        std::array<std::future<void>, 2> threadArray = {
        std::async(std::launch::async,&RpSignalGn::detect_ramp_up_or_down,this, card, nextPhase, currentPhase, SOURCE_1, "PHAS"),
        std::async(std::launch::async,&RpSignalGn::detect_ramp_up_or_down,this, card, nextPhase, currentPhase, SOURCE_2, "PHAS"),
        };

        for (std::future<void>& thread : threadArray) { thread.get(); }


    }



		







};
