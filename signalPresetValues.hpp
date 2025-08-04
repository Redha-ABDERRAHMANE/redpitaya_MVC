#ifndef   SIGNALPRESETVALUES_H
#define  SIGNALPRESETVALUES_H
#include <map>
#include <array>
#include <controller.hpp>
#include "commonValues.h"

typedef std::array<float, 9> preset_array_t;
typedef std::pair<preset_array_t, preset_array_t> pair_p_array_t;
typedef std::pair<std::map<int, preset_array_t>&, std::map<int, pair_p_array_t>&> pair_dimension_dictionnary_t;

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
{ Buttons::BUMPER_LEFT,  preset_array_t{ -20.0f, -20.0f, -20.0f, -20.0f, -20.0f, -20.0f, -20.0f, -20.0f, -20.0f } },
{ Buttons::BUMPER_RIGHT, preset_array_t{ -19.0f, -19.0f, -19.0f, -19.0f, -19.0f, -19.0f, -19.0f, -19.0f, -19.0f } },
{ Buttons::HAT_UP,       preset_array_t{ -18.0f, -18.0f, -18.0f, -18.0f, -18.0f, -18.0f, -18.0f, -18.0f, -18.0f} },
{ Buttons::HAT_DOWN,     preset_array_t{ -17.0f, -17.0f, -17.0f, -17.0f, -17.0f, -17.0f, -17.0f, -17.0f, -17.0f } },
{ Buttons::HAT_RIGHT,    preset_array_t{ -16.0f, -16.0f, -16.0f, -16.0f, -16.0f, -16.0f, -16.0f, -16.0f, -16.0f } },
{ Buttons::HAT_LEFT,     preset_array_t{ -15.0f, -15.0f, -15.0f, -15.0f, -15.0f, -15.0f, -15.0f, -15.0f, -15.0f } }
};
// Button combinations - each button has 2 presets (up/down or left/right variants)
inline std::map<int, pair_p_array_t> dictionary_buttonPreset_XZ{
	{ Buttons::X, {
		{ -14.0f, -14.0f, -14.0f, -14.0f, -14.0f, -14.0f, -14.0f, -14.0f, -14.0f } ,
		{ -13.0f, -13.0f, -13.0f, -13.0f, -13.0f, -13.0f, -13.0f, -13.0f, -13.0f }
	  }
	},
	{ Buttons::B, {
		{ -12.0f, -12.0f, -12.0f, -12.0f, -12.0f, -12.0f, -12.0f, -12.0f, -12.0f},
		{ -11.0f, -11.0f, -11.0f, -11.0f, -11.0f, -11.0f, -11.0f, -11.0f, -11.0f }
	  }
	},
	{ Buttons::Y, {
		{ -10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f},
		{ -9.0f, -9.0f, -9.0f, -9.0f, -9.0f, -9.0f, -9.0f, -9.0f, -9.0f}

	  }
	},
	{ Buttons::A, {
		{ -8.0f, -8.0f, -8.0f, -8.0f, -8.0f, -8.0f, -8.0f, -8.0f, -8.0f},
		{ -7.0f, -7.0f, -7.0f, -7.0f, -7.0f, -7.0f, -7.0f, -7.0f, -7.0f}


	  }
	}
};


inline std::map<int, preset_array_t> dictionary_bumperHatPreset_YZ{
{ Buttons::BUMPER_LEFT,  preset_array_t{ -6.0f, -6.0f, -6.0f, -6.0f, -6.0f, -6.0f, -6.0f, -6.0f, -6.0f } },
{ Buttons::BUMPER_RIGHT, preset_array_t{ -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f } },
{ Buttons::HAT_UP,       preset_array_t{ -4.0f, -4.0f, -4.0f, -4.0f, -4.0f, -4.0f, -4.0f, -4.0f, -4.0f } },
{ Buttons::HAT_DOWN,     preset_array_t{ -3.0f, -3.0f, -3.0f, -3.0f, -3.0f, -3.0f, -3.0f, -3.0f, -3.0f } },
{ Buttons::HAT_RIGHT,    preset_array_t{ -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f } },
{ Buttons::HAT_LEFT,     preset_array_t{ -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f } }
};
// Button combinations - each button has 2 presets (up/down or left/right variants)
inline std::map<int, pair_p_array_t> dictionary_buttonPreset_YZ{
	{ Buttons::X, {
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } ,
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }
	  }
	},
	{ Buttons::B, {
		{ 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f},
		{ 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f }
	  }
	},
	{ Buttons::Y, {
		{ 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f},
		{ 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f}

	  }
	},
	{ Buttons::A, {
		{ 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f},
		{ 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f}


	  }
	}
};

inline std::array< pair_dimension_dictionnary_t, Dimensions::DIMENSIONSIZE> arrayDimensionDictionnaries = {
	pair_dimension_dictionnary_t{dictionary_bumperHatPreset_XY,dictionary_buttonPreset_XY},
	pair_dimension_dictionnary_t{dictionary_bumperHatPreset_XZ,dictionary_buttonPreset_XZ},
	pair_dimension_dictionnary_t{dictionary_bumperHatPreset_YZ,dictionary_buttonPreset_YZ}
};
#endif