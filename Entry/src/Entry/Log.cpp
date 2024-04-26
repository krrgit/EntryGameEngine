#include "etpch.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Entry
{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
    std::shared_ptr<PrintConsole> Log::s_PrintConsole;

    void Log::Init()
    {
        // Initialization of Log is separate from LogLayer.
        // This allows logs to be stored before the screen is displayed.
        s_PrintConsole = static_cast<std::shared_ptr<PrintConsole>>(consoleGetDefault());

        // Store current GFX values.
        // Screen is just used to initialize the console.
        // The screen its rendered on is set in LogLayer().
        gfxScreen_t screen = GFX_BOTTOM;
        GSPGPU_FramebufferFormat fbFormat = gfxGetScreenFormat(screen);
        bool doubleBuffering = false;

        // Initialize console
        int width = 240;
        int height = 240;
        consoleInit(screen, s_PrintConsole.get());
        s_PrintConsole->frameBuffer = (u16*)linearAlloc(width * height * sizeof(u16));
        consoleSetWindow(s_PrintConsole.get(), 0, 0, 30, 30);

        // Restore GFX settings
        gfxSetScreenFormat(screen, fbFormat);
        gfxSetDoubleBuffering(screen, doubleBuffering);
        gfxSwapBuffersGpu();
        gspWaitForVBlank();
        
        spdlog::set_pattern("%^[%M:%S]%n:%v%$");

        s_CoreLogger = spdlog::stdout_color_mt("ET");
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = spdlog::stdout_color_mt("AP");
        s_ClientLogger->set_level(spdlog::level::trace);

        auto color_sink = static_cast<spdlog::sinks::stdout_color_sink_mt*>(s_CoreLogger->sinks().back().get());
        color_sink->set_color_mode(spdlog::color_mode::always);

        auto color_sink2 = static_cast<spdlog::sinks::stdout_color_sink_mt*>(s_ClientLogger->sinks().back().get());
        color_sink2->set_color_mode(spdlog::color_mode::always);
        
    }
}