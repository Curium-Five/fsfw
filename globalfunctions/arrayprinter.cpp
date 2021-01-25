#include "arrayprinter.h"
#include "../serviceinterface/ServiceInterface.h"
#include <bitset>


void arrayprinter::print(const uint8_t *data, size_t size, OutputType type,
        bool printInfo, size_t maxCharPerLine) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
    if(printInfo) {
         sif::info << "Printing data with size " << size << ": " << std::endl;
    }
#else
#if FSFW_NO_C99_IO == 1
    sif::printInfo("Printing data with size %lu: \n", static_cast<unsigned long>(size));
#else
    sif::printInfo("Printing data with size %zu: \n", size);
#endif /* FSFW_NO_C99_IO == 1 */
#endif /* FSFW_CPP_OSTREAM_ENABLED == 1 */

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
    if(sif::info.crAdditionEnabled()) {
        std::cout << "\r" << std::endl;
    }

    std::cout << "[" << std::hex;
    for(size_t i = 0; i < size; i++) {
        std::cout << "0x" << static_cast<int>(data[i]);
        if(i < size - 1) {
            std::cout << " , ";
            if(i > 0 and (i + 1) % maxCharPerLine == 0) {
                std::cout << std::endl;

            }
        }
    }
    std::cout << std::dec;
    std::cout << "]" << std::endl;
#else
    // General format: 0x01, 0x02, 0x03 so it is number of chars times 6
    // plus line break plus small safety margin.
    char printBuffer[(size + 1) * 7 + 1];
    size_t currentPos = 0;
    for(size_t i = 0; i < size; i++) {
        // To avoid buffer overflows.
        if(sizeof(printBuffer) - currentPos <= 7) {
            break;
        }

        currentPos += snprintf(printBuffer + currentPos, 6, "0x%02x", data[i]);
        if(i < size - 1) {
            currentPos += sprintf(printBuffer + currentPos, ", ");
            if(i > 0 and (i + 1) % maxCharPerLine == 0) {
                currentPos += sprintf(printBuffer + currentPos, "\n");
            }
        }
    }
    printf("[%s]\n", printBuffer);
#endif
}

void arrayprinter::printDec(const uint8_t *data, size_t size,
        size_t maxCharPerLine) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
    if(sif::info.crAdditionEnabled()) {
        std::cout << "\r" << std::endl;
    }

    std::cout << "[" << std::dec;
    for(size_t i = 0; i < size; i++) {
        std::cout << static_cast<int>(data[i]);
        if(i < size - 1){
            std::cout << " , ";
            if(i > 0 and (i + 1) % maxCharPerLine == 0) {
                std::cout << std::endl;
            }
        }
    }
    std::cout << "]" << std::endl;
#else
    // General format: 32, 243, -12 so it is number of chars times 5
    // plus line break plus small safety margin.
    char printBuffer[(size + 1) * 5 + 1];
    size_t currentPos = 0;
    for(size_t i = 0; i < size; i++) {
        // To avoid buffer overflows.
        if(sizeof(printBuffer) - currentPos <= 5) {
            break;
        }

        currentPos += snprintf(printBuffer + currentPos, 3, "%d", data[i]);
        if(i < size - 1) {
            currentPos += sprintf(printBuffer + currentPos, ", ");
            if(i > 0 and (i + 1) % maxCharPerLine == 0) {
                currentPos += sprintf(printBuffer + currentPos, "\n");
            }
        }
    }
    printf("[%s]\n", printBuffer);
#endif
}

void arrayprinter::printBin(const uint8_t *data, size_t size) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
    for(size_t i = 0; i < size; i++) {
        sif::info << "Byte " << i + 1 << ": 0b" <<  std::bitset<8>(data[i]) << std::endl;
    }
#else
    for(size_t i = 0; i < size; i++) {
        sif::printInfo("Byte %d: 0b" BYTE_TO_BINARY_PATTERN "\n", i + 1, BYTE_TO_BINARY(data[i]));
    }
#endif
}
