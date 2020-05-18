#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <framework/internalError/InternalErrorReporterIF.h>
#include <framework/ipc/MessageQueueIF.h>
#include <framework/ipc/MessageQueueMessage.h>
#include <framework/osal/FreeRTOS/TaskManagement.h>

extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
}



//TODO this class assumes that MessageQueueId_t is the same size as void* (the FreeRTOS handle type), compiler will catch this but it might be nice to have something checking or even an always working solution
// https://scaryreasoner.wordpress.com/2009/02/28/checking-sizeof-at-compile-time/

/**
 *	@brief		This class manages sending and receiving of message queue messages.
 *
 *	@details	Message queues are used to pass asynchronous messages between processes.
 *				They work like post boxes, where all incoming messages are stored in FIFO
 *				order. This class creates a new receiving queue and provides methods to fetch
 *				received messages. Being a child of MessageQueueSender, this class also provides
 *				methods to send a message to a user-defined or a default destination. In addition
 *				it also provides a reply method to answer to the queue it received its last message
 *				from.
 *
 *				The MessageQueue should be used as "post box" for a single owning object. So all
 *				message queue communication is "n-to-one".
 *				For creating the queue, as well as sending and receiving messages, the class makes
 *				use of the operating system calls provided.
 *
 *				Please keep in mind that FreeRTOS offers
 *				different calls for message queue operations if called from an ISR.
 *				For now, the system context needs to be switched manually.
 *	@ingroup osal
 *	@ingroup message_queue
 */
class MessageQueue : public MessageQueueIF {
	friend class MessageQueueSenderIF;
public:
	/**
	 * @brief	The constructor initializes and configures the message queue.
	 * @details	By making use of the according operating system call, a message queue is created
	 * 			and initialized. The message depth - the maximum number of messages to be
	 * 			buffered - may be set with the help of a parameter, whereas the message size is
	 * 			automatically set to the maximum message queue message size. The operating system
	 * 			sets the message queue id, or i case of failure, it is set to zero.
	 * @param message_depth	The number of messages to be buffered before passing an error to the
	 * 						sender. Default is three.
	 * @param max_message_size	With this parameter, the maximum message size can be adjusted.
	 * 							This should be left default.
	 */
	MessageQueue( size_t message_depth = 3, size_t max_message_size = MessageQueueMessage::MAX_MESSAGE_SIZE );

	/**
	 * @brief	The destructor deletes the formerly created message queue.
	 * @details	This is accomplished by using the delete call provided by the operating system.
	 */
	virtual ~MessageQueue();

	void switchSystemContext(CallContext callContext);

	/**
	 * @brief	This operation sends a message to the given destination.
	 * @details	It directly uses the sendMessage call of the MessageQueueSender parent, but passes its
	 * 			queue id as "sentFrom" parameter.
	 * @param sendTo	This parameter specifies the message queue id of the destination message queue.
	 * @param message	A pointer to a previously created message, which is sent.
	 * @param ignoreFault If set to true, the internal software fault counter is not incremented if queue is full.
	 */
	ReturnValue_t sendMessage(MessageQueueId_t sendTo,
			MessageQueueMessage* message, bool ignoreFault = false );
	/**
	 * @brief	This operation sends a message to the default destination.
	 * @details	As in the sendMessage method, this function uses the sendToDefault call of the
	 * 			MessageQueueSender parent class and adds its queue id as "sentFrom" information.
	 * @param message	A pointer to a previously created message, which is sent.
	 */
	ReturnValue_t sendToDefault( MessageQueueMessage* message );
	/**
	 * @brief	This operation sends a message to the last communication partner.
	 * @details	This operation simplifies answering an incoming message by using the stored
	 * 			lastParnter information as destination. If there was no message received yet
	 * 			(i.e. lastPartner is zero), an error code is returned.
	 * @param message	A pointer to a previously created message, which is sent.
	 */
	ReturnValue_t reply( MessageQueueMessage* message );

