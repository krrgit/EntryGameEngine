
#include "Entry/Log/LogLayer.h"
#include "Entry/Input.h"
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

	void LogLayer::OnAttach()
	{
		showLogs = true;
		m_Console = Log::GetPrintConsole().get();
		m_Width = m_Console->windowWidth * 8;
		m_Height = m_Console->windowHeight * 8;

		// Initialize console buffer texture
		C3D_Tex* tex = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		static const Tex3DS_SubTexture subt3x = { 256, 256, 0.0f, 1.0f, 1.0f, 0.0f };
		image = (C2D_Image){ tex, &subt3x };
		C3D_TexInit(image.tex, 256, 256, GPU_RGBA5551);
		C3D_TexSetFilter(image.tex, GPU_NEAREST, GPU_NEAREST);
		C3D_TexSetWrap(image.tex, GPU_REPEAT, GPU_REPEAT);
	}

	void LogLayer::OnDetach()
	{
	}

	void LogLayer::OnUpdate() {
		if (Input::GetButtonDown(ET_KEY_SELECT)) {
			showLogs = !showLogs;
		}

		if (!showLogs) return;
		//printf("fps %.1f fps\ncpu: %.2f ms\ngpu: %.2f ms\n", 1000.0f / C3D_GetProcessingTime(), C3D_GetProcessingTime(), C3D_GetDrawingTime());

		// TODO: Find way to avoid this copy
		// Copy from console framebuffer to texture
		for (u32 y = 0; y < m_Height; ++y) 
		{
			for (u32 x = 0; x < m_Width; ++x) 
			{
				uint32_t dest = ((((y >> 3) * (256 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
				u16& pixel = ((u16*)image.tex->data)[dest];
				pixel = m_Console->frameBuffer[(x * m_Width) + (m_Width - 1 - y)];
				pixel |= pixel == 0 ? 0 : 1;
			}
		}
		C2D_DrawImageAt(image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
	}

	void LogLayer::OnEvent(Event& event)
	{

	}
}