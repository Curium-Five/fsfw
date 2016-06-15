#include <framework/serialize/SerializeAdapter.h>
#include <framework/globalfunctions/Type.h>
#include <framework/serialize/SerializeAdapter.h>

Type::Type() :
		actualType(UNKNOWN_TYPE) {
}

Type::Type(ActualType_t actualType) :
		actualType(actualType) {
}

Type::Type(const Type& type) :
		actualType(type.actualType) {
}

Type& Type::operator =(Type rhs) {
	this->actualType = rhs.actualType;
	return *this;
}

Type& Type::operator =(ActualType_t actualType) {
	this->actualType = actualType;
	return *this;
}

Type::operator Type::ActualType_t() const {
	return actualType;
}

bool Type::operator ==(const Type& rhs) {
	return this->actualType == rhs.actualType;
}

bool Type::operator !=(const Type& rhs) {
	return !operator==(rhs);
}

uint8_t Type::getSize() const {
	switch (actualType) {
	case UINT8_T:
		return sizeof(uint8_t);
	case INT8_T:
		return sizeof(int8_t);
	case UINT16_T:
		return sizeof(uint16_t);
	case INT16_T:
		return sizeof(int16_t);
	case UINT32_T:
		return sizeof(uint32_t);
	case INT32_T:
		return sizeof(int32_t);
	case FLOAT:
		return sizeof(float);
	case DOUBLE:
		return sizeof(double);
	default:
		return 0;
	}
}

ReturnValue_t Type::serialize(uint8_t** buffer, uint32_t* size,
		const uint32_t max_size, bool bigEndian) const {
	uint8_t ptc;
	uint8_t pfc;
	ReturnValue_t result = getPtcPfc(&ptc, &pfc);
	if (result != HasReturnvaluesIF::RETURN_OK) {
		return result;
	}

	result = SerializeAdapter<uint8_t>::serialize(&ptc, buffer, size, max_size,
			bigEndian);
	if (result != HasReturnvaluesIF::RETURN_OK) {
		return result;
	}

	result = SerializeAdapter<uint8_t>::serialize(&pfc, buffer, size, max_size,
			bigEndian);

	return result;

}

uint32_t Type::getSerializedSize() const {
	uint8_t dontcare = 0;
	return 2 * SerializeAdapter<uint8_t>::getSerializedSize(&dontcare);
}

ReturnValue_t Type::deSerialize(const uint8_t** buffer, int32_t* size,
		bool bigEndian) {
	uint8_t ptc;
	uint8_t pfc;
	ReturnValue_t result = SerializeAdapter<uint8_t>::deSerialize(&ptc, buffer,
			size, bigEndian);
	if (result != HasReturnvaluesIF::RETURN_OK) {
		return result;
	}

	result = SerializeAdapter<uint8_t>::deSerialize(&pfc, buffer, size,
			bigEndian);
	if (result != HasReturnvaluesIF::RETURN_OK) {
		return result;
	}

	actualType = getActualType(ptc, pfc);

	return HasReturnvaluesIF::RETURN_OK;
}

ReturnValue_t Type::getPtcPfc(uint8_t* ptc, uint8_t* pfc) const {
	switch (actualType) {
	case UINT8_T:
		*ptc = 3;
		*pfc = 4;
		break;
	case INT8_T:
		*ptc = 4;
		*pfc = 4;
		break;
	case UINT16_T:
		*ptc = 3;
		*pfc = 12;
		break;
	case INT16_T:
		*ptc = 4;
		*pfc = 12;
		break;
	case UINT32_T:
		*ptc = 3;
		*pfc = 14;
		break;
	case INT32_T:
		*ptc = 4;
		*pfc = 14;
		break;
	case FLOAT:
		*ptc = 5;
		*pfc = 1;
		break;
	case DOUBLE:
		*ptc = 5;
		*pfc = 2;
		break;
	default:
		return HasReturnvaluesIF::RETURN_FAILED;
	}
	return HasReturnvaluesIF::RETURN_OK;
}

Type::ActualType_t Type::getActualType(uint8_t ptc, uint8_t pfc) {
	switch (ptc) {
	case 3:
		switch (pfc) {
		case 4:
			return UINT8_T;
		case 12:
			return UINT16_T;
		case 14:
			return UINT32_T;
		}
		break;
	case 4:
		switch (pfc) {
		case 4:
			return INT8_T;
		case 12:
			return INT16_T;
		case 14:
			return INT32_T;
		}
		break;
	case 5:
		switch (pfc) {
		case 1:
			return FLOAT;
		case 2:
			return DOUBLE;
		}
		break;
	}
	return UNKNOWN_TYPE;
}
