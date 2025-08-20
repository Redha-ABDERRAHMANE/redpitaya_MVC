#ifndef CONTROLLER_H
#define CONTROLLER_H
#include<iostream>
#include<SDL3/SDL_hints.h>
#include<SDL3/SDL_events.h>
#include<SDL3/SDL_init.h>
#include<SDL3/SDL_gamepad.h>

#include <unordered_map>




#define WithInInterval(a, b, c) ((a) <= (b) && (b) <= (c))



enum Buttons {
	A = SDL_GAMEPAD_BUTTON_SOUTH, B = SDL_GAMEPAD_BUTTON_EAST, X = SDL_GAMEPAD_BUTTON_WEST, Y = SDL_GAMEPAD_BUTTON_NORTH,

	HAT_UP = SDL_GAMEPAD_BUTTON_DPAD_UP, HAT_DOWN = SDL_GAMEPAD_BUTTON_DPAD_DOWN, HAT_LEFT = SDL_GAMEPAD_BUTTON_DPAD_LEFT, HAT_RIGHT = SDL_GAMEPAD_BUTTON_DPAD_RIGHT,

	BUMPER_RIGHT = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, BUMPER_LEFT = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,

	TRIGGER_RIGHT = 100, TRIGGER_LEFT = 101,

	RIGHT_THUMBSTICK_X, RIGHT_THUMBSTICK_Y, LEFT_THUMBSTICK_X, LEFT_THUMBSTICK_Y,

	SELECT = SDL_GAMEPAD_BUTTON_BACK, START = SDL_GAMEPAD_BUTTON_START,
	INVALID_BUTTON = SDL_GAMEPAD_BUTTON_INVALID


};
enum Triggers {
	AXIS_TRIGGER_RIGHT = SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, AXIS_TRIGGER_LEFT = SDL_GAMEPAD_AXIS_LEFT_TRIGGER
};

enum Thumbsticks {
	LEFT_THUMBSTICK_X_AXIS = SDL_GAMEPAD_AXIS_LEFTX, LEFT_THUMBSTICK_Y_AXIS = SDL_GAMEPAD_AXIS_LEFTY,
	RIGHT_THUMBSTICK_X_AXIS = SDL_GAMEPAD_AXIS_RIGHTX, RIGHT_THUMBSTICK_Y_AXIS = SDL_GAMEPAD_AXIS_RIGHTY
};

