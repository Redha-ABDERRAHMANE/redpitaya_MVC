#ifndef   SIGNALPRESETVALUES_H
#define  SIGNALPRESETVALUES_H
#include <map>
#include <array>
#include <controller.hpp>
#include "commonValues.h"

typedef std::array<float, 9> preset_array_t;
typedef std::pair<preset_array_t, preset_array_t> pair_p_array_t;

inline std::map<int, preset_array_t> dictionary_bumperHatPreset_XY{
	{ Buttons::BUMPER_LEFT,  preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
	{ Buttons::BUMPER_RIGHT, preset_array_t{ 0.11f, 0.11f, 85.0f, 0.15f, 0.15f, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
	{ Buttons::HAT_UP,       preset_array_t{ 0.11f, 0.11f, 85.0f, 0.285f, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
	{ Buttons::HAT_DOWN,     preset_array_t{ 0.11f, 0.11f, 85.0f, AMPLITUDE_0, 0.32f, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
	{ Buttons::HAT_RIGHT,    preset_array_t{ 0.22f, AMPLITUDE_0, 85.0f, 0.15f, 0.15f, 175.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
	{ Buttons::HAT_LEFT,     preset_array_t{ AMPLITUDE_0, 0.21f, 85.0f, 0.15f, 0.15f, 175.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } }
};
// Button combinations - each button has 2 presets (up/down or left/right variants)
inline std::map<int, pair_p_array_t> dictionary_buttonPreset_XY{
	{ Buttons::X, {
		{ 0.11f, 0.11f, 85.0f,   AMPLITUDE_0, 0.32f, 175.0f,  AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 } ,  // up‐X{ 0.11f, 0.11f, 85.0f,   AMPLITUDE_0, 0.32f, 175.0f } 
		{ 0.11f, 0.11f, 85.0f,0.285f, AMPLITUDE_0, 175.0f , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 }   // down‐X
	  }
	},
	{ Buttons::B, {
		{ 0.11f,0.11f,  85.0f, 0.285f, AMPLITUDE_0,PHASE_0 , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0},  // up‐B
		{ 0.11f,0.11f	, 85.0f, AMPLITUDE_0,0.32f,PHASE_0 , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 }   // down‐B
	  }
	},
	{ Buttons::Y, {
		{ 0.22f, AMPLITUDE_0, 85.0f,0.15f, 0.15f, PHASE_0  , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0},  // left‐Y { AMPLITUDE_0, 0.21f, 85.0f,   0.15f, 0.15f, PHASE_0 }
		{ AMPLITUDE_0, 0.21f, 85.0f,   0.15f, 0.15f, PHASE_0 , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0}  // right‐Y

	  }
	},
	{ Buttons::A, {
		{ AMPLITUDE_0,0.21f	,85.0f, 0.15f,  0.15f, 175.0f  , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0}, // left-A
		{0.22f,  AMPLITUDE_0,  85.0f, 0.15f, 0.15f, 175.0f , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0},   // right‐A


	  }
	}
};


inline std::map<int, preset_array_t> dictionary_bumperHatPreset_XZ{
{ Buttons::BUMPER_LEFT,  preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::BUMPER_RIGHT, preset_array_t{ 0.11f, 0.11f, 85.0f, 0.15f, 0.15f, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::HAT_UP,       preset_array_t{ 0.11f, 0.11f, 85.0f, 0.285f, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0} },
{ Buttons::HAT_DOWN,     preset_array_t{ 0.11f, 0.11f, 85.0f, AMPLITUDE_0, 0.32f, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::HAT_RIGHT,    preset_array_t{ 0.22f, AMPLITUDE_0, 85.0f, 0.15f, 0.15f, 175.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::HAT_LEFT,     preset_array_t{ AMPLITUDE_0, 0.21f, 85.0f, 0.15f, 0.15f, 175.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } }
};
// Button combinations - each button has 2 presets (up/down or left/right variants)
inline std::map<int, pair_p_array_t> dictionary_buttonPreset_XZ{
	{ Buttons::X, {
		{ 0.11f, 0.11f, 85.0f,   AMPLITUDE_0, 0.32f, 175.0f,  AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 } ,  // up‐X{ 0.11f, 0.11f, 85.0f,   AMPLITUDE_0, 0.32f, 175.0f } 
		{ 0.11f, 0.11f, 85.0f,0.285f, AMPLITUDE_0, 175.0f , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 }   // down‐X
	  }
	},
	{ Buttons::B, {
		{ 0.11f,0.11f,  85.0f, 0.285f, AMPLITUDE_0,PHASE_0 , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0},  // up‐B
		{ 0.11f,0.11f	, 85.0f, AMPLITUDE_0,0.32f,PHASE_0 , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 }   // down‐B
	  }
	},
	{ Buttons::Y, {
		{ 0.22f, AMPLITUDE_0, 85.0f,0.15f, 0.15f, PHASE_0  , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0},  // left‐Y { AMPLITUDE_0, 0.21f, 85.0f,   0.15f, 0.15f, PHASE_0 }
		{ AMPLITUDE_0, 0.21f, 85.0f,   0.15f, 0.15f, PHASE_0 , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0}  // right‐Y

	  }
	},
	{ Buttons::A, {
		{ AMPLITUDE_0,0.21f	,85.0f, 0.15f,  0.15f, 175.0f  , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0}, // left-A
		{0.22f,  AMPLITUDE_0,  85.0f, 0.15f, 0.15f, 175.0f , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0},   // right‐A


	  }
	}
};


inline std::map<int, preset_array_t> dictionary_bumperHatPreset_YZ{
{ Buttons::BUMPER_LEFT,  preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::BUMPER_RIGHT, preset_array_t{ 0.11f, 0.11f, 85.0f, 0.15f, 0.15f, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::HAT_UP,       preset_array_t{ 0.11f, 0.11f, 85.0f, 0.285f, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::HAT_DOWN,     preset_array_t{ 0.11f, 0.11f, 85.0f, AMPLITUDE_0, 0.32f, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::HAT_RIGHT,    preset_array_t{ 0.22f, AMPLITUDE_0, 85.0f, 0.15f, 0.15f, 175.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::HAT_LEFT,     preset_array_t{ AMPLITUDE_0, 0.21f, 85.0f, 0.15f, 0.15f, 175.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } }
};
// Button combinations - each button has 2 presets (up/down or left/right variants)
inline std::map<int, pair_p_array_t> dictionary_buttonPreset_YZ{
	{ Buttons::X, {
		{ 0.11f, 0.11f, 85.0f,   AMPLITUDE_0, 0.32f, 175.0f,  AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 } ,  // up‐X{ 0.11f, 0.11f, 85.0f,   AMPLITUDE_0, 0.32f, 175.0f } 
		{ 0.11f, 0.11f, 85.0f,0.285f, AMPLITUDE_0, 175.0f , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 }   // down‐X
	  }
	},
	{ Buttons::B, {
		{ 0.11f,0.11f,  85.0f, 0.285f, AMPLITUDE_0,PHASE_0 , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0},  // up‐B
		{ 0.11f,0.11f	, 85.0f, AMPLITUDE_0,0.32f,PHASE_0 , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 }   // down‐B
	  }
	},
	{ Buttons::Y, {
		{ 0.22f, AMPLITUDE_0, 85.0f,0.15f, 0.15f, PHASE_0  , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0},  // left‐Y { AMPLITUDE_0, 0.21f, 85.0f,   0.15f, 0.15f, PHASE_0 }
		{ AMPLITUDE_0, 0.21f, 85.0f,   0.15f, 0.15f, PHASE_0 , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0}  // right‐Y

	  }
	},
	{ Buttons::A, {
		{ AMPLITUDE_0,0.21f	,85.0f, 0.15f,  0.15f, 175.0f  , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0}, // left-A
		{0.22f,  AMPLITUDE_0,  85.0f, 0.15f, 0.15f, 175.0f , AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0},   // right‐A


	  }
	}
};
#endif