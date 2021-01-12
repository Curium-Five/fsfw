#include <FSFWConfig.h>
#include "ServiceInterfacePrinter.h"
#include "serviceInterfaceDefintions.h"
#include "../timemanager/Clock.h"

#include <cstdarg>
#include <cstdint>

static sif::PrintLevel printLevel = sif::PrintLevel::DEBUG_LEVEL;
#if defined(WIN32) && FSFW_COLORED_OUTPUT == 1
static bool consoleInitialized = false;
#endif /* defined(WIN32) && FSFW_COLORED_OUTPUT == 1 */
static bool addCrAtEnd = false;

#if FSFW_DISABLE_PRINTOUT == 0
uint8_t printBuffer[fsfwconfig::FSFW_PRINT_BUFFER_SIZE];

void fsfwPrint(sif::PrintLevel printType, const char* fmt, va_list arg) {

#if defined(WIN32) && FSFW_COLORED_OUTPUT == 1
	if(not consoleInitialized) {
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	}
	consoleInitialized = true;
#endif

    size_t len = 0;
    char* bufferPosition = reinterpret_cast<char*>(printBuffer);

    /* Check logger level */
    if(printType == sif::PrintLevel::NONE or printType > printLevel) {
        return;
    }

    /* Log message to terminal */

#if FSFW_COLORED_OUTPUT == 1
    if(printType == sif::PrintLevel::INFO_LEVEL) {
    	len += sprintf(bufferPosition, sif::ANSI_COLOR_GREEN);
    }
    else if(printType == sif::PrintLevel::DEBUG_LEVEL) {
    	len += sprintf(bufferPosition, sif::ANSI_COLOR_MAGENTA);
    }
    else if(printType == sif::PrintLevel::WARNING_LEVEL) {
    	len += sprintf(bufferPosition, sif::ANSI_COLOR_YELLOW);
    }
    else if(printType == sif::PrintLevel::ERROR_LEVEL) {
    	len += sprintf(bufferPosition, sif::ANSI_COLOR_RED);
    }
#endif

   if (printType == sif::PrintLevel::INFO_LEVEL) {
       len += sprintf(bufferPosition + len, "INFO: ");
   }
   if(printType == sif::PrintLevel::DEBUG_LEVEL) {
	   len += sprintf(bufferPosition + len, "DEBUG: ");
   }
   if(printType == sif::PrintLevel::WARNING_LEVEL) {
	   len += sprintf(bufferPosition + len, "WARNING: ");
   }

   if(printType == sif::PrintLevel::ERROR_LEVEL) {
	   len += sprintf(bufferPosition + len, "ERROR: ");
   }

    Clock::TimeOfDay_t now;
    Clock::getDateAndTime(&now);
    /*
     * Log current time to terminal if desired.
     */
    len += sprintf(bufferPosition + len, "| %lu:%02lu:%02lu.%03lu | ",
    		(unsigned long) now.hour,
			(unsigned long) now.minute,
			(unsigned long) now.second,
			(unsigned long) now.usecond /1000);

    len += vsnprintf(bufferPosition + len, sizeof(printBuffer) - len, fmt, arg);

    if(addCrAtEnd) {
    	len += sprintf(bufferPosition + len, "\r");
    }

    printf("%s", printBuffer);
}


void sif::printInfo(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fsfwPrint(sif::PrintLevel::INFO_LEVEL, fmt, args);
    va_end(args);
}

void sif::printWarning(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fsfwPrint(sif::PrintLevel::WARNING_LEVEL, fmt, args);
    va_end(args);
}

void sif::printDebug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fsfwPrint(sif::PrintLevel::DEBUG_LEVEL, fmt, args);
    va_end(args);
}

void sif::setToAddCrAtEnd(bool addCrAtEnd_) {
	addCrAtEnd = addCrAtEnd_;
}

void sif::printError(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fsfwPrint(sif::PrintLevel::ERROR_LEVEL, fmt, args);
    va_end(args);
}

#else

void sif::printInfo(const char *fmt, ...) {}
void sif::printWarning(const char *fmt, ...) {}
void sif::printDebug(const char *fmt, ...) {}
void sif::printError(const char *fmt, ...) {}

#endif /* FSFW_DISABLE_PRINTOUT == 0 */

void sif::setPrintLevel(PrintLevel printLevel_) {
	printLevel = printLevel_;
}

sif::PrintLevel sif::getPrintLevel() {
	return printLevel;
}
