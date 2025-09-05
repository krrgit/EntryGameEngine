#pragma once

#include <filesystem> // C++17 :c Switch to Windows API or use tinydir (if switching to c++11 for editor)

namespace Entry
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		~ContentBrowserPanel() = default;

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
	};
}