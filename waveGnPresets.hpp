#pragma once 
#include<map>
#include <utility>
#include <array>
#include "controller.hpp"
#include <commonValues.h>


//#define SOURCE_1 1
//#define SOURCE_2 2

//#define DELAY 0.3f
//#define STEPS 20
#define check_Equals(val, x, y) ((val) == (x) || (val) == (y))



class waveGnPresets
{
using preset_array_t = std::array<float, 6>;
    //Template <up or left preset, down or right preset>
using pair_p_array_t = std::pair<preset_array_t, preset_array_t>;

private:

    std::map<int, preset_array_t> dictionary_bumperHatPreset{
		{ Buttons::BUMPER_LEFT,  { AMPLITUDE_0,	AMPLITUDE_0	,   PHASE_0,	AMPLITUDE_0	,   AMPLITUDE_0,   PHASE_0   } },
		{ Buttons::BUMPER_RIGHT, { 0.11f,			0.11f	,	PHASE_0,	0.15f		,         0.15f,          PHASE_0 } },
		{ Buttons::HAT_UP,       { 0.11f,			0.11f	,          85,   0.285f,          AMPLITUDE_0,   PHASE_0} },
		{ Buttons::HAT_DOWN,     { 0.11f,			0.11f	,         85	, AMPLITUDE_0,          0.32f,   PHASE_0 } },
		{ Buttons::HAT_RIGHT,    { 0.22f,		AMPLITUDE_0	,			85,   0.15f,         0.15f,          175 } },
		{ Buttons::HAT_LEFT,     {AMPLITUDE_0,   0.21f,         85,   0.15f,         0.15f,          175 } }
	};
	// Button combinations - each button has 2 presets (up/down or left/right variants)
    std::map<int, pair_p_array_t> dictionary_buttonPreset{
		{ Buttons::X, {
			{ 0.11f, 0.11f, 85,0.285f, AMPLITUDE_0, 175 },  // up‐X
			{ 0.11f, 0.11f, 85,   AMPLITUDE_0, 0.32f, 175 }   // down‐X
		  }
		},
		{ Buttons::B, {
			{ 0.11f,0.11f,  85, 0.285f, AMPLITUDE_0,PHASE_0},  // up‐B
			{ 0.11f,0.11f	, 85, AMPLITUDE_0,0.32f,PHASE_0 }   // down‐B
		  }
		},
		{ Buttons::Y, {
			{ 0.22f, AMPLITUDE_0, 85,0.15f, 0.15f, PHASE_0 },  // left‐Y
			{ AMPLITUDE_0, 0.21f, 85,   0.15f, 0.15f, PHASE_0 }   // right‐Y

		  }
		},
		{ Buttons::A, {
			{0.22f,  AMPLITUDE_0,  85, 0.15f, 0.15f, 175 }, // left-A
			{ AMPLITUDE_0,0.21f	,85, 0.15f,  0.15f, 175 },   // right‐A


		  }
		}
	};
    preset_array_t currentPreset = dictionary_bumperHatPreset.at(Buttons::BUMPER_LEFT);
    preset_array_t previousPresetUsed = dictionary_bumperHatPreset.at(Buttons::BUMPER_LEFT);
    preset_array_t nextPreset = {};


public:
	waveGnPresets(){}

    const preset_array_t& get_currentPreset()const {
		return currentPreset;
	}
    const preset_array_t& get_previousPresetUsed()const {
		return previousPresetUsed;
	}
    const preset_array_t& get_nextPreset()const {
		return nextPreset;
	}
    void set_previousPresetUsed(const preset_array_t preset) {
		previousPresetUsed = preset;
	}

    //  USED BY MVC_Controller  TO SET  GUI USER INPUT INSTRUCTIONS
    void set_currentPreset(const int& button_value) {
        currentPreset =  dictionary_bumperHatPreset.at(button_value);
    }
    ////////////////////////////////////////////////////////
    void set_currentPreset(const preset_array_t preset) {
		currentPreset = preset;
	}

	//Second parameter for GUI use only
	void set_nextPreset(const int& button_value,const int& GUI_hat_button=-1){

        static const preset_array_t& hat_up_preset = dictionary_bumperHatPreset.at(Buttons::HAT_UP);
        static const preset_array_t& hat_left_preset = dictionary_bumperHatPreset.at(Buttons::HAT_LEFT);

		if (Controller::isButton(button_value)) {
            bool isUp_or_left;
			if (GUI_hat_button != -1) {
                preset_array_t& hatPreset = dictionary_bumperHatPreset.at(GUI_hat_button);
				isUp_or_left = (hatPreset == hat_up_preset) || (hatPreset == hat_left_preset);
			}
			else {
				isUp_or_left = (currentPreset == hat_up_preset) || (currentPreset == hat_left_preset);
			}
            const std::pair<preset_array_t, preset_array_t>& preset_pair = dictionary_buttonPreset.at(button_value);
			nextPreset = isUp_or_left ? preset_pair.first : preset_pair.second;
			return;

		}
		nextPreset= dictionary_bumperHatPreset.at(button_value);

	}



	void update_currentAndPreviousPreset() {
		set_previousPresetUsed(currentPreset);
		set_currentPreset(nextPreset);
	}




};

