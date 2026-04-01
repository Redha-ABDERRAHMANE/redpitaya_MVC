

#ifndef LINEARSTAGE_H

#define LINEARSTAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdint.h>
#include <iostream>
#include <commonValues.h>

#include "Thorlabs.MotionControl.KCube.DCServo.h"
#pragma comment(lib, "Thorlabs.MotionControl.KCube.DCServo.lib")
//#pragma comment(lib, "Thorlabs.MotionControl.DeviceManager.lib")



// DU: Device unit
// RU: Real unit


class LinearStage
{

private:
    static constexpr int DEFAULTSERIALNUMBER = 27007209;
    static constexpr int BUFFERSIZE = 100;
    static constexpr int TYPEID = 27;
    static constexpr int POLLINGRATE = 200;
    static constexpr int SUCCESS = 0;
    static constexpr int DISTANCETYPE = 0;
    static constexpr int VELOCITYTYPE = 1;
    static constexpr int ACCELERATIONTYPE = 2;

    static constexpr double COUNTS_PER_MM = 34554.96;  // MTSZ-B8 encoder counts per mm
    static constexpr double COUNTS_PER_MM_VEL = 772981.4;  // velocity scaling (counts/s)
    static constexpr double COUNTS_PER_MM_ACC = 261.93;    // acceleration scaling


    const double defaultVelocityRU = 2.0; //2.0;
    const double defaultAccelerationRU = 100.0; //1.0;

    int serialNumber=0;


    int positionDU = 346020;  // 1mm ->34602 device units 
    int positionForwardDU = 346020;
    int positionBackwardDU = 0;



    int velocityDU = 0;
    int accelerationDU = 0;

    double velocityRU = 2;
    double accelerationRU = 100;

    char bufferSerialNo[16] = {};
    char bufferSerialNoList[100] = {};

    WORD messageType = 0;
    WORD messageId = 0;
    DWORD messageData = 0;

    short deviceSerialNo = 0;

    bool movingStage = false;










public:
    LinearStage(const int serialnumber = DEFAULTSERIALNUMBER) : serialNumber(serialnumber) { std::cout << "serial number " << serialNumber<<std::endl; }


    ~LinearStage() {
        StopMotion();
        CC_Close(bufferSerialNo);





    }


    bool ConnectToDevice() {
        if (DeviceConnected()) {
            std::cout << "Device already connected Serial number: " << bufferSerialNo << "\n";
            return true;
        }

        sprintf_s(bufferSerialNo, "%d", serialNumber);
        if (TLI_BuildDeviceList() != SUCCESS) {
            std::cout << "Failed to fetch device list \n";
            return false;
        }

        deviceSerialNo = TLI_GetDeviceListSize();

        if (TLI_GetDeviceListByTypeExt(bufferSerialNoList, BUFFERSIZE, TYPEID) != SUCCESS) {
            std::cout << "Failed to get the content of device list\n";
            return false;
        }

        if (!strstr(bufferSerialNoList, bufferSerialNo)) {
            std::cout << "Failed to find specified serial number in list\n";
            return false;
        }

        if (CC_Open(bufferSerialNo) != SUCCESS) {
            std::cout << "Failed to connect to device\n";
            return false;
        }

        if (!CC_StartPolling(bufferSerialNo, POLLINGRATE)) {
            std::cout << "Failed to start polling\n";
            return false;
        }

        Sleep(1000); // DC servo needs longer init time than stepper
        CC_ClearMessageQueue(bufferSerialNo);
        CC_RequestSettings(bufferSerialNo); // Request device state
        Sleep(400);

        // CC_LoadSettings failure is non-fatal now — log and continue
        if (!CC_LoadSettings(bufferSerialNo)) {
            std::cout << "Warning: CC_LoadSettings failed, using hardcoded MTSZ-B8 params\n";
        }

        CC_SetLimitsSoftwareApproachPolicy(bufferSerialNo, AllowAllMoves);

        if (!SetVelocityAccelerationParams(defaultAccelerationRU, defaultVelocityRU)) {
            std::cout << "Failed to set default velocity and acceleration settings\n";
            return false;
        }

        std::cout << "Linear stage connected and ready to use\n";
        return true;
    }


    bool Home() {
        if (!DeviceConnected()) return false;

        if (movingStage && !StopMotion()) return false;

        movingStage = CC_Home(bufferSerialNo) == SUCCESS ? true : false;




        while (movingStage && CC_WaitForMessage(bufferSerialNo, &messageType, &messageId, &messageData)) {
            if (messageType == 2 && messageId == SUCCESS) {


                movingStage = false;
                // Add Qsleep

            }
        }

        return true;





    }

    bool JogForward() {
        if (!DeviceConnected()) return false;

        if (movingStage && !StopMotion()) return false;

        return CC_MoveJog(bufferSerialNo, MOT_Forwards) == SUCCESS;


    }

    bool JogBackward() {
        if (!DeviceConnected()) return false;

        if (movingStage && !StopMotion()) return false;

        return CC_MoveJog(bufferSerialNo, MOT_Backwards) == SUCCESS;


    }

    bool MoveForward() {
        if (!DeviceConnected()) return false;

        if (movingStage && !StopMotion()) return false;


        movingStage = CC_MoveAtVelocity(bufferSerialNo, MOT_Forwards) == SUCCESS ? true : false;

        return movingStage;


    }

    bool MoveBackward() {
        if (!DeviceConnected()) return false;

        if (movingStage && !StopMotion()) return false;

        movingStage = CC_MoveAtVelocity(bufferSerialNo, MOT_Backwards) == SUCCESS ? true : false;

        return movingStage;


    }

    bool StopMotion() {

        movingStage = CC_StopProfiled(bufferSerialNo) == SUCCESS ? false : true;
        return !movingStage;

    }





    // ADD IN GUI 
    bool SetVelocityAccelerationParams(const double accelerationInMm, const double velocityInMm) {
        velocityDU = (int)(velocityInMm * COUNTS_PER_MM_VEL);
        accelerationDU = (int)(accelerationInMm * COUNTS_PER_MM_ACC);

        std::cout << "Serial " << serialNumber
            << " | velDU: " << velocityDU
            << " | accDU: " << accelerationDU << "\n";

        if (CC_SetVelParams(bufferSerialNo, accelerationDU, velocityDU) != SUCCESS) {
            std::cout << "Failed to set velocity/acceleration params\n";
            return false;
        }

        velocityRU = velocityInMm;
        accelerationRU = accelerationInMm;
        return true;
    }

    bool DeviceConnected() const {
        return CC_CheckConnection(bufferSerialNo);


    }



private:





};





#endif