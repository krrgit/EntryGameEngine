
#include "Entry/Log/LogLayer.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <stdio.h>
#include <string.h>
#include <sys/iosupport.h>
#include <3ds/gfx.h>
#include <3ds/svc.h>


#define R565_SHIFT 11
#define G565_SHIFT 5
#define B565_SHIFT 0
#define R565_MASK 0xF800
#define G565_MASK 0x07E0
#define B565_MASK 0x001F

//#define LOG_CONSOLE_ENABLED

namespace Entry {

	/////////////////////////////////////////////////////////////////
	// LOG LAYER ////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////// 


	std::shared_ptr<spdlog::logger> LogLayer::s_CoreLogger;
	std::shared_ptr<spdlog::logger> LogLayer::s_ClientLogger;
	std::shared_ptr<PrintConsole> LogLayer::s_PrintConsole;

	void LogLayer::OnAttach()
	{
		// Store current GFX values
		gfxScreen_t screen = GFX_BOTTOM;
		GSPGPU_FramebufferFormat fbFormat = gfxGetScreenFormat(screen);
		bool doubleBuffering = false;
		
		// Initialize console
		m_Console = consoleGetDefault();
		consoleInit(screen, m_Console);

		// Restore GFX values
		gfxSetScreenFormat(screen, fbFormat);
		gfxSetDoubleBuffering(screen, doubleBuffering);
		gfxSwapBuffersGpu();
		gspWaitForVBlank();

		m_Width = 240;
		m_Height = 240;
		// Initialize console buffer texture
		C3D_Tex* tex = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		static const Tex3DS_SubTexture subt3x = { 256, 256, 0.0f, 1.0f, 1.0f, 0.0f };
		image = (C2D_Image){ tex, &subt3x };
		C3D_TexInit(image.tex, 256, 256, GPU_RGB565);
		C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);
		C3D_TexSetWrap(image.tex, GPU_REPEAT, GPU_REPEAT);

		m_ConsoleBuffer = (u16*)linearAlloc(m_Width * m_Height * sizeof(u16));
		m_Console->frameBuffer = m_ConsoleBuffer;

		consoleSetWindow(m_Console, 0, 0, 30, 30);
		index = 0;
		printf("Hello, World!\n");
		Initspdlog();
	}

	void LogLayer::OnDetach()
	{
	}

	void LogLayer::Initspdlog() {
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("ET");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("AP");
		s_ClientLogger->set_level(spdlog::level::trace);

		auto color_sink = static_cast<spdlog::sinks::stdout_color_sink_mt*>(s_CoreLogger->sinks().back().get());
		color_sink->set_color_mode(spdlog::color_mode::always);

		auto color_sink2 = static_cast<spdlog::sinks::stdout_color_sink_mt*>(s_ClientLogger->sinks().back().get());
		color_sink2->set_color_mode(spdlog::color_mode::always);
	}

	void LogLayer::OnUpdate() {
		// TODO: Modify console.c to draw directly to correct coordinates.
		for (u32 y = 0; y < m_Height; ++y) 
		{
			for (u32 x = 0; x < m_Width; ++x) 
			{
				uint32_t dest = ((((y >> 3) * (256 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
				u16& pixel = ((u16*)image.tex->data)[dest];
				pixel = m_ConsoleBuffer[(x * m_Width) + (m_Width- 1 - y)];
			}
		}

		printf("%lu Hello, World!\n", index++);
		C2D_DrawImageAt(image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
	}
	void LogLayer::OnEvent(Event& event)
	{
	}
}