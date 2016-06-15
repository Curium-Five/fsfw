/**
 * @file	SerialLinkedListAdapter.h
 * @brief	This file defines the SerialLinkedListAdapter class.
 * @date	22.07.2014
 * @author	baetz
 */
#ifndef SERIALLINKEDLISTADAPTER_H_
#define SERIALLINKEDLISTADAPTER_H_

#include <framework/container/SinglyLinkedList.h>
#include <framework/serialize/SerializeAdapter.h>
#include <framework/serialize/SerializeElement.h>
#include <framework/serialize/SerializeIF.h>
//This is where we need the SerializeAdapter!
template<typename T, typename count_t = uint8_t>
class SerialLinkedListAdapter: public SinglyLinkedList<T>, public SerializeIF {
public:
	SerialLinkedListAdapter(typename LinkedElement<T>::Iterator start,
			bool printCount = false) :
			SinglyLinkedList<T>(start), printCount(printCount) {
	}
	SerialLinkedListAdapter(LinkedElement<T>* first, bool printCount = false) :
			SinglyLinkedList<T>(first), printCount(printCount) {

	}
	SerialLinkedListAdapter(bool printCount = false) :
			SinglyLinkedList<T>(), printCount(printCount) {
	}

	virtual ReturnValue_t serialize(uint8_t** buffer, uint32_t* size,
			const uint32_t max_size, bool bigEndian) const {
		if (printCount) {
			count_t mySize = SinglyLinkedList<T>::getSize();
			ReturnValue_t result = SerializeAdapter<count_t>::serialize(&mySize,
					buffer, size, max_size, bigEndian);
			if (result != HasReturnvaluesIF::RETURN_OK) {
				return result;
			}
		}
		return serialize(SinglyLinkedList<T>::start, buffer, size, max_size,
				bigEndian);
	}

	static ReturnValue_t serialize(const LinkedElement<T>* element,
			uint8_t** buffer, uint32_t* size, const uint32_t max_size,
			bool bigEndian) {
		ReturnValue_t result = HasReturnvaluesIF::RETURN_OK;
		while ((result == HasReturnvaluesIF::RETURN_OK) && (element != NULL)) {
			result = element->value->serialize(buffer, size, max_size,
					bigEndian);
			element = element->getNext();
		}
		return result;
	}
	virtual uint32_t getSerializedSize() const {
		if (printCount) {
			return SerialLinkedListAdapter<T>::getSerializedSize()
					+ sizeof(count_t);
		} else {
			return getSerializedSize(SinglyLinkedList<T>::start);
		}
	}
	static uint32_t getSerializedSize(const LinkedElement<T> *element) {
		uint32_t size = 0;
		while (element != NULL) {
			size += element->value->getSerializedSize();
			element = element->getNext();
		}
		return size;
	}

	virtual ReturnValue_t deSerialize(const uint8_t** buffer, int32_t* size,
			bool bigEndian) {
		return deSerialize(SinglyLinkedList<T>::start, buffer, size, bigEndian);
	}

	static ReturnValue_t deSerialize(LinkedElement<T>* element,
			const uint8_t** buffer, int32_t* size, bool bigEndian) {
		ReturnValue_t result = HasReturnvaluesIF::RETURN_OK;
		while ((result == HasReturnvaluesIF::RETURN_OK) && (element != NULL)) {
			result = element->value->deSerialize(buffer, size, bigEndian);
			element = element->getNext();
		}
		return result;
	}

	bool printCount;

};

#endif /* SERIALLINKEDLISTADAPTER_H_ */
