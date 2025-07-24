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

	SELECT = SDL_GAMEPAD_BUTTON_BACK, START = SDL_GAMEPAD_BUTTON_START,
	INVALID_BUTTON = SDL_GAMEPAD_BUTTON_INVALID


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

class Controller
{

private:
	int gamepadIndex = 0;

	bool SDLInitialized;
	int gamepadsConnected = 0;
	SDL_JoystickID* joystickIDArray;
	SDL_Gamepad* gamepadID;
	SDL_Event event;

	int lastDpadUsed = Buttons::INVALID_BUTTON;

	int CheckValidControllerButtonAndCoherence(const int& button_value) {
		std::cout << "button used" << button_value << std::endl;
		if (WithInInterval(Buttons::BUMPER_LEFT, button_value, Buttons::BUMPER_RIGHT)) {
			std::cout << "HAT PRESSED" << std::endl;
			lastDpadUsed = Buttons::INVALID_BUTTON;
			return button_value;
		}
		if (WithInInterval(Buttons::HAT_UP, button_value, Buttons::HAT_RIGHT)) {
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

public:
	Controller() {


		SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
		
		SDLInitialized = SDL_Init(SDL_INIT_GAMEPAD);
		joystickIDArray = SDL_GetGamepads(&gamepadsConnected);
		if (gamepadsConnected != 0) {
			gamepadID = SDL_OpenGamepad(joystickIDArray[gamepadIndex]);


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



	const int CheckControllerEvent() {
		if ((SDL_WaitEvent(&event) && event.type == SDL_EVENT_GAMEPAD_REMOVED)) {
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
		return  (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) ? CheckValidControllerButtonAndCoherence(static_cast<int>(event.gbutton.button)) : Buttons::INVALID_BUTTON;



	}
	void set_lastDpadUsed(const int& button_value) {
		lastDpadUsed = button_value;
	}
	static bool isButton(const int& button_value) {
		return WithInInterval(Buttons::A, button_value, Buttons::Y);

	}

	static bool isBumper(const int& button_value) {
		return WithInInterval(Buttons::BUMPER_RIGHT, button_value, Buttons::BUMPER_LEFT);

	}
	static bool isHat(const int& button_value) {
		return WithInInterval(Buttons::HAT_UP, button_value, Buttons::HAT_RIGHT);

	}

	int get_lastDpadUsed() const {
		return lastDpadUsed;
	}


};
#endif