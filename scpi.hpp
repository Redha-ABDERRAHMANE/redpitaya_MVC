#pragma once


#include <iostream>
#include <string>
#include <SDL3_net/SDL_net.h>







class ScpiServer {
private:

    std::string delimiter = "\r\n";
    const char* hostAddress;
    int portNumber = 5000;
    NET_Address* netHostAddress = nullptr;
    bool connectionSuccess = false;
    NET_StreamSocket* clientTCPSocket = nullptr;

    char RecvBuffer[512] = { 0 };

public:
    ScpiServer(const char* host, const int& port = 5000) :hostAddress(host), portNumber(port) {

        netHostAddress = NET_ResolveHostname(hostAddress);

    }

    ~ScpiServer() {
        std::cout << "DESTRUCTOR CALLED\n";
        if (clientTCPSocket) NET_DestroyStreamSocket(clientTCPSocket);
        if (netHostAddress) NET_UnrefAddress(netHostAddress);



    }

    bool ConnectServer() {
        int resolveHAResult = NET_WaitUntilResolved(netHostAddress, 200);

        if (resolveHAResult == 0) {
            std::cout << "still resolving \n";
        }
        if (resolveHAResult == -1) {
            std::cout << "Failed to resolve host address \n";
            return false;
        }

        clientTCPSocket = NET_CreateClient(netHostAddress, portNumber);
        int socketConnectionStatus;
        while ((socketConnectionStatus = NET_WaitUntilConnected(clientTCPSocket, 500)) == 0);
        if (socketConnectionStatus == -1) {

            std::cout << "TCP Client Socket Creation failed: " << SDL_GetError() << "\n";

            return false;
        }
        connectionSuccess = true;
        std::cout << "TCP client socket creation success";
        return true;

    }









    bool tx_txt(const std::string& message) const {
        //if (clientTCPSocket==nullptr) return false;


        std::string full_message = message + delimiter;
        std::cout << "sending command " << full_message << std::endl;


        bool sentBytes = NET_WriteToStreamSocket(clientTCPSocket, (void*)full_message.c_str(), strlen(full_message.c_str()));

        //ADD IN CASE DATA ARE NOT SENT PROPERLY : NET_WaitUntilStreamSocketDrained

        if (!sentBytes) {
            std::cout << " failed because : " << SDL_GetError() << std::endl;
            return false;
        }

        //std::cout << "Data sending success" << std::endl;
        return true;
    }


    std::string rx_txt() {
        std::string message = "";
        int bytesReceived = -2;
        while (bytesReceived <= 0) {
            bytesReceived = NET_ReadFromStreamSocket(clientTCPSocket, (void*)RecvBuffer, sizeof(RecvBuffer) - 1);
        }

        if (bytesReceived > 0) {
            std::cout << "received some stuff\n";
            RecvBuffer[bytesReceived] = '\0';
            message += std::string(RecvBuffer);
            std::cout << message << "\n";
            // Check if we have the delimiter "\r\n"
            if (message.length() >= 2 && message.substr(message.length() - 2) == delimiter) {
                std::cout << message.substr(0, message.length() - 2) << "\n"; // Remove delimiter
                return message.substr(0, message.length() - 2); // Remove delimiter
            }
        }

        return "";





    }

    std::string txrx_txt(const std::string& message) {
        if (!tx_txt(message)) {
            std::cout << "failed" << std::endl;
            return "";  // Send failed

        }
        return rx_txt();  // Return received response
    }

    bool GetConnectionStatus()const {
        return connectionSuccess;
    }
};
