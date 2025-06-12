
#pragma once

// Ensure consistent Windows header inclusion
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX        // Prevent min/max conflicts
#define NOGDI
#define NOUSER
#define NOSERVICE
#include <Windows.h>
#include <mmsystem.h>
#include <Xinput.h>
#include <iostream>
#include <unordered_map>
#pragma comment(lib, "xinput.lib") // or "xinput.lib"
// Add linker XInput.lib

#define WithInInterval(a, b, c) ((a) <= (b) && (b) <= (c))



enum Buttons {
	A = VK_PAD_A, B = VK_PAD_B, X = VK_PAD_X, Y = VK_PAD_Y,

	HAT_UP = VK_PAD_DPAD_UP, HAT_DOWN = VK_PAD_DPAD_DOWN, HAT_LEFT = VK_PAD_DPAD_LEFT, HAT_RIGHT = VK_PAD_DPAD_RIGHT,

	BUMPER_RIGHT = VK_PAD_RSHOULDER, BUMPER_LEFT = VK_PAD_LSHOULDER,

	SELECT = VK_PAD_START, START = VK_PAD_BACK

};
inline std::unordered_map<int, int> dictionary_ButtonDirection {
    {HAT_UP, 0},
    {HAT_DOWN, 2},
    {HAT_RIGHT, 4},
    {HAT_LEFT, 6},
    {BUMPER_RIGHT, 8},
    {BUMPER_LEFT, 9}
};

struct ButtonCombination {
	const int nextButton;
	const int currentHat;
};

class Controller
{

private:
	DWORD dwControllerIndex = 0;

	_XINPUT_KEYSTROKE keyStroke; // Structure to hold key stroke information

	XINPUT_STATE state; // Structure to hold the state of the controller

	int lastDpadUsed = -1;

	bool check_validControllerButtonAndCoherence(const int& button_value) {
        if (WithInInterval(Buttons::BUMPER_RIGHT, button_value, Buttons::BUMPER_LEFT)) {
            lastDpadUsed=-1;
            return true;
        }
		if (WithInInterval(Buttons::HAT_UP, button_value, Buttons::HAT_RIGHT)) {
			if (button_value != lastDpadUsed) {
				lastDpadUsed = button_value;
				return true;
			}
			return false;
		}
		switch (button_value) {

		case  Buttons::A:
		case  Buttons::Y:  return lastDpadUsed == Buttons::HAT_LEFT || lastDpadUsed == Buttons::HAT_RIGHT; break;

		case  Buttons::B:
		case  Buttons::X:  return lastDpadUsed == Buttons::HAT_UP || lastDpadUsed == Buttons::HAT_DOWN; break;
		default: return false;std::cout << "false"; break;



		}



	}

public:
	Controller() :keyStroke{}, state() {

		DWORD dwResult = XInputGetState(dwControllerIndex, &state);// Variable to hold the result of XInput functions

		if (dwResult == ERROR_SUCCESS) {
			std::cout << "Controller is connected." << std::endl;
		}
		else {
			// Controller is not connected
			std::cout << "Controller  is not connected." << std::endl;
		}
	}
	const int CheckControllerEvent() {
		DWORD dwReserved = 0;
		DWORD KeyStroke_result = XInputGetKeystroke(dwControllerIndex, dwReserved, (PXINPUT_KEYSTROKE)&keyStroke);
        (void)KeyStroke_result;
		return  (keyStroke.Flags == XINPUT_KEYSTROKE_KEYDOWN && check_validControllerButtonAndCoherence(keyStroke.VirtualKey)) ? keyStroke.VirtualKey : -1;



	}
	void set_lastDpadUsed(const int& button_value) {
		lastDpadUsed = button_value;
	}
	static bool isButton(const int& button_value) {
		return WithInInterval(Buttons::A, button_value, Buttons::Y);

	}

    static bool isBumper(const int& button_value) {
        return WithInInterval(Buttons::BUMPER_LEFT, button_value, Buttons::BUMPER_RIGHT);

    }
    static bool isHat(const int& button_value) {
        return WithInInterval(Buttons::HAT_UP, button_value, Buttons::HAT_RIGHT);

    }

    int get_lastDpadUsed() const {
        return lastDpadUsed;
    }



	~Controller() {
		std::cout << "end" << std::endl;
	}

};


