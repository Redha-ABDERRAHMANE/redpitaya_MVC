
#include<QSerialPort>
#include<QSerialPortInfo>
#include <iostream>
#include <QObject>
#include <QDebug>
class CapacitiveBankManager: public QObject
{
	Q_OBJECT
public:
	CapacitiveBankManager(QObject* parent = nullptr) {}
	~CapacitiveBankManager() {
		DisconnectSerialPortDevice();

	}

private:
	bool ConnectToSerialPortDevice() {
		if (availableDevicePortList.size() != 0) {
			deviceSerialPort = new QSerialPort(availableDevicePortList[0], this);

			deviceSerialPort->setBaudRate(QSerialPort::Baud115200);
			deviceSerialPort->setDataBits(QSerialPort::Data8);
			deviceSerialPort->setParity(QSerialPort::NoParity);
			deviceSerialPort->setStopBits(QSerialPort::OneStop);
			deviceSerialPort->setFlowControl(QSerialPort::NoFlowControl);

			if (!deviceSerialPort->open(QIODeviceBase::ReadWrite)) {
				std::cout << "Failed to open port\n";
				return false;
			}
			std::cout << "Device port open and connected \n";
			return true;


		}
		return false;
	}

	bool LookForAvailableDevices() {
		availableDevicePortList = QSerialPortInfo::availablePorts();
		if (availableDevicePortList.size() == 0) {
			std::cout << "NO SERIAL PORT DEVICE FOUND \n";
			return false;
		}

		return true;


	}

	bool SendData(const char* data) {
		std::cout << " DATA TO SEND" << data;
		qint64 bytesWritten = deviceSerialPort->write(data, strlen(data));

		if (bytesWritten == -1) {
			qDebug() << "TX failed:" << deviceSerialPort->errorString();
			
			return false;
		}

		// Wait for data to be written
		if (!deviceSerialPort->waitForBytesWritten(1000)) {
			qDebug() << "Write timeout:" << deviceSerialPort->errorString();
			
			return false;
		}
		return true;
	}

	QByteArray ReceiveData() {
		if (deviceSerialPort->waitForReadyRead(3000)) {  // Wait up to 3 seconds
			QByteArray rx = deviceSerialPort->readAll();
			return rx;
		}
		
		qDebug() << "No response received:" << deviceSerialPort->errorString();
		return QByteArray();

	}
	 bool DisconnectSerialPortDevice() {
		 if (deviceSerialPort) {
			 deviceSerialPort->close();
			 deviceSerialPort->deleteLater();
			 deviceSerialPort = nullptr;
			 std::cout << "device port closed \n";
			 return true;
		 }
		 return false;
	 }


public :

	bool ConnectToDevice() {
		if (LookForAvailableDevices()) {
			return ConnectToSerialPortDevice();
		}
		return false;
	}

	bool sendFrequencyChange(int frequency) {
		if (SendData(std::to_string(frequency).c_str())) {
			if (ReceiveData() == std::to_string(frequency)) {
				qDebug() << "FREQUENCY CHANGE SUCCESS";
				return true;
			}
		}
		return false;

	}




private:
	QList<QSerialPortInfo> availableDevicePortList = {};
	QSerialPort* deviceSerialPort=nullptr;
};

