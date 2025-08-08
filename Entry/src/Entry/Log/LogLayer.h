#pragma once

#include "Entry/Core/Layer.h"
#include "Entry/Core/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>


namespace Entry {
	class LogLayer : public Layer {
	public:
		LogLayer() {}
		~LogLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnUpdate(Timestep ts, uint16_t screenSide = 0) override;
		virtual void OnEvent(Event& event) override;

	private:
		void CopyFramebufferToTexture(int prevY, int currY);
		void ClearNextLine();
	private:
		PrintConsole* m_Console;
		u16 m_Width, m_Height;
		C2D_Image m_Image;

		C3D_Tex m_Tex;
		Tex3DS_SubTexture m_Subtex;
		float m_SubtexVerticalShift;

		bool m_ShowLogs;
		int m_PrevCursorPosX, m_PrevCursorPosY;
		bool m_ConsoleUpdated;
	};		  
}