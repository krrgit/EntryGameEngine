
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

#define UV_LINE_HEIGHT 0.03125f // Height of one text line in UV tex coords
#define UV_FIRST_LINE_POS 0.095f //(0.005f + ((3.0f *8.0f) / 240.0f))
#define UV_LAST_LINE_POS 0.97f //(0.005f + ((3.0f *8.0f) / 240.0f))
 

namespace Entry {

	void LogLayer::OnAttach()
	{
		m_ShowLogs = true;
		m_Console = Log::GetPrintConsole().get();
		m_Width = m_Console->windowWidth * 8;
		m_Height = m_Console->windowHeight * 8;
		
		// Initialize console buffer texture
		m_Subtex = { 256, 256, 0.0f, 1.0f, 1.0f, 0.0f };
		m_SubtexVerticalShift = (((m_Console->cursorY + 1) * 8.0f) / 240.0f) - 0.012f;
		m_Image = (C2D_Image){ &m_Tex, &m_Subtex};
		C3D_TexInitVRAM(m_Image.tex, 256, 256, GPU_RGBA5551);
		C3D_TexSetFilter(m_Image.tex, GPU_NEAREST, GPU_NEAREST);
		C3D_TexSetWrap(m_Image.tex, GPU_REPEAT, GPU_REPEAT);

		CopyFramebufferToTexture(0, m_Console->cursorY);
	}

	void LogLayer::OnDetach()
	{
	}

	void LogLayer::OnImGuiRender() {
	}

	void LogLayer::OnUpdate(Timestep ts, uint16_t screenSide) {
		ET_PROFILE_FUNCTION();

		if (Input::GetButtonDown(ET_KEY_SELECT)) {
			m_ShowLogs = !m_ShowLogs;
		}

		if (Input::GetButtonDown(ET_KEY_A)) {
			ET_CORE_TRACE("A Pressed.");
		}
		if (Input::GetButtonDown(ET_KEY_B)) {
			ET_CORE_TRACE("B Pressed.");
		}

		if (!m_ShowLogs) return;

		m_ConsoleUpdated = m_PrevCursorPosX != m_Console->cursorX || m_PrevCursorPosY != m_Console->cursorY;

		if (m_ConsoleUpdated) {
			int currentCursorX = m_Console->cursorX;
			int currentCursorY = m_Console->cursorY;

			// Loop subtexture around once first line is printed to.
			if (currentCursorY == 1)
			{
				m_SubtexVerticalShift = UV_FIRST_LINE_POS;
			} else if (currentCursorY == 0) 
			{
				// Align subtexture with last line.
				m_SubtexVerticalShift = UV_LAST_LINE_POS;
			}
			else 
			{
				// Shift the subtexture up by 1 text line
				m_SubtexVerticalShift += UV_LINE_HEIGHT;
			}

			// Loop cursor around to start once it reaches the end.
			if (currentCursorY + 1 >= m_Console->windowHeight)
			{

				m_Console->cursorX = 0;
				m_Console->cursorY = 0;
			}

			ClearNextLine();
			CopyFramebufferToTexture(m_PrevCursorPosY, currentCursorY);

			m_PrevCursorPosX = currentCursorX;
			m_PrevCursorPosY = currentCursorY;
			m_ConsoleUpdated = false;
		}

		m_Subtex.bottom = -m_SubtexVerticalShift;
		m_Subtex.top = 1.0f - m_SubtexVerticalShift;
		m_Image.subtex = const_cast<Tex3DS_SubTexture*>(&m_Subtex);

		C2D_DrawImageAt(m_Image, 0.0f, 0.0f, 0.0f, NULL, 0.5f, 1.0f);
	}

	void LogLayer::OnEvent(Event& event)
	{

	}
	
	void LogLayer::CopyFramebufferToTexture(int prevY, int currY)
	{
		ET_PROFILE_FUNCTION();

		// Determine vertical pixel range (line height is 8 pixels)
		int startY = std::min(prevY, currY) * 8;
		int endY = (std::max(prevY, currY) + 1) * 8;

		// Clamp to screen height to avoid overflow
		startY = startY < m_Height ? startY : m_Height;
		endY = endY < m_Height ? endY : m_Height;

		u16* texData = (u16*)m_Image.tex->data;
		u16* fb = m_Console->frameBuffer;
		const u32 blocksX = 256 >> 3;
		const u32 width = m_Width;

		for (int y = startY; y < endY; ++y)
		{
			const u32 yShift3 = y >> 3;
			const u32 y0 = y & 1;
			const u32 y1 = (y & 2) >> 1;
			const u32 y2 = (y & 4) >> 2;

			for (u32 x = 0; x < width; ++x)
			{
				const u32 xShift3 = x >> 3;
				const u32 x0 = x & 1;
				const u32 x1 = (x & 2) >> 1;
				const u32 x2 = (x & 4) >> 2;

				const u32 mortonIndex =
					((x0)) |
					((y0) << 1) |
					((x1) << 2) |
					((y1) << 3) |
					((x2) << 4) |
					((y2) << 5);

				const u32 blockIndex = (yShift3 * blocksX + xShift3) << 6;
				const u32 dest = blockIndex + mortonIndex;

				const u32 srcY = width - 1 - y;
				const u32 srcIndex = x * width + srcY;

				u16 color = fb[srcIndex];
				if (color) color |= 1;
				texData[dest] = color;
			}
		}
	}


	void LogLayer::ClearNextLine() {
		ET_PROFILE_FUNCTION();

		printf("\x1b[2K");
	}
}