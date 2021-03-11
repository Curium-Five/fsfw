#include "TmTcWinUdpBridge.h"

#include <fsfw/serviceinterface/ServiceInterface.h>
#include <fsfw/ipc/MutexGuard.h>
#include <ws2tcpip.h>

const std::string TmTcWinUdpBridge::DEFAULT_UDP_SERVER_PORT =  "7301";
const std::string TmTcWinUdpBridge::DEFAULT_UDP_CLIENT_PORT =  "7302";

TmTcWinUdpBridge::TmTcWinUdpBridge(object_id_t objectId,
        object_id_t tcDestination, object_id_t tmStoreId, object_id_t tcStoreId,
        std::string udpServerPort, std::string udpClientPort):
        TmTcBridge(objectId, tcDestination, tmStoreId, tcStoreId) {
    if(udpServerPort == "") {
        udpServerPort = DEFAULT_UDP_SERVER_PORT;
    }
    else {
        this->udpServerPort = udpServerPort;
    }
    if(udpClientPort == "") {
        udpClientPort = DEFAULT_UDP_CLIENT_PORT;
    }
    else {
        this->udpClientPort = udpClientPort;
    }

    mutex = MutexFactory::instance()->createMutex();
    communicationLinkUp = false;
}

ReturnValue_t TmTcWinUdpBridge::initialize() {
    ReturnValue_t result = TmTcBridge::initialize();
    if(result != HasReturnvaluesIF::RETURN_OK) {
        return result;
    }

    /* Initiates Winsock DLL. */
    WSAData wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "TmTcWinUdpBridge::TmTcWinUdpBridge: WSAStartup failed with error: " <<
                err << std::endl;
#else
        sif::printError("TmTcWinUdpBridge::TmTcWinUdpBridge: WSAStartup failed with error: %d\n",
                err);
#endif
        return HasReturnvaluesIF::RETURN_FAILED;
    }

    struct addrinfo *addrResult = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    /* Set up UDP socket:
    https://en.wikipedia.org/wiki/Getaddrinfo
    Passing nullptr as the first parameter and specifying AI_PASSIVE in hints will cause
    getaddrinfo to assign the address 0.0.0.0 (any address)
    */
    int retval = getaddrinfo(nullptr, udpServerPort.c_str(), &hints, &addrResult);
    if (retval != 0) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::warning << "TmTcWinUdpBridge::TmTcWinUdpBridge: Retrieving address info failed!" <<
                std::endl;
#endif
        return HasReturnvaluesIF::RETURN_FAILED;
    }

    serverSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if(serverSocket == INVALID_SOCKET) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::warning << "TmTcWinUdpBridge::TmTcWinUdpBridge: Could not open UDP socket!" <<
                std::endl;
#endif
        handleSocketError();
        return HasReturnvaluesIF::RETURN_FAILED;
    }

//    serverAddress.sin_family = AF_INET;
//
//    /* Accept packets from any interface. (potentially insecure). */
//    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
//    serverAddress.sin_port = htons(setServerPort);
//    serverAddressLen = sizeof(serverAddress);
//    int result = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
//            reinterpret_cast<const char*>(&serverSocketOptions),
//            sizeof(serverSocketOptions));
//    if(result != 0) {
//#if FSFW_CPP_OSTREAM_ENABLED == 1
//        sif::warning << "TmTcWinUdpBridge::TmTcWinUdpBridge: Could not set socket options!" <<
//                std::endl;
//#endif
//        handleSocketError();
//    }
//
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddress.sin_port = htons(7302);
    clientAddressLen = sizeof(clientAddress);

    retval = bind(serverSocket, addrResult->ai_addr, static_cast<int>(addrResult->ai_addrlen));
    if(retval != 0) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "TmTcWinUdpBridge::TmTcWinUdpBridge: Could not bind "
                "local port " << udpServerPort << " to server socket!" << std::endl;
#endif
        handleBindError();
    }
    return HasReturnvaluesIF::RETURN_OK;
}

TmTcWinUdpBridge::~TmTcWinUdpBridge() {
    if(mutex != nullptr) {
        MutexFactory::instance()->deleteMutex(mutex);
    }
    closesocket(serverSocket);
    WSACleanup();
}

