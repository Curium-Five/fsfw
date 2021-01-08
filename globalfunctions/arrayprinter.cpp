#include "arrayprinter.h"
#include "../serviceinterface/ServiceInterface.h"
#include <bitset>

void arrayprinter::print(const uint8_t *data, size_t size, OutputType type,
		bool printInfo, size_t maxCharPerLine) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
	if(printInfo) {
		sif::info << "Printing data with size " << size << ": ";
	}
	sif::info << "[";
#else
	fsfw::printInfo("Printing data with size %zu: [", size);
#endif
	if(type == OutputType::HEX) {
		arrayprinter::printHex(data, size, maxCharPerLine);
	}
	else if (type == OutputType::DEC) {
		arrayprinter::printDec(data, size, maxCharPerLine);
	}
	else if(type == OutputType::BIN) {
	    arrayprinter::printBin(data, size);
	}
}

void arrayprinter::printHex(const uint8_t *data, size_t size,
		size_t maxCharPerLine) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
	sif::info << std::hex;
	for(size_t i = 0; i < size; i++) {
		sif::info << "0x" << static_cast<int>(data[i]);
		if(i < size - 1){
			sif::info << " , ";
			if(i > 0 and i % maxCharPerLine == 0) {
				sif::info << std::endl;

			}
		}
	}
	sif::info << std::dec;
	sif::info << "]" << std::endl;
#else
	// how much memory to reserve for printout?
#endif
}

void arrayprinter::printDec(const uint8_t *data, size_t size,
		size_t maxCharPerLine) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
	sif::info << std::dec;
	for(size_t i = 0; i < size; i++) {
		sif::info << static_cast<int>(data[i]);
		if(i < size - 1){
			sif::info << " , ";
			if(i > 0 and i % maxCharPerLine == 0) {
				sif::info << std::endl;
			}
		}
	}
	sif::info << "]" << std::endl;
#else
	// how much memory to reserve for printout?
#endif
}

void arrayprinter::printBin(const uint8_t *data, size_t size) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
    sif::info << "\n" << std::flush;
    for(size_t i = 0; i < size; i++) {
        sif::info << "Byte " << i + 1 << ": 0b"<<
                std::bitset<8>(data[i]) << ",\n" << std::flush;
    }
    sif::info << "]" << std::endl;
#else
    // how much memory to reserve for printout?
#endif
}
