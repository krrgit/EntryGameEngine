#include "etpch.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Entry
{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init()
    {
        consoleInit(GFX_TOP, NULL);
        spdlog::set_pattern("%^[%T] %n: %v%$");
        s_CoreLogger = spdlog::stdout_color_mt("ENTRY");
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = spdlog::stdout_color_mt("APP");
        s_ClientLogger->set_level(spdlog::level::trace);

        auto color_sink = static_cast<spdlog::sinks::stdout_color_sink_mt*>(s_CoreLogger->sinks().back().get());
        color_sink->set_color_mode(spdlog::color_mode::always);

        auto color_sink2 = static_cast<spdlog::sinks::stdout_color_sink_mt*>(s_ClientLogger->sinks().back().get());
        color_sink2->set_color_mode(spdlog::color_mode::always);
    }
}