enum InputType {
	NOINPUT,
	BUTTONPRESS,
	TRIGGERPRESS,
	THUMBSTICKMOTION
};
inline std::unordered_map<int, int> dictionary_ButtonDirection{
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

struct PressedButton {
	int button = Buttons::INVALID_BUTTON;
	int inputType = InputType::NOINPUT;
	int triggerForce = -1;

};

static constexpr int AXISMAXVALUE = SDL_JOYSTICK_AXIS_MAX;
static constexpr int AXISMINVALUE = SDL_JOYSTICK_AXIS_MIN;

class Controller
{

private:
	static constexpr int INVALID_VALUE = -1;
	static constexpr int SDL_WAITPOLLTIMEOUT = 100;
	const int DEADZONE = 10000;
	const 
	int gamepadIndex = 0;

	bool SDLInitialized;
	bool blockTrigger = false;
	int gamepadsConnected = 0;
	SDL_JoystickID* joystickIDArray;
	SDL_Gamepad* gamepadID;
	SDL_Event event;

	int lastDpadUsed = Buttons::INVALID_BUTTON;
	bool thumbstickReachedMax = false;
	PressedButton lastUsedThumbstick = { Buttons::INVALID_BUTTON, InputType::THUMBSTICKMOTION,INVALID_VALUE };

	int CheckValidControllerButtonAndCoherence(const int& button_value) {
		std::cout << "button used" << button_value << std::endl;
		if (IsBumper(button_value)) {
			std::cout << "BUMPER PRESSED" << std::endl;
			lastDpadUsed = Buttons::INVALID_BUTTON;
			return button_value;
		}
		if (button_value == Buttons::SELECT) {
			return Buttons::INVALID_BUTTON;
		}
		if (isHat(button_value)) {
			if (button_value != lastDpadUsed) {
				lastDpadUsed = button_value;
				return button_value;
			}
			return Buttons::INVALID_BUTTON;
		}
		switch (button_value) {

		case  Buttons::A:
		case  Buttons::Y:  return (lastDpadUsed == Buttons::HAT_LEFT || lastDpadUsed == Buttons::HAT_RIGHT) ? button_value : Buttons::INVALID_BUTTON; break;

		case  Buttons::B:
		case  Buttons::X:  return (lastDpadUsed == Buttons::HAT_UP || lastDpadUsed == Buttons::HAT_DOWN) ? button_value : Buttons::INVALID_BUTTON; break;
		default: return Buttons::INVALID_BUTTON; std::cout << "false"; break;



		}



	}
	int CheckValidControllerTriggerAndCoherence(const int& button_value) {
		
		std::cout << "trigger pressed \n";
		Buttons realButtonValue = button_value == Triggers::AXIS_TRIGGER_LEFT ? Buttons::TRIGGER_LEFT : Buttons::TRIGGER_RIGHT;
		if (blockTrigger) { return Buttons::INVALID_BUTTON; }
		lastDpadUsed = Buttons::INVALID_BUTTON;
		return realButtonValue;
		


	}
	int CheckValidControllerThumbstickAndCoherence(const int& button_value) {
		
		std::cout << "thumbstick pressed \n";

		switch (button_value) {
		case Thumbsticks::LEFT_THUMBSTICK_X_AXIS: return Buttons::LEFT_THUMBSTICK_X;  break;
		case Thumbsticks::LEFT_THUMBSTICK_Y_AXIS: return Buttons::LEFT_THUMBSTICK_Y;  break;
		case Thumbsticks::RIGHT_THUMBSTICK_X_AXIS:return Buttons::RIGHT_THUMBSTICK_X;  break;
		case Thumbsticks::RIGHT_THUMBSTICK_Y_AXIS:return Buttons::RIGHT_THUMBSTICK_Y;  break;

		}
		
	}
	bool IsTrigger(const int& button_value) {
		return WithInInterval(Triggers::AXIS_TRIGGER_LEFT, button_value, Triggers::AXIS_TRIGGER_RIGHT);
	}
	bool IsThumbstick(const int& button_value) {
		return WithInInterval(Thumbsticks::LEFT_THUMBSTICK_X_AXIS, button_value, Thumbsticks::RIGHT_THUMBSTICK_Y_AXIS);
	}

public:
	Controller() {


		SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
		
		SDLInitialized = SDL_Init(SDL_INIT_GAMEPAD);
		joystickIDArray = SDL_GetGamepads(&gamepadsConnected);
		if (gamepadsConnected != 0) {
			gamepadID = SDL_OpenGamepad(joystickIDArray[gamepadIndex]);


		}
		else {
			if (gamepadID) {
				SDL_CloseGamepad(gamepadID);
			}
			if (joystickIDArray) {
				SDL_free(joystickIDArray);
			}
			gamepadID = nullptr;
			joystickIDArray = nullptr;

		}




	}

	~Controller() {

		if (gamepadID) {
			SDL_CloseGamepad(gamepadID);
		}
		if (joystickIDArray) {
			SDL_free(joystickIDArray);
		}
		gamepadID = nullptr;
		joystickIDArray = nullptr;
		SDL_Quit();
	}


	Controller(const Controller&) = delete;
	Controller& operator=(const Controller&) = delete;



	PressedButton CheckControllerEvent() {
		if ((SDL_WaitEventTimeout(&event, SDL_WAITPOLLTIMEOUT) && event.type == SDL_EVENT_GAMEPAD_REMOVED)) {
			if (gamepadID) {
				SDL_CloseGamepad(gamepadID);
			}
			if (joystickIDArray) {
				SDL_free(joystickIDArray);
			}
			gamepadID = nullptr;
			joystickIDArray = nullptr;
		}
		if (event.type== SDL_EVENT_GAMEPAD_ADDED && !gamepadID) {

			joystickIDArray = SDL_GetGamepads(&gamepadsConnected);

			if (gamepadsConnected != 0) {

				gamepadID = SDL_OpenGamepad(joystickIDArray[gamepadIndex]);
				std::cout << "Gamepad connected\n";

			}
			else { std::cout << "Gamepad not connected\n"; }
			
		}
		
		if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
			return { CheckValidControllerButtonAndCoherence(static_cast<int>(event.gbutton.button)),InputType::BUTTONPRESS, INVALID_VALUE };
		}
		//ADD HISTORY CHECKER FOR THUMBSTICK ITS NOT BEEN DONE YET
		if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
			if ((event.gaxis.value == SDL_JOYSTICK_AXIS_MAX || event.gaxis.value == SDL_JOYSTICK_AXIS_MIN)) {

				if (IsTrigger(static_cast<int>(event.gaxis.axis))) {
					std::cout << "Trigger pressed : " << event.gaxis.axis << " with value : " << event.gaxis.value << "\n";
					return { CheckValidControllerTriggerAndCoherence(static_cast<int>(event.gaxis.axis)),InputType::TRIGGERPRESS,event.gaxis.value };
				}
				if (IsThumbstick(static_cast<int>(event.gaxis.axis))) {
					std::cout << "Thumbstick moved : " << event.gaxis.axis << " with value : " << event.gaxis.value << "\n";
					
					lastUsedThumbstick.button = event.gaxis.axis;
					thumbstickReachedMax = true;

					return { CheckValidControllerThumbstickAndCoherence(static_cast<int>(event.gaxis.axis)),InputType::THUMBSTICKMOTION,event.gaxis.value };
				}
			}
			else if (IsThumbstick(event.gaxis.axis)&&(abs(event.gaxis.value) < SDL_JOYSTICK_AXIS_MAX) && thumbstickReachedMax && lastUsedThumbstick.button== event.gaxis.axis) {
				std::cout << "Thumbstick moved : " << event.gaxis.axis << " with value : " << event.gaxis.value << "\n";
				thumbstickReachedMax = false;
				return { CheckValidControllerThumbstickAndCoherence(static_cast<int>(event.gaxis.axis)),InputType::THUMBSTICKMOTION,0 };

			}

		}



		return { Buttons::INVALID_BUTTON,InputType::NOINPUT,INVALID_VALUE };



	}
	void SetLastDpadUsed(const int& button_value) {
		lastDpadUsed = button_value;
	}
	static bool IsButton(const int& button_value) {
		return WithInInterval(Buttons::A, button_value, Buttons::Y);

	}

	static bool IsBumper(const int& button_value) {
		return WithInInterval(Buttons::BUMPER_LEFT, button_value, Buttons::BUMPER_RIGHT);

	}
	static bool isHat(const int& button_value) {
		return WithInInterval(Buttons::HAT_UP, button_value, Buttons::HAT_RIGHT);

	}


	static bool IsLeftThumbstick(const int& button_value) {
		return WithInInterval(Buttons::LEFT_THUMBSTICK_X, button_value, Buttons::LEFT_THUMBSTICK_Y);
	}
	static bool IsRightThumbstick(const int& button_value) {
		return WithInInterval(Buttons::RIGHT_THUMBSTICK_X, button_value, Buttons::RIGHT_THUMBSTICK_Y);
	}
	int get_lastDpadUsed() const {
		return lastDpadUsed;
	}


};
#endif