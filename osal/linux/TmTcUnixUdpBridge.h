#ifndef FRAMEWORK_OSAL_LINUX_TMTCUNIXUDPBRIDGE_H_
#define FRAMEWORK_OSAL_LINUX_TMTCUNIXUDPBRIDGE_H_

#include <framework/tmtcservices/TmTcBridge.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>

class TmTcUnixUdpBridge: public TmTcBridge {
public:
	static constexpr int DEFAULT_UDP_SERVER_PORT = 7;
	static constexpr int DEFAULT_UDP_CLIENT_PORT = 2008;

	TmTcUnixUdpBridge(object_id_t objectId, object_id_t ccsdsPacketDistributor,
			 uint16_t serverPort = 0xFFFF,uint16_t clientPort = 0xFFFF);
	virtual~ TmTcUnixUdpBridge();

protected:
	virtual ReturnValue_t handleTc() override;
	virtual ReturnValue_t receiveTc(uint8_t ** recvBuffer,
			size_t * size) override;
	virtual ReturnValue_t sendTm(const uint8_t * data, size_t dataLen) override;
private:
	int serverSocket = 0;
	const int serverSocketOptions = 0;
	struct sockaddr_in clientAddress;
	struct sockaddr_in serverAddress;
};



#endif /* FRAMEWORK_OSAL_LINUX_TMTCUNIXUDPBRIDGE_H_ */
