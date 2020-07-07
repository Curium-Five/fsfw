#ifndef FRAMEWORK_GLOBALFUNCTIONS_DLEENCODER_H_
#define FRAMEWORK_GLOBALFUNCTIONS_DLEENCODER_H_

#include <framework/returnvalues/HasReturnvaluesIF.h>
#include <cstddef>

/**
 * @brief   This DLE Encoder (Data Link Encoder) can be used to encode and
 *          decode arbitrary data with ASCII control characters
 * @details
 * List of control codes:
 * https://en.wikipedia.org/wiki/C0_and_C1_control_codes
 *
 * This encoder can be used to achieve a basic transport layer when using
 * char based transmission systems.
 * The passed source strean is converted into a encoded stream by adding
 * a STX marker at the startr of the stream and an ETX marker at the end of
 * the stream. Any STX, ETX and DLE occurences in the source stream are escaped
 * by a DLE character.
 */
class DleEncoder: public HasReturnvaluesIF {
private:
	DleEncoder();
	virtual ~DleEncoder();

public:
	static constexpr uint8_t INTERFACE_ID = CLASS_ID::DLE_ENCODER;
	static constexpr ReturnValue_t STREAM_TOO_SHORT = MAKE_RETURN_CODE(0x01);
	static constexpr ReturnValue_t DECODING_ERROR = MAKE_RETURN_CODE(0x02);

	//! Start Of Text character. First character is encoded stream
	static const uint8_t STX = 0x02;
	//! End Of Text character. Last character in encoded stream
	static const uint8_t ETX = 0x03;
	//! Data Link Escape character. Used to escape STX, ETX and DLE occurences
	//! in the source stream.
	static const uint8_t DLE = 0x10;

    /**
     * Encodes the give data stream by preceding it with the STX marker
     * and ending it with an ETX marker. STX, ETX and DLE characters inside
     * the stream are escaped by DLE characters.
     * @param sourceStream
     * @param sourceLen
     * @param destStream
     * @param maxDestLen
     * @param encodedLen
     * @param addStxEtx
     * Adding STX and ETX can be omitted, if they are added manually.
     * @return
     */
    static ReturnValue_t encode(const uint8_t *sourceStream, size_t sourceLen,
            uint8_t *destStream, size_t maxDestLen, size_t *encodedLen,
            bool addStxEtx = true);

    /**
     * Converts an encoded stream back
     * @param sourceStream
     * @param sourceStreamLen
     * @param readLen
     * @param destStream
     * @param maxDestStreamlen
     * @param decodedLen
     * @return
     */
	static ReturnValue_t decode(const uint8_t *sourceStream,
			size_t sourceStreamLen, size_t *readLen, uint8_t *destStream,
			size_t maxDestStreamlen, size_t *decodedLen);
};

#endif /* FRAMEWORK_GLOBALFUNCTIONS_DLEENCODER_H_ */
