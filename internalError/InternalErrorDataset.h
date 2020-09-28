#ifndef FSFW_INTERNALERROR_INTERNALERRORDATASET_H_
#define FSFW_INTERNALERROR_INTERNALERRORDATASET_H_

#include <fsfw/datapoollocal/StaticLocalDataSet.h>
#include <fsfw/datapoollocal/LocalPoolVariable.h>

enum errorPoolIds {
    TM_HITS,
    QUEUE_HITS,
    STORE_HITS
};


class InternalErrorDataset: public StaticLocalDataSet<3 * sizeof(uint32_t)> {
public:
    static constexpr uint8_t ERROR_SET_ID = 0;

    InternalErrorDataset(HasLocalDataPoolIF* owner):
            StaticLocalDataSet(owner, ERROR_SET_ID) {}

    InternalErrorDataset(sid_t sid):
           StaticLocalDataSet(sid) {}

    lp_var_t<uint32_t> tmHits = lp_var_t<uint32_t>(TM_HITS,
            hkManager->getOwner(), this);
    lp_var_t<uint32_t> queueHits = lp_var_t<uint32_t>(QUEUE_HITS,
            hkManager->getOwner(), this);
    lp_var_t<uint32_t> storeHits = lp_var_t<uint32_t>(STORE_HITS,
            hkManager->getOwner(), this);
};



#endif /* FSFW_INTERNALERROR_INTERNALERRORDATASET_H_ */
