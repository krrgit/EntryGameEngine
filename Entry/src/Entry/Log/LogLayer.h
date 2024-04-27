#pragma once

#include "Entry/Layer.h"
#include "Core.h"
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
		PrintConsole* m_Console;
		u16 m_Width, m_Height;
		C2D_Image image;
		u16* m_ConsoleBuffer;

		bool showLogs;
	};
}