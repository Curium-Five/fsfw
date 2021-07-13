#ifndef FSFW_INTERNALERROR_INTERNALERRORREPORTER_H_
#define FSFW_INTERNALERROR_INTERNALERRORREPORTER_H_

#include "InternalErrorReporterIF.h"

#include "fsfw/tasks/PeriodicTaskIF.h"
#include "fsfw/internalerror/InternalErrorDataset.h"
#include "fsfw/datapoollocal/LocalDataPoolManager.h"
#include "fsfw/tasks/ExecutableObjectIF.h"
#include "fsfw/objectmanager/SystemObject.h"
#include "fsfw/ipc/MutexIF.h"

/**
 * @brief   This class is used to track internal errors like lost telemetry,
 *          failed message sending or a full store.
 * @details
 * All functions were kept virtual so this class can be extended easily
 * to store custom internal errors (e.g. communication interface errors).
 */
class InternalErrorReporter:
        public SystemObject,
        public ExecutableObjectIF,
        public InternalErrorReporterIF,
        public HasLocalDataPoolIF {
public:

    InternalErrorReporter(object_id_t setObjectId,
            uint32_t messageQueueDepth = 5);

    /**
     * Enable diagnostic printout. Please note that this feature will
     * only work if DEBUG has been supplied to the build defines.
     * @param enable
     */
    void setDiagnosticPrintout(bool enable);

    void setMutexTimeout(MutexIF::TimeoutType timeoutType,
            uint32_t timeoutMs);

    virtual ~InternalErrorReporter();

    virtual object_id_t getObjectId() const override;
    virtual MessageQueueId_t getCommandQueue() const override;
    virtual ReturnValue_t initializeLocalDataPool(
            localpool::DataPool& localDataPoolMap,
            LocalDataPoolManager& poolManager) override;
    virtual dur_millis_t getPeriodicOperationFrequency() const override;
    virtual LocalPoolDataSetBase* getDataSetHandle(sid_t sid) override;
    LocalDataPoolManager* getHkManagerHandle() override;

    virtual ReturnValue_t initialize() override;
    virtual ReturnValue_t initializeAfterTaskCreation() override;
    virtual ReturnValue_t performOperation(uint8_t opCode) override;

    virtual void queueMessageNotSent();

    virtual void lostTm();

    virtual void storeFull();

    virtual void setTaskIF(PeriodicTaskIF* task) override;
protected:
    MessageQueueIF* commandQueue;
    LocalDataPoolManager poolManager;

    PeriodicTaskIF* executingTask = nullptr;

    MutexIF* mutex = nullptr;
    MutexIF::TimeoutType timeoutType = MutexIF::TimeoutType::WAITING;
    uint32_t timeoutMs = 20;

    sid_t internalErrorSid;
    InternalErrorDataset internalErrorDataset;

    bool diagnosticPrintout = true;

    uint32_t queueHits = 0;
    uint32_t tmHits = 0;
    uint32_t storeHits = 0;

    uint32_t getAndResetQueueHits();
    uint32_t getQueueHits();
    void incrementQueueHits();

    uint32_t getAndResetTmHits();
    uint32_t getTmHits();
    void incrementTmHits();

    uint32_t getAndResetStoreHits();
    uint32_t getStoreHits();
    void incrementStoreHits();

};

#endif /* FSFW_INTERNALERROR_INTERNALERRORREPORTER_H_ */
