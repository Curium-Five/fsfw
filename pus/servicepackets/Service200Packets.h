#ifndef FRAMEWORK_PUS_SERVICEPACKETS_SERVICE200PACKETS_H_
#define FRAMEWORK_PUS_SERVICEPACKETS_SERVICE200PACKETS_H_

#include <framework/serialize/SerialLinkedListAdapter.h>
#include <framework/modes/ModeMessage.h>
#include <framework/serialize/SerializeIF.h>

/**
 * @brief Subservice 1, 2, 3, 4, 5
 * @ingroup spacepackets
 */
class ModePacket : public SerialLinkedListAdapter<SerializeIF> { //!< [EXPORT] : [SUBSERVICE] 1, 2, 6
public:
	ModePacket() {
		setLinks();
	}

	ModePacket(object_id_t objectId, Mode_t mode, Submode_t submode) :
	        objectId(objectId), mode(mode), submode(submode) {
		setLinks();
	}

	Mode_t getMode() {
		return mode.entry;
	}

	Submode_t getSubmode() {
		return submode.entry;
	}

private:
	// Forbid copying because of next pointer to member
	ModePacket(const ModePacket &command);
	void setLinks() {
		setStart(&objectId);
		objectId.setNext(&mode);
		mode.setNext(&submode);
	}
	SerializeElement<object_id_t> objectId; //!< [EXPORT] : [COMMENT] Target or source object
	SerializeElement<Mode_t> mode; //!< [EXPORT] : [COMMENT] 0: MODE_OFF, 1: MODE_ON, 2: MODE_NORMAL, 3: MODE_RAW
	SerializeElement<Submode_t> submode; //!< [EXPORT] : [COMMENT] Usually 0, device specific submode possible
};

class CantReachModePacket: public SerialLinkedListAdapter<SerializeIF> { //!< [EXPORT] : [SUBSERVICE] 7
public:
    CantReachModePacket(object_id_t objectId, ReturnValue_t reason):
            objectId(objectId), reason(reason) {
        setStart(&this->objectId);
        this->objectId.setNext(&this->reason);
    }

    SerializeElement<object_id_t> objectId; //!< [EXPORT] : [COMMENT] Reply source object
	SerializeElement<ReturnValue_t> reason; //!< [EXPORT] : [COMMENT] Reason the mode could not be reached
};

#endif /* FRAMEWORK_PUS_SERVICEPACKETS_SERVICE200PACKETS_H_ */
