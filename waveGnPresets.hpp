#pragma once 
#include<map>
#include <utility>
#include <array>
#include "controller.hpp"
#include "commonValues.h"
#include "signalPresetValues.hpp"


//#define SOURCE_1 1
//#define SOURCE_2 2

//#define DELAY 0.3f
//#define STEPS 20
#define check_Equals(val, x, y) ((val) == (x) || (val) == (y))



class waveGnPresets
{


private:

	std::map<int, preset_array_t>* dictionary_bumperHatPreset = &dictionary_bumperHatPreset_XY;
	std::map<int, pair_p_array_t>* dictionary_buttonPreset = &dictionary_buttonPreset_XY;
	preset_array_t currentPreset = dictionary_bumperHatPreset_XY.at(Buttons::BUMPER_LEFT);
	preset_array_t previousPresetUsed = dictionary_bumperHatPreset_XY.at(Buttons::BUMPER_LEFT);
	preset_array_t nextPreset = {};

	int dimension = Dimensions::XY;

public:
	waveGnPresets() {}

	const preset_array_t& GetCurrentPreset()const {
		return currentPreset;
	}
	const preset_array_t& GetPreviousPresetUsed()const {
		return previousPresetUsed;
	}
	const preset_array_t& GetNextPreset()const {
		return nextPreset;
	}
	void SetPreviousPresetUsed(const preset_array_t preset) {
		previousPresetUsed = preset;
	}

	void SetDimension(const int button_value) {
		switch (button_value) {
		case Buttons::TRIGGER_LEFT:
			dimension = ((dimension - 1) % Dimensions::DIMENSIONSIZE) < 0 ? Dimensions::DIMENSIONSIZE -1 : (dimension - 1);
			break;
		
		case Buttons::TRIGGER_RIGHT:
			dimension = (dimension + 1) % Dimensions::DIMENSIONSIZE;
			break;
		}
		std::cout << "DIMENSION SET TO : " << dimension << std::endl;
		dictionary_bumperHatPreset = &arrayDimensionDictionnaries.at(dimension).first;
		dictionary_buttonPreset = &arrayDimensionDictionnaries.at(dimension).second ;



	}

	//  USED BY MVC_Controller  TO SET  GUI USER INPUT INSTRUCTIONS
	void SetCurrentPreset(const int& button_value) {
		currentPreset = dictionary_bumperHatPreset->at(button_value);
	}
	////////////////////////////////////////////////////////
	void SetCurrentPreset(const preset_array_t preset) {
		currentPreset = preset;
	}

	//Second parameter for GUI use only
	void SetNextPreset(const int& button_value, const int& GUI_hat_button = -1) {
		static int lastUsedDimension = dimension;
		static preset_array_t hat_up_preset = dictionary_bumperHatPreset->at(Buttons::HAT_UP);
		static preset_array_t hat_left_preset = dictionary_bumperHatPreset->at(Buttons::HAT_LEFT);

		if (lastUsedDimension != dimension) {
			hat_up_preset = dictionary_bumperHatPreset->at(Buttons::HAT_UP);
			hat_left_preset = dictionary_bumperHatPreset->at(Buttons::HAT_LEFT);
			lastUsedDimension = dimension;
			
		}



		if (Controller::IsButton(button_value)) {
			bool isUp_or_left;
			if (GUI_hat_button != -1) {
				preset_array_t& hatPreset = dictionary_bumperHatPreset->at(GUI_hat_button);
				isUp_or_left = (hatPreset == hat_up_preset) || (hatPreset == hat_left_preset);
			}
			else {
				isUp_or_left = (currentPreset == hat_up_preset) || (currentPreset == hat_left_preset);
			}
			const std::pair<preset_array_t, preset_array_t>& preset_pair = dictionary_buttonPreset->at(button_value);
			nextPreset = isUp_or_left ? preset_pair.first : preset_pair.second;
			return;

		}
		nextPreset = dictionary_bumperHatPreset->at(button_value);

	}



	void UpdateCurrentAndPreviousPreset() {
		SetPreviousPresetUsed(currentPreset);
		SetCurrentPreset(nextPreset);
	}




};

