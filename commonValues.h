#ifndef COMMONVALUES_H
#define COMMONVALUES_H



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
    SECONDARY_BOARD_COMMON_PHASE_INDEX = 5
};

enum SignalGnSource {
    SOURCE_1 = 1,
    SOURCE_2 = 2,


};

const enum RPBoards {
    PRIMARY_BOARD = 1,
    SECONDARY_BOARD = 2

};

const enum LinearStageMotion:int {
    MOVEBACKWARD,
    STOPMOTION,
    MOVEFORWARD,
    HOME,
    JOGFORWARD,
    JOGBACKWARD,
    MOTIONSIZE,
    MOVESIZE = 4,
    JOGSIZE = 2


};



#endif // COMMONVALUES_H
