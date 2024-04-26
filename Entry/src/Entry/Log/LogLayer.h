#pragma once

#include "Entry/Layer.h"
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Entry {
	class LogLayer : public Layer {
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& event) override;

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		static std::shared_ptr<PrintConsole>& GetPrintConsole() { return s_PrintConsole; }
	private:
		void Initspdlog();
	private:
		PrintConsole* m_Console;
		u16 m_Width, m_Height;
		C2D_Image image;
		u16* m_ConsoleBuffer;

		bool showLogs;

		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::shared_ptr<PrintConsole> s_PrintConsole;
	};
}