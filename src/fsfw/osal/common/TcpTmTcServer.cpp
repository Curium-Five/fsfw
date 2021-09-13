#include "fsfw/osal/common/TcpTmTcServer.h"
#include "fsfw/osal/common/TcpTmTcBridge.h"
#include "fsfw/osal/common/tcpipHelpers.h"

#include "fsfw/platform.h"
#include "fsfw/container/SharedRingBuffer.h"
#include "fsfw/ipc/MessageQueueSenderIF.h"
#include "fsfw/ipc/MutexGuard.h"
#include "fsfw/objectmanager/ObjectManager.h"

#include "fsfw/serviceinterface/ServiceInterface.h"
#include "fsfw/tmtcservices/TmTcMessage.h"

#ifdef PLATFORM_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(PLATFORM_UNIX)
#include <netdb.h>
#endif

#ifndef FSFW_TCP_RECV_WIRETAPPING_ENABLED
#define FSFW_TCP_RECV_WIRETAPPING_ENABLED 0
#endif

const std::string TcpTmTcServer::DEFAULT_SERVER_PORT = tcpip::DEFAULT_SERVER_PORT;

TcpTmTcServer::TcpTmTcServer(object_id_t objectId, object_id_t tmtcTcpBridge,
        size_t receptionBufferSize, std::string customTcpServerPort):
        SystemObject(objectId), tmtcBridgeId(tmtcTcpBridge),
        tcpPort(customTcpServerPort), receptionBuffer(receptionBufferSize) {
    if(tcpPort == "") {
        tcpPort = DEFAULT_SERVER_PORT;
    }
}

ReturnValue_t TcpTmTcServer::initialize() {
    using namespace tcpip;

    ReturnValue_t result = TcpIpBase::initialize();
    if(result != HasReturnvaluesIF::RETURN_OK) {
        return result;
    }

    tcStore = ObjectManager::instance()->get<StorageManagerIF>(objects::TC_STORE);
    if (tcStore == nullptr) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "TcpTmTcServer::initialize: TC store uninitialized!" << std::endl;
#else
        sif::printError("TcpTmTcServer::initialize: TC store uninitialized!\n");
#endif
        return ObjectManagerIF::CHILD_INIT_FAILED;
    }

    tmtcBridge = ObjectManager::instance()->get<TcpTmTcBridge>(tmtcBridgeId);

    int retval = 0;
    struct addrinfo *addrResult = nullptr;
    struct addrinfo hints = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Listen to all addresses (0.0.0.0) by using AI_PASSIVE in the hint flags
    retval = getaddrinfo(nullptr, tcpPort.c_str(), &hints, &addrResult);
    if (retval != 0) {
        handleError(Protocol::TCP, ErrorSources::GETADDRINFO_CALL);
        return HasReturnvaluesIF::RETURN_FAILED;
    }

    // Open TCP (stream) socket
    listenerTcpSocket = socket(addrResult->ai_family, addrResult->ai_socktype,
            addrResult->ai_protocol);
    if(listenerTcpSocket == INVALID_SOCKET) {
        freeaddrinfo(addrResult);
        handleError(Protocol::TCP, ErrorSources::SOCKET_CALL);
        return HasReturnvaluesIF::RETURN_FAILED;
    }

    // Bind to the address found by getaddrinfo
    retval = bind(listenerTcpSocket, addrResult->ai_addr, static_cast<int>(addrResult->ai_addrlen));
    if(retval == SOCKET_ERROR) {
        freeaddrinfo(addrResult);
        handleError(Protocol::TCP, ErrorSources::BIND_CALL);
        return HasReturnvaluesIF::RETURN_FAILED;
    }

    freeaddrinfo(addrResult);
    return HasReturnvaluesIF::RETURN_OK;
}


TcpTmTcServer::~TcpTmTcServer() {
    closeSocket(listenerTcpSocket);
}

ReturnValue_t TcpTmTcServer::performOperation(uint8_t opCode) {
    using namespace tcpip;
    // If a connection is accepted, the corresponding socket will be assigned to the new socket
    socket_t connSocket = 0;
    // sockaddr clientSockAddr = {};
    // socklen_t connectorSockAddrLen = 0;
    int retval = 0;

    // Listen for connection requests permanently for lifetime of program
    while(true) {
        retval = listen(listenerTcpSocket, tcpBacklog);
        if(retval == SOCKET_ERROR) {
            handleError(Protocol::TCP, ErrorSources::LISTEN_CALL, 500);
            continue;
        }

        //connSocket = accept(listenerTcpSocket, &clientSockAddr, &connectorSockAddrLen);
        connSocket = accept(listenerTcpSocket, nullptr, nullptr);

        if(connSocket == INVALID_SOCKET) {
            handleError(Protocol::TCP, ErrorSources::ACCEPT_CALL, 500);
            closeSocket(connSocket);
            continue;
        };

        handleServerOperation(connSocket);

        // Done, shut down connection and go back to listening for client requests
        retval = shutdown(connSocket, SHUT_SEND);
        if(retval != 0) {
            handleError(Protocol::TCP, ErrorSources::SHUTDOWN_CALL);
        }
        closeSocket(connSocket);
    }
    return HasReturnvaluesIF::RETURN_OK;
}

