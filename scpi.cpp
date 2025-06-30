#include "scpi.hpp"

ScpiClient::~ScpiClient()
{
}



bool ScpiClient::tx_txt(const std::string& message) {
    std::cout << "tx called \n";
    QByteArray byteArray(message.c_str(), message.length());

    qint64 writeState = clientSocket->write(byteArray);
    return writeState != -1 ? true : false;

}
std::string ScpiClient::rx_txt() {
    char buffer[1024];

    qint64 readState = clientSocket->readLine(buffer, sizeof(buffer));
    if (readState != -1) {
        return std::string(buffer);
    }
    return "rx failed";

}

std::string ScpiClient::txrx_txt(const std::string& message) {
    bool txSuccess = tx_txt(message);
    if (txSuccess) {
        return rx_txt();
    }
    return "txrx failed ";



}

bool ScpiClient::get_connectionStatus() const {
    return connectionSuccess;
}



bool ScpiClient::connectToServer() {
    clientSocket->connectToHost(hostAddress, hostPort);
    if (clientSocket->waitForConnected(5000)) {
        connectionSuccess = true;
        std::cout << "Connected successfully!" << std::endl;
        return true;
    }
    switch (clientSocket->error()) {
    case QAbstractSocket::ConnectionRefusedError:
        std::cout << "The connection was refused by the peer (or timed out)." << std::endl;
        break;
    case QAbstractSocket::RemoteHostClosedError:
        std::cout << "The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent." << std::endl;
        break;
    case QAbstractSocket::HostNotFoundError:
        std::cout << "The host address was not found." << std::endl;
        break;
    case QAbstractSocket::SocketAccessError:
        std::cout << "The socket operation failed because the application lacked the required privileges." << std::endl;
        break;
    case QAbstractSocket::SocketResourceError:
        std::cout << "The local system ran out of resources (e.g., too many sockets)." << std::endl;
        break;
    case QAbstractSocket::SocketTimeoutError:
        std::cout << "The socket operation timed out." << std::endl;
        break;
    case QAbstractSocket::DatagramTooLargeError:
        std::cout << "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes)." << std::endl;
        break;
    case QAbstractSocket::NetworkError:
        std::cout << "An error occurred with the network (e.g., the network cable was accidentally plugged out)." << std::endl;
        break;
    case QAbstractSocket::AddressInUseError:
        std::cout << "The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive." << std::endl;
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        std::cout << "The address specified to QAbstractSocket::bind() does not belong to the host." << std::endl;
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        std::cout << "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support)." << std::endl;
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        std::cout << "Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background)." << std::endl;
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        std::cout << "The socket is using a proxy, and the proxy requires authentication." << std::endl;
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        std::cout << "The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)" << std::endl;
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        std::cout << "Could not contact the proxy server because the connection to that server was denied" << std::endl;
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        std::cout << "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)" << std::endl;
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        std::cout << "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase." << std::endl;
        break;
    case QAbstractSocket::ProxyNotFoundError:
        std::cout << "The proxy address set with setProxy() (or the application proxy) was not found." << std::endl;
        break;
    case QAbstractSocket::ProxyProtocolError:
        std::cout << "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood." << std::endl;
        break;
    case QAbstractSocket::OperationError:
        std::cout << "An operation was attempted while the socket was in a state that did not permit it." << std::endl;
        break;
    case QAbstractSocket::SslInternalError:
        std::cout << "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library." << std::endl;
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        std::cout << "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library." << std::endl;
        break;
    case QAbstractSocket::TemporaryError:
        std::cout << "A temporary error occurred (e.g., operation would block and socket is non-blocking)." << std::endl;
        break;
    case QAbstractSocket::UnknownSocketError:
        std::cout << "An unidentified error occurred." << std::endl;
        break;
    default:
        std::cout << "Unknown error occurred." << std::endl;
        break;
    }
    connectionSuccess = false;
    return false;
}