ReturnValue_t TmTcWinUdpBridge::sendTm(const uint8_t *data, size_t dataLen) {
    int flags = 0;

#if FSFW_CPP_OSTREAM_ENABLED == 1 && FSFW_UDP_SEND_WIRETAPPING_ENABLED == 1
    clientAddress.sin_addr.s_addr = htons(INADDR_ANY);
    clientAddressLen = sizeof(serverAddress);
    char ipAddress [15];
    sif::debug << "IP Address Sender: "<< inet_ntop(AF_INET,
            &clientAddress.sin_addr.s_addr, ipAddress, 15) << std::endl;
#endif

    int bytesSent = sendto(serverSocket,
            reinterpret_cast<const char*>(data), dataLen, flags,
            reinterpret_cast<sockaddr*>(&clientAddress), clientAddressLen);
    if(bytesSent == SOCKET_ERROR) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "TmTcWinUdpBridge::sendTm: Send operation failed."
                << std::endl;
#endif
        handleSendError();
    }
#if FSFW_CPP_OSTREAM_ENABLED == 1 && FSFW_UDP_SEND_WIRETAPPING_ENABLED == 1
    sif::debug << "TmTcUnixUdpBridge::sendTm: " << bytesSent << " bytes were"
            " sent." << std::endl;
#endif
    return HasReturnvaluesIF::RETURN_OK;
}

void TmTcWinUdpBridge::checkAndSetClientAddress(sockaddr_in newAddress) {
    MutexGuard lock(mutex, MutexIF::TimeoutType::WAITING, 10);

#if FSFW_CPP_OSTREAM_ENABLED == 1 && FSFW_UDP_SEND_WIRETAPPING_ENABLED == 1
    char ipAddress [15];
    sif::debug << "IP Address Sender: "<< inet_ntop(AF_INET,
            &newAddress.sin_addr.s_addr, ipAddress, 15) << std::endl;
    sif::debug << "IP Address Old: " <<  inet_ntop(AF_INET,
            &clientAddress.sin_addr.s_addr, ipAddress, 15) << std::endl;
#endif
    registerCommConnect();

    /* Set new IP address if it has changed. */
    if(clientAddress.sin_addr.s_addr != newAddress.sin_addr.s_addr) {
        clientAddress.sin_addr.s_addr = newAddress.sin_addr.s_addr;
        clientAddressLen = sizeof(clientAddress);
    }
}

void TmTcWinUdpBridge::handleSocketError() {
    int errCode = WSAGetLastError();
    switch(errCode) {
    case(WSANOTINITIALISED): {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::warning << "TmTcWinUdpBridge::handleSocketError: WSANOTINITIALISED: WSAStartup"
                " call necessary" << std::endl;
#endif
        break;
    }
    default: {
        /*
        https://docs.microsoft.com/en-us/windows/win32/winsock/
        windows-sockets-error-codes-2
         */
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::warning << "TmTcWinUdpBridge::handleSocketError: Error code: " << errCode << std::endl;
#endif
        break;
    }
    }
}

void TmTcWinUdpBridge::handleBindError() {
    int errCode = WSAGetLastError();
    switch(errCode) {
    case(WSANOTINITIALISED): {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::info << "TmTcWinUdpBridge::handleBindError: WSANOTINITIALISED: "
                << "WSAStartup call necessary" << std::endl;
#endif
        break;
    }
    case(WSAEADDRINUSE): {
#if FSFW_CPP_OSTREAM_ENABLED == 1
    	sif::warning << "TmTcWinUdpBridge::handleBindError: WSAEADDRINUSE: "
    	        "Port is already in use!" << std::endl;
#endif
    	break;
    }
    default: {
        /*
        https://docs.microsoft.com/en-us/windows/win32/winsock/
        windows-sockets-error-codes-2
        */
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::info << "TmTcWinUdpBridge::handleBindError: Error code: "
                << errCode << std::endl;
#endif
        break;
    }
    }
}

void TmTcWinUdpBridge::handleSendError() {
    int errCode = WSAGetLastError();
    switch(errCode) {
    case(WSANOTINITIALISED): {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::info << "TmTcWinUdpBridge::handleSendError: WSANOTINITIALISED: "
                << "WSAStartup(...) call necessary" << std::endl;
#endif
        break;
    }
    case(WSAEADDRNOTAVAIL): {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::info << "TmTcWinUdpBridge::handleSendError: WSAEADDRNOTAVAIL: "
                << "Check target address. " << std::endl;
#endif
        break;
    }
    default: {
        /*
        https://docs.microsoft.com/en-us/windows/win32/winsock/
        windows-sockets-error-codes-2
        */
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::info << "TmTcWinUdpBridge::handleSendError: Error code: "
                << errCode << std::endl;
#endif
        break;
    }
    }
}

