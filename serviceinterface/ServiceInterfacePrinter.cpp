#include "ServiceInterfacePrinter.h"

#include "../timemanager/Clock.h"

#include <FSFWConfig.h>
#include <cstdarg>
#include <cstdint>


fsfw::PrintLevel printLevel = fsfw::PrintLevel::DEBUG;
uint8_t printBuffer[fsfwconfig::FSFW_PRINT_BUFFER_SIZE];

void fsfwPrint(fsfw::PrintLevel printType, const char* fmt, va_list arg) {
    uint32_t len = 0;

    /* Check logger level */
    if(printType == fsfw::PrintLevel::NONE or printType > printLevel) {
        return;
    }

    /* Log message to terminal */

#if FSFW_COLORED_OUTPUT == 1
    if(printType == fsfw::PrintLevel::INFO) {
    	len += sprintf((char *)printBuffer, fsfw::ANSI_COLOR_GREEN);
    }
    else if(printType == fsfw::PrintLevel::DEBUG) {
    	len += sprintf((char *)printBuffer, fsfw::ANSI_COLOR_BLUE);
    }
    else if(printType == fsfw::PrintLevel::WARNING) {
    	len += sprintf((char *)printBuffer, fsfw::ANSI_COLOR_YELLOW);
    }
    else if(printType == fsfw::PrintLevel::ERROR_TYPE) {
    	len += sprintf((char *)printBuffer, fsfw::ANSI_COLOR_RED);
    }
#endif

   if (printType == fsfw::PrintLevel::INFO) {
       len += sprintf((char *)printBuffer + len, "INFO: ");
   }
   if(printType == fsfw::PrintLevel::DEBUG) {
	   len += sprintf((char *)printBuffer + len, "DEBUG: ");
   }
   if(printType == fsfw::PrintLevel::WARNING) {
	   len += sprintf((char *)printBuffer + len, "WARNING: ");
   }

   if(printType == fsfw::PrintLevel::ERROR_TYPE) {
	   len += sprintf((char *)printBuffer + len, "ERROR: ");
   }

    Clock::TimeOfDay_t now;
    Clock::getDateAndTime(&now);
    /*
     * Log current time to terminal if desired.
     */
    len += sprintf((char*)printBuffer + len, "| %lu:%02lu:%02lu.%03lu | ",
    		(unsigned long) now.hour,
			(unsigned long) now.minute,
			(unsigned long) now.second,
			(unsigned long) now.usecond /1000);

    //changed Jan 2017.  Need to update length with buffer size
    len += vsnprintf((char *)(printBuffer + len),
    		sizeof(printBuffer)-len, fmt, arg);

    printf("%s", printBuffer);
}

void fsfw::setPrintLevel(PrintLevel printLevel_) {
	printLevel = printLevel_;
}

fsfw::PrintLevel fsfw::getPrintLevel() {
	return printLevel;
}

void fsfw::printInfo(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fsfwPrint(fsfw::PrintLevel::INFO, fmt, args);
    va_end(args);
}

void fsfw::printWarning(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fsfwPrint(fsfw::PrintLevel::WARNING, fmt, args);
    va_end(args);
}

void fsfw::printDebug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fsfwPrint(fsfw::PrintLevel::DEBUG, fmt, args);
    va_end(args);
}

void fsfw::printError(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fsfwPrint(fsfw::PrintLevel::ERROR_TYPE, fmt, args);
    va_end(args);
}
