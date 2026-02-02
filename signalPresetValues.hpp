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
	{ Buttons::BUMPER_RIGHT, preset_array_t{ 0.16f, 0.15f, PHASE_0, 0.065f, 0.062f, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
	{ Buttons::HAT_UP,       preset_array_t{ AMPLITUDE_0, 0.29f, PHASE_0, 0.065f, 0.062f, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
	{ Buttons::HAT_DOWN,     preset_array_t{ 0.295f, AMPLITUDE_0, PHASE_0, 0.065f, 0.062f, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
	{ Buttons::HAT_RIGHT,    preset_array_t{ 0.16f, 0.15f, PHASE_0, AMPLITUDE_0, 0.122f, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
	{ Buttons::HAT_LEFT,     preset_array_t{ 0.16f, 0.15f, PHASE_0, 0.129f, AMPLITUDE_0, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } }
};
// Button combinations - each button has 2 presets (up/down with X/B or left/right with Y/A variants)
inline std::map<int, pair_p_array_t> dictionary_buttonPreset_XY{
	{ Buttons::X, {
		{ AMPLITUDE_0, 0.29f, 90.0f, 0.065f, 0.062f, PHASE_0,  AMPLITUDE_0,	AMPLITUDE_0	, PHASE_0 }, // hatup‐X{ 0.11f, 0.11f, 85.0f,   AMPLITUDE_0, 0.32f, 175.0f } k
		{ 0.295f, AMPLITUDE_0, 90.0f, 0.065f, 0.062f, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } // hatdown‐X k
	  }
	},
	{ Buttons::B, {
		{ AMPLITUDE_0, 0.29f, PHASE_0, 0.065f, 0.062f, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 }, // hatup‐B k
		{ 0.295f, AMPLITUDE_0, PHASE_0, 0.065f, 0.062f, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } // hatdown‐B k
	  }
	},
	{ Buttons::Y, {
		{ 0.16f, 0.15f, PHASE_0, AMPLITUDE_0, 0.122f, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 }, // hatleft Y k
		{ 0.16f, 0.15f, 90.0f, 0.129f, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } // hatright Y k

	  }
	},
	{ Buttons::A, {
		{ 0.16f, 0.15f, PHASE_0, 0.129f, AMPLITUDE_0, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 }, // hatleft-A k
		{ 0.16f, 0.15f, PHASE_0, AMPLITUDE_0, 0.122f, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } // hatright‐A k


	  }
	}
};


inline std::map<int, preset_array_t> dictionary_bumperHatPreset_XZ{
{ Buttons::BUMPER_LEFT,  preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::BUMPER_RIGHT, preset_array_t{ 0.16f, 0.15f, 90.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.125f, 0.125f, PHASE_0 } },
{ Buttons::HAT_UP,       preset_array_t{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f} },
{ Buttons::HAT_DOWN,     preset_array_t{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f } },
{ Buttons::HAT_RIGHT,    preset_array_t{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f } },
{ Buttons::HAT_LEFT,     preset_array_t{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f } }
};
// Button combinations - each button has 2 presets (up/down or left/right variants)
inline std::map<int, pair_p_array_t> dictionary_buttonPreset_XZ{
	{ Buttons::X, {
		{ 0.0f, 0.0f, 0.0f,AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f }, // hatup‐X
		{ 0.0f, 0.0f, 0.0f,AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f } // hatdown-X
	  }
	},
	{ Buttons::B, {
		{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f }, // hatup‐B
		{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f } // hatdown-B
	  }
	},
	{ Buttons::Y, {
		{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f }, // hatleft-Y
		{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f } // hatright-Y

	  }
	},
	{ Buttons::A, {
		{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f}, // hatleft-A
		{ 0.0f, 0.0f, 0.0f, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f} // hatright-A


	  }
	}
};


inline std::map<int, preset_array_t> dictionary_bumperHatPreset_YZ{
{ Buttons::BUMPER_LEFT,  preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0, AMPLITUDE_0, AMPLITUDE_0, PHASE_0 } },
{ Buttons::BUMPER_RIGHT, preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.065f, 0.062f, 90.0f, 0.125f, 0.125f, PHASE_0 } },
{ Buttons::HAT_UP,       preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } },
{ Buttons::HAT_DOWN,     preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } },
{ Buttons::HAT_RIGHT,    preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } },
{ Buttons::HAT_LEFT,     preset_array_t{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } }
};
// Button combinations - each button has 2 presets (up/down or left/right variants)
inline std::map<int, pair_p_array_t> dictionary_buttonPreset_YZ{
	{ Buttons::X, {
		{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }, //hatup‐X
		{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } // hatdown-X
	  }
	},
	{ Buttons::B, {
		{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // hatup-B
		{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } // hatdown-B
	  }
	},
	{ Buttons::Y, {
		{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // hatleft-Y
		{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f} // hatright-Y

	  }
	},
	{ Buttons::A, {
		{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // hatleft-A
		{ AMPLITUDE_0, AMPLITUDE_0, PHASE_0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f} // hatright-A


	  }
	}
};

inline std::array< pair_dimension_dictionnary_t, Dimensions::DIMENSIONSIZE> arrayDimensionDictionnaries = {
	pair_dimension_dictionnary_t{dictionary_bumperHatPreset_XY,dictionary_buttonPreset_XY},
	pair_dimension_dictionnary_t{dictionary_bumperHatPreset_XZ,dictionary_buttonPreset_XZ},
	pair_dimension_dictionnary_t{dictionary_bumperHatPreset_YZ,dictionary_buttonPreset_YZ}
};
#endif