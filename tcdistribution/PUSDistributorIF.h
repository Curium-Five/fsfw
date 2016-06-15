/*
 * PUSDistributorIF.h
 *
 *  Created on: 21.11.2012
 *      Author: baetz
 */

#ifndef PUSDISTRIBUTORIF_H_
#define PUSDISTRIBUTORIF_H_

#include <framework/ipc/MessageQueue.h>
#include <framework/tmtcservices/AcceptsTelecommandsIF.h>
/**
 * This interface allows PUS Services to register themselves at a PUS Distributor.
 * \ingroup tc_distribution
 */
class PUSDistributorIF {
public:
	/**
	 * The empty virtual destructor.
	 */
	virtual ~PUSDistributorIF() {
	}
/**
 * With this method, Services can register themselves at the PUS Distributor.
 * @param service A pointer to the registering Service.
 * @return	- \c RETURN_OK on success,
 * 			- \c RETURN_FAILED on failure.
 */
	virtual ReturnValue_t registerService( AcceptsTelecommandsIF* service ) = 0;
};

#endif /* PUSDISTRIBUTORIF_H_ */
