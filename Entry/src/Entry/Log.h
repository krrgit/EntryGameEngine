#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Entry
{
    class Log
    {
    public:
        static void Init(int screen = 1);

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

// Core Log Macros
#define ET_CORE_TRACE(...)  ::Entry::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ET_CORE_INFO(...)   ::Entry::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ET_CORE_WARN(...)   ::Entry::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ET_CORE_ERROR(...)  ::Entry::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ET_CORE_FATAL(...)  ::Entry::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client Log Macros
#define ET_TRACE(...)  ::Entry::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ET_INFO(...)   ::Entry::Log::GetClientLogger()->info(__VA_ARGS__)
#define ET_WARN(...)   ::Entry::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ET_ERROR(...)  ::Entry::Log::GetClientLogger()->error(__VA_ARGS__)
#define ET_FATAL(...)  ::Entry::Log::GetClientLogger()->fatal(__VA_ARGS__)