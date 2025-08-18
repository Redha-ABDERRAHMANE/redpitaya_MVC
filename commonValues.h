#ifndef COMMONVALUES_H
#define COMMONVALUES_H
#define IsEqual(a,b) (abs((a)-(b)) < EPSILON)
#include <array>
constexpr double EPSILON = 1E-3;

 enum AmplitudeValues {
    AMPLITUDE_0 = 0,
    AMPLITUDE_MAX = 1

};

enum FrequencyValues{
    DEFAULT_FREQUENCY = 5,
    FREQUENCY_0 = 0,
    FREQUENCY_MAX = 1000
};

enum PhaseValues{
    PHASE_0 = 0,
    PHASE_50 = 50,
    PHASE_90 = 90,
    PHASE_140 = 140,
    PHASE_180 = 180
};

enum BoardChannel {
    PRIMARY_BOARD_FIRST_AMP_INDEX = 0,
    PRIMARY_BOARD_SECOND_AMP_INDEX = 1,
    PRIMARY_BOARD_COMMON_PHASE_INDEX = 2,
    SECONDARY_BOARD_FIRST_AMP_INDEX = 3,
    SECONDARY_BOARD_SECOND_AMP_INDEX = 4,
    SECONDARY_BOARD_COMMON_PHASE_INDEX = 5,
    TERTIARY_BOARD_FIRST_AMP_INDEX = 6,
    TERTIARY_BOARD_SECOND_AMP_INDEX = 7,
    TERTIARY_BOARD_COMMON_PHASE_INDEX = 8,
};

enum SignalGnSource {
    SOURCE_1 = 1,
    SOURCE_2 = 2,


};

const enum RPBoards {
    PRIMARY_BOARD = 1,
    SECONDARY_BOARD = 2,
    TERTIARY_BOARD = 3,
    SLAVE_BOARDS = 2

};

const enum LinearStageMotion:int {
    MOVEBACKWARD,
    STOPMOTION,
    MOVEFORWARD,
    HOME,
    JOGBACKWARD,
    JOGFORWARD,
    MOTIONSIZE,
    MOVESIZE = 4,
    JOGSIZE = 2


};

const enum CameraFrameResolution {
    FRAMEWIDTH = 2584,//2592,
    FRAMEHEIGHT= 1942//1944
};
const enum Dimensions {
    XY,
    XZ,
    YZ,
    DIMENSIONSIZE
};


const std::array<const char*, SLAVE_BOARDS> arraySlaveBoardsIP = { };



#endif // COMMONVALUES_H
