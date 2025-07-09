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
    PRIMARY_BOARD_FIRST_AMP = 0,
    PRIMARY_BOARD_SECOND_AMP = 1,
    PRIMARY_BOARD_COMMON_PHASE = 2,
    SECONDARY_BOARD_FIRST_AMP = 3,
    SECONDARY_BOARD_SECOND_AMP = 4,
    SECONDARY_BOARD_COMMON_PHASE = 5
};

enum SignalGnSource {
    SOURCE_1 = 1,
    SOURCE_2 = 2,


};

enum RPBoards {
    PRIMARY_BOARD = 1,
    SECONDARY_BOARD = 2

};


#endif // COMMONVALUES_H
