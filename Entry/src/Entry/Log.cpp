#include "Log.h"
#include <cstring>

namespace Entry
{
    std::shared_ptr<PrintConsole> Log::s_CoreLogger;

    void Log::Init()
    {
        s_CoreLogger.reset(consoleInit(GFX_BOTTOM, NULL));
    }

    void Log::Trace(const char* format, ...) {
        va_list args;
        va_start(args, format);
        std::printf("%s: ", "ENTRY");
        std::vprintf(format, args);
        std::printf("\n");
        va_end(args);
    }

    void Log::Info(const char* format, ...) {
        va_list args;
        va_start(args, format);
        std::printf("%s%s: ", ANSI_COLOR_GREEN, "ENTRY");
        std::vprintf(format, args);
        std::printf("%s\n", ANSI_COLOR_RESET);
        va_end(args);
    }

    void Log::Warn(const char* format, ...) {
        va_list args;
        va_start(args, format);
        std::printf("%s%s: ", ANSI_COLOR_YELLOW, "ENTRY");
        std::vprintf(format, args);
        std::printf("%s\n", ANSI_COLOR_RESET);
        va_end(args);
    }

    void Log::Error(const char* format, ...) {
        va_list args;
        va_start(args, format);
        std::printf("%s%s: ", ANSI_COLOR_RED, "ENTRY");
        std::vprintf(format, args);
        std::printf("%s\n", ANSI_COLOR_RESET);
        va_end(args);
    }

    void Log::Fatal(const char* format, ...) {
        // Set text color to magenta for Fatal
        va_list args;
        va_start(args, format);
        std::printf("%s%s: ", COLOR_WHITE_ON_RED, "ENTRY");
        std::vprintf(format, args);
        std::printf("%s\n", ANSI_COLOR_RESET);
        va_end(args);
    }
}