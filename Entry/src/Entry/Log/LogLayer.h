#pragma once

#include "Entry/Core/Layer.h"
#include "Entry/Core/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Entry {
	class LogLayer : public Layer {
	public:
		LogLayer() {}
		~LogLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& event) override;

	private:
		void CopyFramebufferToTexture();
	private:
		PrintConsole* m_Console;
		u16 m_Width, m_Height;
		C2D_Image image;

		bool m_ShowLogs;
		int m_LastX, m_LastY;
	};		  
}