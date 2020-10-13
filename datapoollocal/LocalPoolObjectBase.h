#ifndef FSFW_DATAPOOLLOCAL_LOCALPOOLOBJECTBASE_H_
#define FSFW_DATAPOOLLOCAL_LOCALPOOLOBJECTBASE_H_

#include "../datapoollocal/LocalDataPoolManager.h"
#include "../datapool/PoolVariableIF.h"

class LocalPoolObjectBase: public PoolVariableIF, HasReturnvaluesIF {
public:
    LocalPoolObjectBase(lp_id_t poolId,
            HasLocalDataPoolIF* hkOwner, DataSetIF* dataSet,
            pool_rwm_t setReadWriteMode);

    LocalPoolObjectBase(lp_id_t poolId, object_id_t poolOwner,
            DataSetIF* dataSet = nullptr,
            pool_rwm_t setReadWriteMode = pool_rwm_t::VAR_READ_WRITE);

    pool_rwm_t getReadWriteMode() const;

    bool isValid() const override;
    void setValid(bool valid) override;

    void setChanged(bool changed);
    bool hasChanged() const;

    lp_id_t getDataPoolId() const override;
    void setDataPoolId(lp_id_t poolId);

protected:
    /**
     * @brief   To access the correct data pool entry on read and commit calls,
     *          the data pool id is stored.
     */
    uint32_t localPoolId = PoolVariableIF::NO_PARAMETER;
    /**
     * @brief   The valid information as it was stored in the data pool
     *          is copied to this attribute.
     */
    bool valid = false;

    /**
     * @brief   A local pool variable can be marked as changed.
     */
    bool changed = false;

    /**
     * @brief   The information whether the class is read-write or
     *          read-only is stored here.
     */
    ReadWriteMode_t readWriteMode = pool_rwm_t::VAR_READ_WRITE;

    //! @brief  Pointer to the class which manages the HK pool.
    LocalDataPoolManager* hkManager;

};



#endif /* FSFW_DATAPOOLLOCAL_LOCALPOOLOBJECTBASE_H_ */
