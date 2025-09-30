#include "etpch.h"
#include "ContentBrowserPanel.h"
#include "Entry/Renderer/Material.h"
#include <imgui.h>


namespace Entry
{
	// TODO: Once we have projects, change this
	extern const std::filesystem::path g_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(g_AssetPath)
	{
		m_DirectoryIcon = Ref<Texture2D>(Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png"));
		m_FileIcon = Ref<Texture2D>(Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png"));
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Begin("Project");

		// Header
		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		// Body
		static float padding = 6.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		columnCount = columnCount < 1 ? 1 : columnCount;

		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::Columns(columnCount, 0, false);

			for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				auto relativePath = std::filesystem::relative(path, g_AssetPath);
				std::string filenameString = relativePath.filename().string();

				Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(path.string().c_str(), (ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

				if (ImGui::BeginDragDropSource())
				{
					const wchar_t* itemPath = relativePath.c_str();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Always);
					ImGui::EndDragDropSource();
				}

				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (directoryEntry.is_directory())
						m_CurrentDirectory /= path.filename();
				}

				// Center Text
				ImVec2 textSize = ImGui::CalcTextSize(filenameString.c_str(),0,false, thumbnailSize);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (thumbnailSize - textSize.x) * 0.5f);

				ImGui::TextWrapped(filenameString.c_str());

				ImGui::NextColumn();
			}

			ImGui::Columns(1);

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::MenuItem("Material"))
						CreateNewMaterial();

					ImGui::EndMenu();
				}
				ImGui::EndPopup();
			}
		}
		ImGui::EndChild();

		// Footer
		ImGui::Separator();
		ImGui::SetCursorPosX(panelWidth * 3.0f / 4.0f);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("sm");
		ImGui::SameLine();
		ImGui::PushItemWidth((panelWidth / 4.0f) - 50.0f);
		ImGui::SliderFloat("##thumbnailSize", &thumbnailSize, 32, 128,"");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("lrg");


        ImGui::End();
	}

	void ContentBrowserPanel::CreateNewMaterial()
	{
		// TODO
	}

}