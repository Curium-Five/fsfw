#ifndef FRAMEWORK_TMTCSERVICES_TMTCBRIDGE_H_
#define FRAMEWORK_TMTCSERVICES_TMTCBRIDGE_H_

#include <framework/tmtcservices/AcceptsTelemetryIF.h>
#include <framework/tasks/ExecutableObjectIF.h>
#include <framework/ipc/MessageQueueIF.h>
#include <framework/storagemanager/StorageManagerIF.h>
#include <framework/objectmanager/SystemObject.h>

#include <framework/tmtcservices/TmTcMessage.h>
#include <framework/container/FIFO.h>

class TmTcBridge : public AcceptsTelemetryIF,
		public ExecutableObjectIF,
		public HasReturnvaluesIF,
		public SystemObject {
public:
	static constexpr uint8_t TMTC_RECEPTION_QUEUE_DEPTH = 20;
	static constexpr uint8_t MAX_STORED_DATA_SENT_PER_CYCLE = 10;
	static constexpr uint8_t MAX_DOWNLINK_PACKETS_STORED = 15;

	TmTcBridge(object_id_t objectId_, object_id_t ccsdsPacketDistributor_);
	virtual ~TmTcBridge();

	/**
	 * Set number of packets sent per performOperation().Please note that this
	 * value must be smaller than MAX_STORED_DATA_SENT_PER_CYCLE
	 * @param sentPacketsPerCycle
	 * @return -@c RETURN_OK if value was set successfully
	 * 		   -@c RETURN_FAILED otherwise, stored value stays the same
	 */
	ReturnValue_t setNumberOfSentPacketsPerCycle(uint8_t sentPacketsPerCycle);

	void registerCommConnect();
	void registerCommDisconnect();

	/**
	 * Initializes necessary FSFW components for the TMTC Bridge
	 * @return
	 */
	ReturnValue_t initialize() override;

	/**
	 * @brief	Handles TMTC reception
	 */
	ReturnValue_t performOperation(uint8_t operationCode = 0) override;

	/**
	 * Return TMTC Reception Queue
	 * @param virtualChannel
	 * @return
	 */
	MessageQueueId_t getReportReceptionQueue(uint8_t virtualChannel = 0) override;
protected:
	//! Used to send and receive TMTC messages.
	//! TmTcMessage is used to transport messages between tasks.
	MessageQueueIF* TmTcReceptionQueue = nullptr;
	StorageManagerIF* tcStore = nullptr;
	StorageManagerIF* tmStore = nullptr;
	object_id_t ccsdsPacketDistributor = 0;
	//! Used to specify whether communication link is up
	bool communicationLinkUp = false;
	bool tmStored = false;

	/**
	 * @brief 	Handle TC reception
	 * @details
	 * Default implementation provided, but is empty.
	 * Child handler should override this in most cases orsend TC to the
	 * TC distributor directly with the address of the reception queue by
	 * calling getReportRecptionQueue()
	 * @return
	 */
	virtual ReturnValue_t handleTc();

	/**
	 * Implemented by child class. Perform receiving of Telecommand,
	 * for example by implementing specific drivers or wrappers,
	 * e.g. UART Communication or an ethernet stack
	 * @param recvBuffer [out] Received data
	 * @param size [out] Size of received data
	 * @return
	 */
	virtual ReturnValue_t receiveTc(uint8_t ** recvBuffer, size_t * size) = 0;

	/**
	 * Handle Telemetry. Default implementation provided.
	 * Calls sendTm()
	 * @return
	 */
	virtual ReturnValue_t handleTm();

	/**
	 * Read the TM Queue and send TM if necessary. Default implementation provided
	 * @return
	 */
	virtual ReturnValue_t readTmQueue();

	/**
	 * Implemented by child class. Perform sending of Telemetry by implementing
	 * communication drivers or wrappers, e.g. UART communication or lwIP stack.
	 * @param data
	 * @param dataLen
	 * @return
	 */
	virtual ReturnValue_t sendTm(const uint8_t * data, size_t dataLen) = 0;

	/**
	 * Store data to be sent later if communication link is not up.
	 * @param message
	 * @return
	 */
	virtual ReturnValue_t storeDownlinkData(TmTcMessage * message);

	/**
	 * Send stored data if communication link is active
	 * @return
	 */
	ReturnValue_t sendStoredTm();

	/**
	 * Print data as hexidecimal array
	 * @param data
	 * @param dataLen
	 */
	void printData(uint8_t * data, size_t dataLen);

private:
	FIFO<store_address_t, MAX_DOWNLINK_PACKETS_STORED> fifo;
	uint8_t sentPacketsPerCycle = 10;
};


#endif /* FRAMEWORK_TMTCSERVICES_TMTCBRIDGE_H_ */
