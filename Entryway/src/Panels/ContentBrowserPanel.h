#pragma once

#include <filesystem> // C++17 :c Switch to Windows API or use tinydir (if switching to c++11 for editor)

#include "Entry/Renderer/Texture.h"

namespace Entry
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		~ContentBrowserPanel() = default;

		void OnImGuiRender();
	private:
		void CreateNewMaterial();
	private:
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};
}