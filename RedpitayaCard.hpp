#pragma once
#include"scpi.hpp"
#include <commonValues.h>

class RedpitayaCards
{
private:
    scpi rp_primary;
    scpi rp_secondary;



public:

    RedpitayaCards(const char* host_primary, const char* host_secondary, const int& frequency) :rp_primary(host_primary), rp_secondary(host_secondary) {
        std::cout<<"frequency:"<<frequency<<std::endl;
        reset_generators();
        set_DaisyChain_SourceTrigger_MasterBoard();
        set_DaisyChain_SourceTrigger_SlaveBoard();
        verify_board_daisy_configuration();
        set_InitialSource_sineWave_parameters(PRIMARY_BOARD, frequency,PHASE_0);
        set_InitialSource_sineWave_parameters(SECONDARY_BOARD, frequency,PHASE_0);
        enable_board_outputs();
        set_arm_trigger_slave_board(rp_secondary);
        displayBoardsConfig();

        rp_primary.tx_txt("SOUR:TRig:INT");


    }
    ~RedpitayaCards() {
        disable_daisy_chain_configuration(rp_primary);
        disable_daisy_chain_configuration(rp_secondary);
    }



    void send_txt(const int& card,std::string full_command) {
        scpi& rp_board = card == PRIMARY_BOARD ? rp_primary : rp_secondary;
        rp_board.tx_txt(full_command.c_str());


    }

    std::string send_txrxt(const int& card,std::string full_command) {
        scpi& rp_board = card == PRIMARY_BOARD ? rp_primary : rp_secondary;
        return rp_board.txrx_txt(full_command.c_str());


    }

private:



    void reset_generators() {
        rp_primary.tx_txt("GEN:RST");
        rp_secondary.tx_txt("GEN:RST");
    }


    void set_DaisyChain_SourceTrigger_MasterBoard() {

        rp_primary.tx_txt("DAISY:SYNC:TRig ON");
        rp_primary.tx_txt("DAISY:SYNC:CLK ON");
        rp_primary.tx_txt("DAISY:TRIG_O:SOUR DAC");  //Use DAC  as output trigger source(Trigger signal for secondary boad to start generating signal)
        rp_primary.tx_txt("TRIG:O:STATE ON");      //Make sure trigger output is enabled
        rp_primary.tx_txt("TRIG:O:LEV 1.0");       //Set trigger output level to 1V
        rp_primary.tx_txt("TRIG:O:DUR 0.001");       //Set trigger duration in seconds(1ms)
        rp_primary.tx_txt("TRIG:O:SLO POS");        //Set output trigger slope to positive

    }
    void set_DaisyChain_SourceTrigger_SlaveBoard() {
        rp_secondary.tx_txt("DAISY:SYNC:TRig ON");
        rp_secondary.tx_txt("DAISY:SYNC:CLK ON");
        rp_secondary.tx_txt("DAISY:TRIG_I:SOUR EXT");    // Setting internal trigger coming from external source ie. SOUR DAC of the primary board
        rp_secondary.tx_txt("SOUR1:TRIG:SOUR EXT_PE");   // Use external positive edge trigger
        rp_secondary.tx_txt("SOUR1:TRIG:LEV 0.5");       // Set trigger level threshold
        rp_secondary.tx_txt("SOUR1:TRIG:DLY 0");         // No delay after trigger

        rp_secondary.tx_txt("SOUR2:TRIG:SOUR EXT_PE");   // Use external positive edge trigger
        rp_secondary.tx_txt("SOUR2:TRIG:LEV 0.5");       // Set trigger level threshold
        rp_secondary.tx_txt("SOUR2:TRIG:DLY 0");         // No delay after trigger
    }

    //TOPUT IN RPSIGNALGN

    void verify_board_daisy_configuration() {
        printf("PRIMARY Trig Sync:  %s\n", (rp_primary.txrx_txt("DAISY:SYNC:TRig?")).c_str());
        printf("PRIMARY Clock Sync: %s\n", (rp_primary.txrx_txt("DAISY:SYNC:CLK?")).c_str());
        printf("SECONDARY Trig Sync: %s\n", (rp_secondary.txrx_txt("DAISY:SYNC:TRig?")).c_str());
        printf("SECONDARY Clock Sync: %s\n", (rp_secondary.txrx_txt("DAISY:SYNC:CLK?")).c_str());
    }

    // Amplitude , frequency and phase are fixed at the start of the code.No need to put them as parameters
    void set_InitialSource_sineWave_parameters(const int card, const int frequency, const float amplitude = AMPLITUDE_0, const int phase = PHASE_0) {
        scpi& rp_board = card == PRIMARY_BOARD ? rp_primary : rp_secondary;
        for (int source = 1; source <= 2; source++) {
            rp_board.tx_txt("SOUR" + std::to_string(source) + ":FUNC SINE");
            rp_board.tx_txt("SOUR" + std::to_string(source) + ":FREQ:FIX " + std::to_string(frequency));
            rp_board.tx_txt("SOUR" + std::to_string(source) + ":VOLT " + std::to_string(amplitude));
            rp_board.tx_txt("SOUR" + std::to_string(source) + ":PHAS " + std::to_string(phase));
        }
    }

    void enable_SingleBoard_outputs(scpi& board) {
        board.tx_txt("OUTPUT1:STATE ON");
        board.tx_txt("OUTPUT2:STATE ON");
    }
    void enable_board_outputs() {
        enable_SingleBoard_outputs(rp_primary);
        enable_SingleBoard_outputs(rp_secondary);

    }

    // Make the slave board sources wait for the external trigger
    void set_arm_trigger_slave_board(scpi& rp_board) {
        (void)(rp_board);
        rp_secondary.tx_txt("SOUR1:TRIG:ARM");
        rp_secondary.tx_txt("SOUR2:TRIG:ARM");
    }

    void disable_daisy_chain_configuration(scpi& rp_board) {
        rp_board.tx_txt("DAISY:SYNC:TRig OFF");
        rp_board.tx_txt("DAISY:SYNC:CLK OFF");
    }
    void displayBoardsConfig() {
        printf("PRIMARY Output state : %s\n", rp_primary.txrx_txt("OUTPUT1:STATE?").c_str());
        printf("SECONDARY Output state : %s\n", rp_secondary.txrx_txt("OUTPUT1:STATE?").c_str());
        printf("SECONDARY Output state : %s\n", rp_secondary.txrx_txt("OUTPUT2:STATE?").c_str());
        printf("PRIMARY Trigger source: %s\n", rp_primary.txrx_txt("SOUR1:TRIG:SOUR?").c_str());
        printf("SECONDARY Trigger source: %s\n", rp_secondary.txrx_txt("SOUR1:TRIG:SOUR?").c_str());
        printf("SECONDARY Trigger source: %s\n", rp_secondary.txrx_txt("SOUR2:TRIG:SOUR?").c_str());
        printf("PRIMARY Trigger Output source: %s\n", rp_primary.txrx_txt("DAISY:TRIG_O:SOUR?").c_str());
        printf("SECONDARY Trigger source: %s\n", rp_secondary.txrx_txt("SOUR1:TRIG:SOUR?").c_str());

    }



};
