#pragma once

#include <memory>
#include <3ds.h>
#include <cstdio>
#include <cstdarg>
#include "Core.h"
// #include "spdlog/spdlog.h"

// ANSI escape codes for text color
constexpr const char* ANSI_COLOR_RESET = "\033[0m";
constexpr const char* ANSI_COLOR_GREEN = "\033[32m";
constexpr const char* ANSI_COLOR_YELLOW = "\033[33m";
constexpr const char* ANSI_COLOR_RED = "\033[31m";
constexpr const char* ANSI_COLOR_MAGENTA = "\033[35m";
constexpr const char* COLOR_WHITE_ON_RED = "\033[41;37m";
namespace Entry
{
    class Log
    {
    public:
        static void Init();

        static void Trace(const char* format, ...);
        static void Info(const char* format, ...);
        static void Warn(const char* format, ...);
        static void Error(const char* format, ...);
        static void Fatal(const char* format, ...);

        inline static std::shared_ptr<PrintConsole>& GetCoreLogger() { return s_CoreLogger; }

    private:
        static std::shared_ptr<PrintConsole> s_CoreLogger;
    };
}

// Core Log Macros
#define ET_CORE_TRACE(...)  ::Entry::Log::Trace(__VA_ARGS__)
#define ET_CORE_INFO(...)   ::Entry::Log::Info(__VA_ARGS__)
#define ET_CORE_WARN(...)   ::Entry::Log::Warn(__VA_ARGS__)
#define ET_CORE_ERROR(...)  ::Entry::Log::Error(__VA_ARGS__)
#define ET_CORE_FATAL(...)  ::Entry::Log::Fatal(__VA_ARGS__)

// Client Log Macros (TODO: separate header)
#define ET_TRACE(...)  ::Entry::Log::Trace(__VA_ARGS__)
#define ET_INFO(...)   ::Entry::Log::Info(__VA_ARGS__)
#define ET_WARN(...)   ::Entry::Log::Warn(__VA_ARGS__)
#define ET_ERROR(...)  ::Entry::Log::Error(__VA_ARGS__)
#define ET_FATAL(...)  ::Entry::Log::Fatal(__VA_ARGS__)