	/**
	 * \brief	With the sendMessage call, a queue message is sent to a receiving queue.
	 * \details	This method takes the message provided, adds the sentFrom information and passes
	 * 			it on to the destination provided with an operating system call. The OS's return
	 * 			value is returned.
	 * \param sendTo	This parameter specifies the message queue id to send the message to.
	 * \param message	This is a pointer to a previously created message, which is sent.
	 * \param sentFrom	The sentFrom information can be set to inject the sender's queue id into the message.
	 * 					This variable is set to zero by default.
	 * \param ignoreFault If set to true, the internal software fault counter is not incremented if queue is full.
	 */
	virtual ReturnValue_t sendMessageFrom( MessageQueueId_t sendTo, MessageQueueMessage* message,
			MessageQueueId_t sentFrom = NO_QUEUE, bool ignoreFault = false );

	/**
	 * \brief	The sendToDefault method sends a queue message to the default destination.
	 * \details	In all other aspects, it works identical to the sendMessage method.
	 * \param message	This is a pointer to a previously created message, which is sent.
	 * \param sentFrom	The sentFrom information can be set to inject the sender's queue id into the message.
	 * 					This variable is set to zero by default.
	 */
	virtual ReturnValue_t sendToDefaultFrom( MessageQueueMessage* message, MessageQueueId_t sentFrom = NO_QUEUE, bool ignoreFault = false );

	/**
	 * @brief	This function reads available messages from the message queue and returns the sender.
	 * @details	It works identically to the other receiveMessage call, but in addition returns the
	 * 			sender's queue id.
	 * @param message	A pointer to a message in which the received data is stored.
	 * @param receivedFrom	A pointer to a queue id in which the sender's id is stored.
	 */
	ReturnValue_t receiveMessage(MessageQueueMessage* message,
			MessageQueueId_t *receivedFrom);

	/**
	 * @brief	This function reads available messages from the message queue.
	 * @details	If data is available it is stored in the passed message pointer. The message's
	 * 			original content is overwritten and the sendFrom information is stored in the
	 * 			lastPartner attribute. Else, the lastPartner information remains untouched, the
	 * 			message's content is cleared and the function returns immediately.
	 * @param message	A pointer to a message in which the received data is stored.
	 */
	ReturnValue_t receiveMessage(MessageQueueMessage* message);
	/**
	 * Deletes all pending messages in the queue.
	 * @param count The number of flushed messages.
	 * @return RETURN_OK on success.
	 */
	ReturnValue_t flush(uint32_t* count);
	/**
	 * @brief	This method returns the message queue id of the last communication partner.
	 */
	MessageQueueId_t getLastPartner() const;
	/**
	 * @brief	This method returns the message queue id of this class's message queue.
	 */
	MessageQueueId_t getId() const;

	/**
	 * \brief	This method is a simple setter for the default destination.
	 */
	void setDefaultDestination(MessageQueueId_t defaultDestination);
	/**
	 * \brief	This method is a simple getter for the default destination.
	 */
	MessageQueueId_t getDefaultDestination() const;

	bool isDefaultDestinationSet() const;
protected:
	/**
	 * Implementation to be called from any send Call within MessageQueue and MessageQueueSenderIF
	 * 	 \details	This method takes the message provided, adds the sentFrom information and passes
	 * 			it on to the destination provided with an operating system call. The OS's return
	 * 			value is returned.
	 * \param sendTo	This parameter specifies the message queue id to send the message to.
	 * \param message	This is a pointer to a previously created message, which is sent.
	 * \param sentFrom	The sentFrom information can be set to inject the sender's queue id into the message.
	 * 					This variable is set to zero by default.
	 * \param ignoreFault If set to true, the internal software fault counter is not incremented if queue is full.
	 * \param context
	 */
	static ReturnValue_t sendMessageFromMessageQueue(MessageQueueId_t sendTo,
			MessageQueueMessage* message, MessageQueueId_t sentFrom = NO_QUEUE,
			bool ignoreFault=false, CallContext callContext = CallContext::task);

	static ReturnValue_t handleSendResult(BaseType_t result, bool ignoreFault);


private:
	QueueHandle_t handle;
	MessageQueueId_t defaultDestination;
	MessageQueueId_t lastPartner;
	CallContext callContext; //!< Stores the current system context
};

#endif /* MESSAGEQUEUE_H_ */
