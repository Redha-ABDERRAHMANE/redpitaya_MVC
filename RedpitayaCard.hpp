#pragma once
#include"scpi.hpp"
#include <commonValues.h>
#include <SDL3_net/SDL_net.h>
class RedpitayaCards
{
private:
    
    ScpiServer rp_primary;
    std::array<ScpiServer, SLAVE_BOARDS> arrayRpSlaves;



public:

    RedpitayaCards(const char* hostAddress_primary, std::array<const char*,SLAVE_BOARDS> hostAddress_slave, const int& frequency = 5) :rp_primary(hostAddress_primary) {
        for (int i = 0;i < SLAVE_BOARDS;i++) {
            arrayRpSlaves[i] = ScpiServer(hostAddress_slave[i]);
        }
        std::cout << "frequency:" << frequency << std::endl;
        if (NET_Init() == 1) {
            ConnectConfigureRpBoards(frequency);
        }
        else {
            std::cout << "Could not initialize SDL net" << '\n';
        }




    }
    ~RedpitayaCards() {
        DisableDaisyChainConfig(rp_primary);
        for (ScpiServer& rp_slave : arrayRpSlaves) {
            DisableDaisyChainConfig(rp_slave);
        }
        NET_Quit();
    }


    bool ConnectConfigureRpBoards(const int& frequency=5) {
        rp_primary.ConnectServer();
        bool allRPSlavesConnected = true;

        for (ScpiServer& rp_slave : arrayRpSlaves) {
            rp_slave.ConnectServer();
            ////////////////
            //IF THERE IS ERROR LOOK FOR THIS LINE
            if (!rp_slave.GetConnectionStatus()) {
                allRPSlavesConnected = false;
            }
            
        }



        if (rp_primary.GetConnectionStatus() && allRPSlavesConnected) {
            ResetGenerators();
            SetDaisyChainSourceTriggerMasterBoard();
            SetDaisySourceTriggerSlaveBoard();
            CheckBoardDaisyConfiguration();
            SetInitialSourceSineWaveParams(PRIMARY_BOARD, frequency, PHASE_0);
            
            SetInitialSourceSineWaveParams(SECONDARY_BOARD, frequency, PHASE_0);
            SetInitialSourceSineWaveParams(TERTIARY_BOARD, frequency, PHASE_0);
            EnableAllBoardsOutputs();

            SetArmTriggerSlaveBoard();
            DisplayBoardsConfig();

            rp_primary.tx_txt("SOUR:TRig:INT");
            return true;

        }
        return false;
    }


    void send_txt(const int& card,std::string full_command) {
        ScpiServer& rp_board = card == PRIMARY_BOARD ? rp_primary : arrayRpSlaves[card-SLAVE_BOARDS];
        rp_board.tx_txt(full_command.c_str());


    }

    std::string send_txrxt(const int& card,std::string full_command) {
        ScpiServer& rp_board = card == PRIMARY_BOARD ? rp_primary : arrayRpSlaves[card-SLAVE_BOARDS];
        return rp_board.txrx_txt(full_command.c_str());


    }
    bool GetConnectionStatus()const {
        bool AllRPSlavesConnected = true;
        for (const ScpiServer& rp_slave : arrayRpSlaves) {
            if (!rp_slave.GetConnectionStatus()) { return false; }
        }
        return rp_primary.GetConnectionStatus();
    }


private:



    void ResetGenerators() {
        rp_primary.tx_txt("GEN:RST");
        for (ScpiServer& rp_slave : arrayRpSlaves) { rp_slave.tx_txt("GEN:RST"); }
        
    }


    void SetDaisyChainSourceTriggerMasterBoard() {

        rp_primary.tx_txt("DAISY:SYNC:TRig ON");
        rp_primary.tx_txt("DAISY:SYNC:CLK ON");
        rp_primary.tx_txt("DAISY:TRIG_O:SOUR DAC");  //Use DAC  as output trigger source(Trigger signal for secondary boad to start generating signal)
        rp_primary.tx_txt("TRIG:O:STATE ON");      //Make sure trigger output is enabled
        rp_primary.tx_txt("TRIG:O:LEV 1.0");       //Set trigger output level to 1V
        rp_primary.tx_txt("TRIG:O:DUR 0.001");       //Set trigger duration in seconds(1ms)
        rp_primary.tx_txt("TRIG:O:SLO POS");        //Set output trigger slope to positive

    }
    void SetDaisySourceTriggerSlaveBoard() {
        for (ScpiServer& rp_slave : arrayRpSlaves) {
            rp_slave.tx_txt("DAISY:SYNC:TRig ON");
            rp_slave.tx_txt("DAISY:SYNC:CLK ON");
            rp_slave.tx_txt("DAISY:TRIG_I:SOUR EXT");    // Setting internal trigger coming from external source ie. SOUR DAC of the primary board
            rp_slave.tx_txt("SOUR1:TRIG:SOUR EXT_PE");   // Use external positive edge trigger
            rp_slave.tx_txt("SOUR1:TRIG:LEV 0.5");       // Set trigger level threshold
            rp_slave.tx_txt("SOUR1:TRIG:DLY 0");         // No delay after trigger

            rp_slave.tx_txt("SOUR2:TRIG:SOUR EXT_PE");   // Use external positive edge trigger
            rp_slave.tx_txt("SOUR2:TRIG:LEV 0.5");       // Set trigger level threshold
            rp_slave.tx_txt("SOUR2:TRIG:DLY 0");         // No delay after trigger
        }
    }

