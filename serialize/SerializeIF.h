#ifndef SERIALIZEIF_H_
#define SERIALIZEIF_H_

#include <framework/returnvalues/HasReturnvaluesIF.h>
#include <cstddef>
#include <type_traits>

#ifndef ssize_t
typedef std::make_signed<std::size_t>::type ssize_t;
#endif

/**
 * @defgroup serialize Serialization
 * Contains serialisation services.
 */

/**
 * @brief An interface for alle classes which require
 *        translation of objects data into data streams and vice-versa.
 * @details
 * If the target architecture is little endian (e.g. ARM), any data types created might
 * have the wrong endiness if they are to be used for the FSFW.
 * There are three ways to retrieve data out of a buffer to be used in the FSFW to use
 * regular aligned (big endian) data. This can also be applied to uint32_t and uint64_t:
 *
 *   1. Use the @c AutoSerializeAdapter::deSerialize function with @c bigEndian = true
 *   2. Perform a bitshift operation
 *   3. @c memcpy can be used when data is in little-endian format. Otherwise, @c EndianSwapper has to be used in conjuction.
 *
 * When serializing for downlink, the packets are generally serialized assuming big endian data format
 * like seen in TmPacketStored.cpp for example.
 *
 * @ingroup serialize
 */
class SerializeIF {
public:
	static const uint8_t INTERFACE_ID = CLASS_ID::SERIALIZE_IF;
	static const ReturnValue_t BUFFER_TOO_SHORT = MAKE_RETURN_CODE(1);
	static const ReturnValue_t STREAM_TOO_SHORT = MAKE_RETURN_CODE(2);
	static const ReturnValue_t TOO_MANY_ELEMENTS = MAKE_RETURN_CODE(3);

	virtual ~SerializeIF() {
	}

	virtual ReturnValue_t serialize(uint8_t** buffer, size_t* size,
			const size_t max_size, bool bigEndian) const = 0;

	virtual size_t getSerializedSize() const = 0;

	virtual ReturnValue_t deSerialize(const uint8_t** buffer, ssize_t* size,
			bool bigEndian) = 0;

};

#endif /* SERIALIZEIF_H_ */
