
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

namespace Entry {

	void LogLayer::OnAttach()
	{
		m_ShowLogs = true;
		m_Console = Log::GetPrintConsole().get();
		m_Width = m_Console->windowWidth * 8;
		m_Height = m_Console->windowHeight * 8;

		// Initialize console buffer texture
		static C3D_Tex tex;
		static const Tex3DS_SubTexture subtex = { 256, 256, 0.0f, 1.0f, 1.0f, 0.0f };
		image = (C2D_Image){ &tex, &subtex };
		C3D_TexInit(image.tex, 256, 256, GPU_RGBA5551);
		C3D_TexSetFilter(image.tex, GPU_NEAREST, GPU_NEAREST);
		C3D_TexSetWrap(image.tex, GPU_REPEAT, GPU_REPEAT);
	}

	void LogLayer::OnDetach()
	{
	}

	void LogLayer::OnUpdate(Timestep ts) {
		//printf("fps %.1f fps\ncpu: %.2f ms\ngpu: %.2f ms\n", 1000.0f / C3D_GetProcessingTime(), C3D_GetProcessingTime(), C3D_GetDrawingTime());
		if (Input::GetButtonDown(ET_KEY_SELECT)) {
			m_ShowLogs = !m_ShowLogs;
		}

		if (Input::GetButtonDown(ET_KEY_A)) {
			ET_CORE_TRACE("A Button Pressed.");
		}

		if (!m_ShowLogs) return;

		// Only update once per second.
		static float redrawTimer = 1.0f;
		static int clearCounter = 0;
		redrawTimer += ts;
		
		if (m_LastX != m_Console->cursorX || 
			m_LastY != m_Console->cursorY || 
			redrawTimer >= 1.0f) 
		{
			redrawTimer = 0.0f;
			
			if (clearCounter >= 5) {
				std::fill_n(m_Console->frameBuffer, m_Height * m_Width, 0x0);
				clearCounter = 0;
				m_Console->cursorX = 0;
				m_Console->cursorY = 0;
				CopyFramebufferToTexture();
			}else if (m_LastX == m_Console->cursorX && m_LastY == m_Console->cursorY) {
				clearCounter++;
			}
			else {
				m_LastX = m_Console->cursorX;
				m_LastY = m_Console->cursorY;
				clearCounter = 0;
				CopyFramebufferToTexture();
			}
		}
		C2D_DrawImageAt(image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
	}

	void LogLayer::OnEvent(Event& event)
	{

	}
	void LogLayer::CopyFramebufferToTexture()
	{
		u32 endY = (m_Console->cursorY + 1) * 8;
		endY = m_Console->cursorY == 0 ? m_Height : endY;

		// TODO: Minimize how often this copy is done
		// Copy from console framebuffer to texture
		for (u32 y = 0; y < endY; ++y)
		{
			for (u32 x = 0; x < m_Width; ++x)
			{
				uint32_t dest = ((((y >> 3) * (256 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
				u16& pixel = ((u16*)image.tex->data)[dest];
				pixel = m_Console->frameBuffer[(x * m_Width) + (m_Width - 1 - y)];
				pixel |= pixel == 0 ? 0 : 1;
			}
		}
	}
}