#include "../common/tcpipHelpers.h"
#include <FSFWConfig.h>

#include "../../tasks/TaskFactory.h"
#include "../../serviceinterface/ServiceInterface.h"


#include <winsock2.h>
#include <string>

void tcpip::handleError(Protocol protocol, ErrorSources errorSrc, dur_millis_t sleepDuration) {
#if FSFW_VERBOSE_LEVEL >= 1
    int errCode = WSAGetLastError();
    std::string protocolString;
    std::string errorSrcString;
    determineErrorStrings(protocol, errorSrc, protocolString, errorSrcString);

    std::string infoString;
    switch(errCode) {
    case(WSANOTINITIALISED): {
        infoString = "WSANOTINITIALISED";
        break;
    }
    case(WSAEADDRINUSE): {
        infoString = "WSAEADDRINUSE";
        break;
    }
    case(WSAEFAULT): {
        infoString = "WSAEFAULT";
        break;
    }
    case(WSAEADDRNOTAVAIL): {
        infoString = "WSAEADDRNOTAVAIL";
        break;
    }
    case(WSAEINVAL): {
        infoString = "WSAEINVAL";
        break;
    }
    default: {
        /*
        https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
         */
        infoString = "Error code: " + std::to_string(errCode);
        break;
    }
    }

#if FSFW_CPP_OSTREAM_ENABLED == 1
    sif::warning << "tcpip::handleError: " << protocolString << " | " << errorSrcString <<
            " | " << infoString << std::endl;
#else
    sif::printWarning("tcpip::handleError: %s | %s | %s\n", protocolString,
            errorSrcString, infoString);
#endif /* FSFW_CPP_OSTREAM_ENABLED == 1 */

#endif /* FSFW_VERBOSE_LEVEL >= 1 */

    if(sleepDuration > 0) {
        TaskFactory::instance()->delayTask(sleepDuration);
    }
}