ReturnValue_t TcpTmTcServer::initializeAfterTaskCreation() {
    if(tmtcBridge == nullptr) {
        return ObjectManagerIF::CHILD_INIT_FAILED;
    }
    /* Initialize the destination after task creation. This ensures
    that the destination has already been set in the TMTC bridge. */
    targetTcDestination = tmtcBridge->getRequestQueue();
    tcStore = tmtcBridge->tcStore;
    tmStore = tmtcBridge->tmStore;
    return HasReturnvaluesIF::RETURN_OK;
}

void TcpTmTcServer::handleServerOperation(socket_t connSocket) {
    int retval = 0;
    do {
        // Read all telecommands sent by the client
        retval = recv(connSocket,
                reinterpret_cast<char*>(receptionBuffer.data()),
                receptionBuffer.capacity(),
                tcpFlags);
        if (retval > 0) {
            handleTcReception(retval);
        }
        else if(retval == 0) {
            // Client has finished sending telecommands, send telemetry now
            handleTmSending(connSocket);
        }
        else {
            // Should not happen
            tcpip::handleError(tcpip::Protocol::TCP, tcpip::ErrorSources::RECV_CALL);
        }
    } while(retval > 0);
}

ReturnValue_t TcpTmTcServer::handleTcReception(size_t bytesRecvd) {
#if FSFW_TCP_RECV_WIRETAPPING_ENABLED == 1
    arrayprinter::print(receptionBuffer.data(), bytesRead);
#endif
    store_address_t storeId;
    ReturnValue_t result = tcStore->addData(&storeId, receptionBuffer.data(), bytesRecvd);
    if (result != HasReturnvaluesIF::RETURN_OK) {
#if FSFW_VERBOSE_LEVEL >= 1
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::warning<< "TcpTmTcServer::handleServerOperation: Data storage failed." << std::endl;
        sif::warning << "Packet size: " << bytesRecvd << std::endl;
#endif /* FSFW_CPP_OSTREAM_ENABLED == 1 */
#endif /* FSFW_VERBOSE_LEVEL >= 1 */
    }

    TmTcMessage message(storeId);

    result  = MessageQueueSenderIF::sendMessage(targetTcDestination, &message);
    if (result != HasReturnvaluesIF::RETURN_OK) {
#if FSFW_VERBOSE_LEVEL >= 1
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::warning << "UdpTcPollingTask::handleSuccessfullTcRead: "
                " Sending message to queue failed" << std::endl;
#endif /* FSFW_CPP_OSTREAM_ENABLED == 1 */
#endif /* FSFW_VERBOSE_LEVEL >= 1 */
        tcStore->deleteData(storeId);
    }
    return result;
}

void TcpTmTcServer::setTcpBacklog(uint8_t tcpBacklog) {
    this->tcpBacklog = tcpBacklog;
}

std::string TcpTmTcServer::getTcpPort() const {
    return tcpPort;
}

ReturnValue_t TcpTmTcServer::handleTmSending(socket_t connSocket) {
    // Access to the FIFO is mutex protected because it is filled by the bridge
    MutexGuard(tmtcBridge->mutex, tmtcBridge->timeoutType, tmtcBridge->mutexTimeoutMs);
    store_address_t storeId;
    while((not tmtcBridge->tmFifo->empty()) and
            (tmtcBridge->packetSentCounter < tmtcBridge->sentPacketsPerCycle)) {
        tmtcBridge->tmFifo->retrieve(&storeId);

        // Using the store accessor will take care of deleting TM from the store automatically
        ConstStorageAccessor storeAccessor(storeId);
        ReturnValue_t result = tmStore->getData(storeId, storeAccessor);
        if(result != HasReturnvaluesIF::RETURN_OK) {
            return result;
        }
        int retval = send(connSocket,
                reinterpret_cast<const char*>(storeAccessor.data()),
                storeAccessor.size(),
                tcpTmFlags);
        if(retval != static_cast<int>(storeAccessor.size())) {
            tcpip::handleError(tcpip::Protocol::TCP, tcpip::ErrorSources::SEND_CALL);
        }
    }
    return HasReturnvaluesIF::RETURN_OK;
}
