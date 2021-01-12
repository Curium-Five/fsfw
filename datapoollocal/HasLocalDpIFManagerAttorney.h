#ifndef FSFW_DATAPOOLLOCAL_HASLOCALDPIFMANAGERATTORNEY_H_
#define FSFW_DATAPOOLLOCAL_HASLOCALDPIFMANAGERATTORNEY_H_

#include "localPoolDefinitions.h"

class HasLocalDataPoolIF;
class LocalPoolDataSetBase;
class LocalPoolObjectBase;

class HasLocalDpIFManagerAttorney {

	static LocalPoolDataSetBase* getDataSetHandle(HasLocalDataPoolIF* interface, sid_t sid);

	static LocalPoolObjectBase* getPoolObjectHandle(HasLocalDataPoolIF* interface,
			lp_id_t localPoolId);

	static object_id_t getObjectId(HasLocalDataPoolIF* interface);

	friend class LocalDataPoolManager;
};

#endif /* FSFW_DATAPOOLLOCAL_HASLOCALDPIFMANAGERATTORNEY_H_ */
