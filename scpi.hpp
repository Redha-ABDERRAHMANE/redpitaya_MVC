#pragma once
#ifndef SCPI_H
#define SCPI_H

#include <QTcpSocket>
#include <iostream>



class ScpiClient: public QObject
{
	Q_OBJECT

public:
	ScpiClient(const char* address,int port=5000 ): clientSocket(new QTcpSocket(this)),hostAddress(address),hostPort(port){}
	~ScpiClient();

    bool connectToServer();
    bool tx_txt(const std::string& message);
    std::string rx_txt();
    std::string txrx_txt(const std::string& message);
    bool get_connectionStatus()const;


	
private:
	QTcpSocket* clientSocket;
	QString hostAddress;
	int hostPort;
    bool connectionSuccess = false;


};


#endif
