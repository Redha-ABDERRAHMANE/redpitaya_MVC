#pragma once

// Critical: Include winsock2.h BEFORE windows.h to avoid redefinitions
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Exclude problematic Windows components
#define NOMINMAX        // Prevents min/max macro conflicts
#define NOGDI          // Excludes GDI
//#define NOUSER         // Excludes user interface
#define NOSERVICE      // Excludes service functions
#define NOMCX          // Excludes modem configuration
#define NOIME          // Excludes IME functions
#define NOSOUND        // Excludes sound functions

// Include Winsock2 FIRST, then Windows
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <iostream>
#include <string>

// Add this pragma for automatic linking in Visual Studio:
#pragma comment(lib, "ws2_32.lib")





class ScpiServer {
private:

    std::string delimiter = "\r\n";
    const char* hostAddress;
    int portNumber;
    bool connectionSuccess ;

    WSADATA winSockData;
    int iWsaStartup;
    //int iWsaCleanup;

    SOCKET TCPClientSocket;
    int iCloseSocket;
    struct sockaddr_in TCPServerAdd;
    int iConnect;

    //int iSend;

    //int iRecv;
    char RecvBuffer[512];
    //int iRecvBuffer;


    //Step-1 WSAStartup Fun------------------------------------
public:
    ScpiServer(const char* host, const int& port = 5000):iCloseSocket(),iConnect(),RecvBuffer() {
        hostAddress = host;
        portNumber = port;
        
    }

    ~ScpiServer() {
        //STEP - 7 Close Socket
        std::cout << "entered scpi destructor" << std::endl;
        shutdown(TCPClientSocket, SD_BOTH);
        iCloseSocket = closesocket(TCPClientSocket);
        if (iCloseSocket == SOCKET_ERROR)
        {
            std::cout << "Closing Failed & Error No->" << WSAGetLastError() << std::endl;
        }
        std::cout << "Closing Socket success" << std::endl;
        WSACleanup();
    }

    void SetConnectionToSCPIServer() {
        if (InitializeiWsaStartup() && InitializeTCPClientSocket() && InitializeTCPServerStruct() && ConnectServer()) {
            connectionSuccess = true;
       }
        else { connectionSuccess = false; }


    }

    bool InitializeiWsaStartup(){


        iWsaStartup = WSAStartup(MAKEWORD(2, 2), &winSockData);
        if (iWsaStartup != 0)
        {
            std::cout << "WSAStartup Failed" << std::endl;
            
            return false;
        }
        std::cout << "WSAStartup Success" << std::endl;
        return true;

    }

    bool InitializeTCPClientSocket() {

        TCPClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (TCPClientSocket == INVALID_SOCKET)
        {
            std::cout << "TCP Client Socket Creation failed" << WSAGetLastError() << std::endl;
           
            return false;
        }
        std::cout << "TCP client socket creation success";
        return true;

    }

    bool InitializeTCPServerStruct() {

        TCPServerAdd.sin_family = AF_INET;
        if (inet_pton(AF_INET, hostAddress, &TCPServerAdd.sin_addr) != 1) {
            std::cout << "Invalid IPv4 address: " << hostAddress << std::endl;
            return false;
        }
        TCPServerAdd.sin_port = htons(portNumber);
        return true;

    }
    bool ConnectServer() {

        iConnect = connect(TCPClientSocket, (SOCKADDR*)&TCPServerAdd, sizeof(TCPServerAdd));
        if (iConnect == SOCKET_ERROR)
        {
            std::cout << "Connection failed & Error No ->" << WSAGetLastError() << std::endl;
            return false;
        }
        std::cout << "Connection success" << std::endl;
        return true;
    }







    // STEP-6 Send Data to the server

    bool tx_txt(const std::string& message) const {
        std::string full_message = message + delimiter;  // Add \r\n like Python

        int bytes_sent = send(TCPClientSocket, full_message.c_str(), (int)full_message.length(), 0);

        if (bytes_sent == SOCKET_ERROR) {
            //std::cout << "Sending Failed & Error No->" << WSAGetLastError() << std::endl;
            return false;
        }

        //std::cout << "Data sending success" << std::endl;
        return true;
    }


    std::string rx_txt(int chunksize = 4096) {
        std::string message = "";

        while (true) {
            int bytes_received = recv(TCPClientSocket, RecvBuffer, (chunksize < (int)sizeof(RecvBuffer) - 1 ? chunksize : (int)sizeof(RecvBuffer) - 1), 0);

            if (bytes_received == SOCKET_ERROR) {
                std::cout << "Receiving Failed & Error No->" << WSAGetLastError() << std::endl;
                return "";
            }

            if (bytes_received == 0) {
                std::cout << "Connection closed by server" << std::endl;
                return "";
            }

            // Null terminate and add to message
            RecvBuffer[bytes_received] = '\0';
            message += std::string(RecvBuffer);

            // Check if we have the delimiter "\r\n"
            if (message.length() >= 2 &&
                message.substr(message.length() - 2) == delimiter) {
                return message.substr(0, message.length() - 2); // Remove delimiter
            }
        }
    }

    std::string txrx_txt(const std::string& message) {
        if (!tx_txt(message)) {
            return "";  // Send failed
            std::cout<<"failed"<<std::endl;
        }
        return rx_txt();  // Return received response
    }

    bool GetConnectionStatus()const {
        return connectionSuccess;
    }
};


