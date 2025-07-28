
#include "Entry/Log/LogLayer.h"
#include "Entry/Core/Input.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <stdio.h>
#include <string.h>
#include <sys/iosupport.h>
#include <3ds/gfx.h>
#include <3ds/svc.h>

#include "imgui.h"


#define R565_SHIFT 11
#define G565_SHIFT 5
#define B565_SHIFT 0
#define R565_MASK 0xF800
#define G565_MASK 0x07E0
#define B565_MASK 0x001F

#define UV_LINE_HEIGHT 0.0313f // Height of one text line in UV tex coords
 

namespace Entry {

	void LogLayer::OnAttach()
	{
		m_ShowLogs = true;
		m_Console = Log::GetPrintConsole().get();
		m_Width = m_Console->windowWidth * 8;
		m_Height = m_Console->windowHeight * 8;
		
		// Initialize console buffer texture
		m_Subtex = { 256, 256, 0.0f, 1.0f, 1.0f, 0.0f };
		m_SubtexVerticalShift = ((m_Console->cursorY + 1) * 8.0f) / 240.0f;
		m_Image = (C2D_Image){ &m_Tex, &m_Subtex};
		C3D_TexInitVRAM(m_Image.tex, 256, 256, GPU_RGBA5551);
		C3D_TexSetFilter(m_Image.tex, GPU_NEAREST, GPU_NEAREST);
		C3D_TexSetWrap(m_Image.tex, GPU_REPEAT, GPU_REPEAT);
	}

	void LogLayer::OnDetach()
	{
	}

	void LogLayer::OnImGuiRender() {
		ImGui::Begin("Log");
		ImGui::Text("fps %.1f fps\ncpu: %.2f ms\ngpu: %.2f ms\n", 1000.0f / C3D_GetProcessingTime(), C3D_GetProcessingTime(), C3D_GetDrawingTime());

		ImGui::End();
	}

	void LogLayer::OnUpdate(Timestep ts) {
		ET_PROFILE_FUNCTION();

		if (Input::GetButtonDown(ET_KEY_SELECT)) {
			m_ShowLogs = !m_ShowLogs;
		}

		if (Input::GetButtonDown(ET_KEY_A)) {
			ET_CORE_TRACE("A Button Pressed.");
		}
		if (Input::GetButtonDown(ET_KEY_B)) {
			ET_CORE_TRACE("B Button Pressed.");
		}

		if (!m_ShowLogs) return;

		m_ConsoleUpdated = m_PrevCursorPosX != m_Console->cursorX || m_PrevCursorPosY != m_Console->cursorY;

		if (m_ConsoleUpdated) {
			m_PrevCursorPosX = m_Console->cursorX;
			m_PrevCursorPosY = m_Console->cursorY;
			m_ConsoleUpdated = false;

			// Shift the subtexture up by 1 text line
			m_SubtexVerticalShift += UV_LINE_HEIGHT;
			
			// Loop cursor around to start.
			if (m_PrevCursorPosY + 1 >= m_Console->windowHeight)
			{
				m_PrevCursorPosX = 0;
				m_PrevCursorPosY = 0;
				m_Console->cursorX = 0;
				m_Console->cursorY = 0;
			}

			// Loop subtexture around once first line is printed to.
			if (m_Console->cursorY == 1) 
			{
				m_SubtexVerticalShift = (3.0f *8.0f) / 240.0f;
			}

			ClearNextLine();
			CopyFramebufferToTexture();
		}

		m_Subtex.bottom = -m_SubtexVerticalShift;
		m_Subtex.top = 1.0f - m_SubtexVerticalShift;
		m_Image.subtex = const_cast<Tex3DS_SubTexture*>(&m_Subtex);

		C2D_DrawImageAt(m_Image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
	}

	void LogLayer::OnEvent(Event& event)
	{

	}
	void LogLayer::CopyFramebufferToTexture()
	{
		ET_PROFILE_FUNCTION();

		u32 endY = (m_Console->cursorY + 1) * 8;
		endY = m_Console->cursorY == 0 ? m_Height : endY;

		 //TODO: update to only copy line instead whole texture 
		// Copy from console framebuffer to texture
		for (u32 y = 0; y < endY; ++y)
		{
			for (u32 x = 0; x < m_Width; ++x)
			{
				uint32_t dest = ((((y >> 3) * (256 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
				u16& pixel = ((u16*)m_Image.tex->data)[dest];
				pixel = m_Console->frameBuffer[(x * m_Width) + (m_Width - 1 - y)];
				pixel |= pixel == 0 ? 0 : 1;
			}
		}
	}

	void LogLayer::ClearNextLine() {
		printf("\x1b[2K");
	}
}