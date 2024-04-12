#pragma once

#include <memory>
#include <3ds.h>
#include "Core.h"
#include "spdlog/spdlog.h"

namespace Entry
{
    class Log
    {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

// Core Log Macros
// #define ET_CORE_TRACE(...)  ::Entry::Log::Trace(__VA_ARGS__)
// #define ET_CORE_INFO(...)   ::Entry::Log::Info(__VA_ARGS__)
// #define ET_CORE_WARN(...)   ::Entry::Log::Warn(__VA_ARGS__)
// #define ET_CORE_ERROR(...)  ::Entry::Log::Error(__VA_ARGS__)
// #define ET_CORE_FATAL(...)  ::Entry::Log::Fatal(__VA_ARGS__)

// // Client Log Macros (TODO: separate header)
// #define ET_TRACE(...)  ::Entry::Log::Trace(__VA_ARGS__)
// #define ET_INFO(...)   ::Entry::Log::Info(__VA_ARGS__)
// #define ET_WARN(...)   ::Entry::Log::Warn(__VA_ARGS__)
// #define ET_ERROR(...)  ::Entry::Log::Error(__VA_ARGS__)
// #define ET_FATAL(...)  ::Entry::Log::Fatal(__VA_ARGS__)