    //TOPUT IN RPSIGNALGN

    void CheckBoardDaisyConfiguration() {
        printf("PRIMARY Trig Sync:  %s\n", (rp_primary.txrx_txt("DAISY:SYNC:TRig?")).c_str());
        printf("PRIMARY Clock Sync: %s\n", (rp_primary.txrx_txt("DAISY:SYNC:CLK?")).c_str());
        for (int i = 0; i < SLAVE_BOARDS;i++) {
            printf("SLAVE BOARD %d Trig Sync: %s\n", i + 1 ,arrayRpSlaves[i].txrx_txt("DAISY:SYNC:TRig?").c_str());
            printf("SLAVE BOARD %d  Clock Sync: %s\n",i+1, arrayRpSlaves[i].txrx_txt("DAISY:SYNC:CLK?").c_str());
        
        }
        
    }

    // Amplitude , frequency and phase are fixed at the start of the code.No need to put them as parameters
    void SetInitialSourceSineWaveParams(const int card, const int frequency, const float amplitude = AMPLITUDE_0, const int phase = PHASE_0) {
        ScpiServer& rp_board = card == PRIMARY_BOARD ? rp_primary : arrayRpSlaves[card-SLAVE_BOARDS];
        for (int source = 1; source <= 2; source++) {
            rp_board.tx_txt("SOUR" + std::to_string(source) + ":FUNC SINE");
            rp_board.tx_txt("SOUR" + std::to_string(source) + ":FREQ:FIX " + std::to_string(frequency));
            rp_board.tx_txt("SOUR" + std::to_string(source) + ":VOLT " + std::to_string(amplitude));
            rp_board.tx_txt("SOUR" + std::to_string(source) + ":PHAS " + std::to_string(phase));
        }
    }

    void EnableBoardOutputs(ScpiServer& board) {
        board.tx_txt("OUTPUT1:STATE ON");
        board.tx_txt("OUTPUT2:STATE ON");
    }
    void EnableAllBoardsOutputs() {
        EnableBoardOutputs(rp_primary);
        for (ScpiServer& rp_slave : arrayRpSlaves) {
            EnableBoardOutputs(rp_slave);
        }

    }

    // Make the slave board sources wait for the external trigger
    void SetArmTriggerSlaveBoard() {
        
        for (ScpiServer& rp_slave : arrayRpSlaves) {
            rp_slave.tx_txt("SOUR1:TRIG:ARM");
            rp_slave.tx_txt("SOUR2:TRIG:ARM");
        }
    }

    void DisableDaisyChainConfig(ScpiServer& rp_board) {
        rp_board.tx_txt("DAISY:SYNC:TRig OFF");
        rp_board.tx_txt("DAISY:SYNC:CLK OFF");
    }
    void DisplayBoardsConfig() {
        printf("PRIMARY Output state : %s\n", rp_primary.txrx_txt("OUTPUT1:STATE?").c_str());
        printf("PRIMARY Trigger source: %s\n", rp_primary.txrx_txt("SOUR1:TRIG:SOUR?").c_str());
        printf("PRIMARY Trigger Output source: %s\n", rp_primary.txrx_txt("DAISY:TRIG_O:SOUR?").c_str());


        for (int i = 0; i < SLAVE_BOARDS; i++) {  // replace arraySize with your actual array size
            printf("SLAVE BOARD %d Output1 state : %s\n", i + 1, arrayRpSlaves[i].txrx_txt("OUTPUT1:STATE?").c_str());
            printf("SLAVE BOARD %d Output2 state : %s\n", i + 1, arrayRpSlaves[i].txrx_txt("OUTPUT2:STATE?").c_str());
            printf("SLAVE BOARD %d Trigger source1: %s\n", i + 1, arrayRpSlaves[i].txrx_txt("SOUR1:TRIG:SOUR?").c_str());
            printf("SLAVE BOARD %d Trigger source2: %s\n", i + 1, arrayRpSlaves[i].txrx_txt("SOUR2:TRIG:SOUR?").c_str());
        }

    }





};
