#ifndef SERIALFIXEDARRAYLISTADAPTER_H_
#define SERIALFIXEDARRAYLISTADAPTER_H_

#include <framework/container/FixedArrayList.h>
#include <framework/serialize/SerialArrayListAdapter.h>

/**
 * @brief This adapter provides an interface for SerializeIF to serialize and deserialize
 *        buffers with a header containing the buffer length.
 * @details
 *
 * Can be used by SerialLinkedListAdapter by declaring
 * as a linked element with SerializeElement<SerialFixedArrayListAdapter<...>>.
 * The sequence of objects is defined in the constructor by using the setStart and setNext functions.
 *
 *  1. Buffers with a size header inside that class can be declared with
 *     SerialFixedArrayListAdapter<BUFFER_TYPE, MAX_BUFFER_LENGTH, LENGTH_FIELD_TYPE>.
 *  2. MAX_BUFFER_LENGTH specifies the maximum allowed number of elements in FixedArrayList
 *  3. LENGTH_FIELD_TYPE specifies the data type of the buffer header containing the buffer size
 *     (defaults to 1 byte length field) that follows
 *
 * @ingroup serialize
 */
template<typename BUFFER_TYPE, uint32_t MAX_SIZE, typename count_t = uint8_t>
class SerialFixedArrayListAdapter : public FixedArrayList<BUFFER_TYPE, MAX_SIZE, count_t>, public SerializeIF {
public:
	template<typename... Args>
	SerialFixedArrayListAdapter(Args... args) : FixedArrayList<BUFFER_TYPE, MAX_SIZE, count_t>(std::forward<Args>(args)...) {
	}

	ReturnValue_t serialize(uint8_t** buffer, uint32_t* size,
			const uint32_t max_size, bool bigEndian) const {
		return SerialArrayListAdapter<BUFFER_TYPE, count_t>::serialize(this, buffer, size, max_size, bigEndian);
	}

	uint32_t getSerializedSize() const {
		return SerialArrayListAdapter<BUFFER_TYPE, count_t>::getSerializedSize(this);
	}
	ReturnValue_t deSerialize(const uint8_t** buffer, int32_t* size,
			bool bigEndian) {
		return SerialArrayListAdapter<BUFFER_TYPE, count_t>::deSerialize(this, buffer, size, bigEndian);
	}
};



#endif /* SERIALFIXEDARRAYLISTADAPTER_H_ */
