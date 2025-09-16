#include "SceneHierarchyPanel.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>

#include <filesystem> // C++17

namespace Entry
{
	extern const std::filesystem::path g_AssetPath;

	bool IsModelFile(const wchar_t* path)
	{
		return	wcsstr(path, L".obj") != 0 ||
			wcsstr(path, L".dae") != 0 ||
			wcsstr(path, L".gltf") != 0 ||
			wcsstr(path, L".fbx") != 0;
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Hierarchy");

		m_Context->m_Registry.each([&](ECS::Entity entityID)
		{
			Entity entity{ entityID, m_Context.get() };
			DrawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty"))
				m_Context->CreateEntity("Entity");
			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Inspector");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();

		if (m_EntityToDuplicate)
		{
			auto newEntity = m_Context->DuplicateEntity(m_EntityToDuplicate);
			m_EntityToDuplicate = {};
			m_SelectionContext = newEntity;
		}
	}
	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DeleteSelectedEntity()
	{
		if (!m_SelectionContext) return;
		m_Context->DestroyEntity(m_SelectionContext);
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 2.0f));

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		bool entityDeleted = false;
		ImGui::PopStyleVar(2);

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		//// Right-click on entity
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Duplicate"))
				m_EntityToDuplicate = entity;

			ImGui::Separator();

			if (ImGui::MenuItem("Delete"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)((uint32_t)entity + 1000), flags, tag.c_str());
			if (opened)
				ImGui::TreePop();

			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			DeleteSelectedEntity();
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0, 4 });

		ImGui::AlignTextToFramePadding();

		float localColumnWidth = columnWidth - ImGui::GetCursorPosX();


		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, localColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		float posY = ImGui::GetCursorPosY() + 3;
		ImGui::SetCursorPosY(posY);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0, 0 });

		float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight + 8.0f };

		ImGui::PushMultiItemsWidths(3, ImGui::GetContentRegionAvail().x - (buttonSize.x * 2.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0.5f });

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.88f, 0.26f, 0.36f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.98f, 0.46f, 0.56f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.88f, 0.26f, 0.36f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.5f, 0.74f, 0.26f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.7f, 0.84f, 0.46f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5f, 0.74f, 0.26f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.59f, 0.94f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.4f, 0.79f, 1.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.59f, 0.94f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar(4);
		ImGui::Columns(1);
		ImGui::PopID();
	}

	static void DrawDragnDropField(const std::string& label, std::string& value, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0, 4 });

		ImGui::AlignTextToFramePadding();

		float localColumnWidth = columnWidth - ImGui::GetCursorPosX();

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, localColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		float posY = ImGui::GetCursorPosY() + 3;
		ImGui::SetCursorPosY(posY);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0, 0 });

		float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y;
		ImVec2 buttonSize = { ImGui::GetContentRegionAvail().x, lineHeight + 8.0f };

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0.5f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });

		ImGui::PushFont(boldFont);
		ImGui::Button(value.c_str(), buttonSize);
		ImGui::PopFont();

		ImGui::PopStyleVar(4);
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string name, Entity entity, UIFunction uiFunction)
	{
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contenRegionAvailable = ImGui::GetContentRegionAvail();

			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::PopFont();
			ImGui::SameLine(contenRegionAvailable.x + 1.0f - lineHeight * 0.5f);

			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
				{
					removeComponent = true;
				}
				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		float panelWidth = ImGui::GetContentRegionAvail().x;
		float columnWidth = std::max(130.0f, panelWidth * 0.4f);

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];


		if (entity.HasComponent<TagComponent>())
		{
			//static Entity thisEntity = entity;
			//bool entityDeleted = !thisEntity.HasComponent<TagComponent>();

			//if (entityDeleted)
			//	thisEntity = entity;
			
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - GImGui->Style.FramePadding.x - 92.0f);

			ImGui::PushFont(boldFont);
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}

			//thisEntity = entity;
			ImGui::PopFont();

			ImGui::PopItemWidth();

			ImGui::SetItemTooltip("UUID: %u", entity.GetUUID());
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("+ Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				m_SelectionContext.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Mesh Renderer"))
			{
				m_SelectionContext.AddComponent<MeshRendererComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Light"))
			{
				m_SelectionContext.AddComponent<LightComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [&](TransformComponent& component)
		{
			DrawVec3Control("Position", component.Position, 0.0f, columnWidth);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation, 0.0f, columnWidth);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f, columnWidth);
		});

		DrawComponent<CameraComponent>("Camera", entity, [&](CameraComponent& component) {
			auto& camera = component.Camera;

			ImGui::Checkbox("Primary", &component.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)component.Camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; ++i)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Persepective)
			{
				float fov = camera.GetPerspectiveVerticalFOV();

				if (ImGui::DragFloat("FOV", &fov, 0.1f, 1.0f, 179.0f))
				{
					camera.SetPerspectiveVerticalFOV(fov);
				}

				float nearClip = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f, 0.0f))
				{
					camera.SetPerspectiveNearClip(nearClip);
				}

				float farClip = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far Clip", &farClip, 0.1f, 0.0f))
				{
					camera.SetPerspectiveFarClip(farClip);
				}
			}
			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float size = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &size, 0.1f))
				{
					camera.SetOrthographicSize(size);
				}

				float nearClip = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f, 0.0f))
				{
					camera.SetOrthographicNearClip(nearClip);
				}

				float farClip = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far Clip", &farClip, 0.1f, 0.0f))
				{
					camera.SetOrthographicFarClip(farClip);
				}
			}
		});

		DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, [&](MeshRendererComponent& component)
		{
			auto& mesh = component.mesh;
			std::string meshName = mesh != nullptr ? component.mesh->Name.c_str() : "None";
			std::string modelPath = component.model ? component.model->GetFilePath().c_str() : "None";

			DrawDragnDropField("Model", modelPath, columnWidth);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					if (IsModelFile(path))
					{
						modelPath = (std::filesystem::path(g_AssetPath) / path).string();
						LoadMeshInMRC(modelPath, 0, component); // TODO: Set properly when meshes are displayed in ContentBrowserPanel
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Columns(1);

			//DrawDragnDropField("Mesh", meshName, columnWidth);
			int meshID = component.mesh ? component.mesh->MeshID : 0;
			int maxID = component.model ? component.model->GetMeshes().size() - 1 : 0;
			if (ImGui::DragInt("MeshID", &meshID, 0.1f, 0, maxID, "%d", ImGuiSliderFlags_ClampZeroRange | ImGuiSliderFlags_ClampOnInput) && component.model)
			{
				component.mesh = component.model->GetMesh(meshID);
				component.material = component.model->GetMaterial(component.mesh->MaterialID);
			}

			ImGui::Columns(1); // Reset after DrawDragnDropField()

			std::string label = "Material";
			std::string mtlName = component.material ? component.material->GetProps().Name : "None";
			DrawDragnDropField(label, mtlName, columnWidth);
			ImGui::Columns(1);

			ImGui::Separator();

			const char* shaderProgramStrings[] = { "Lit", "Unlit" }; // TODO: Fix to accomodate more shaders
			const char* currentShaderProgramString = component.material ? shaderProgramStrings[(int)component.material->GetShader()] : "None";
			if (ImGui::BeginCombo("Shader", currentShaderProgramString))
			{
				for (int i = 0; i < 2; ++i)
				{
					bool isSelected = currentShaderProgramString == shaderProgramStrings[i];
					if (ImGui::Selectable(shaderProgramStrings[i], isSelected))
					{
						currentShaderProgramString = shaderProgramStrings[i];
						component.material->SetShader((ShaderProgramEnum)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			std::string texName = component.material ? component.material->GetProps().DiffuseMap->GetName() : "None";
			DrawDragnDropField("Diffuse Map", texName, columnWidth);
			ImGui::Columns(1); // Reset after DrawDragnDropField()
			
			ImGui::Separator();
			ImGui::PushFont(boldFont);
			ImGui::Text("Texture Environment");
			ImGui::PopFont();
			if (component.material)
			{
				auto& te = component.material->GetTexEnvProps();

				const char* channels[] = {"RGB", "Alpha", "", "RGBA"};
				const char* blendModes[] = { "Replace", "Modulate", "Add", "Signed Add", "Interpolat", "Subtract", "Dot3 RGB", "Dot3 RGBA", "Multiply Add", "Add Multiply"};
				const char* sources[] = {"Primary Color", "Fragment Primary Color", "Fragment Seocndary Color", "Texture0", "Texture1", "Texture2", "Texture3", "Previous Buffer", "Constant", "Previous"};

				const char* c = channels[(int)te.Channels];
				const char* bm = blendModes[(int)te.BlendMode];
				const char* s1 = sources[(int)te.Source1];
				const char* s2 = sources[(int)te.Source2];
				const char* s3 = sources[(int)te.Source3];
				ImGui::Text("Channels: %s", c);
				ImGui::Text("Blend Mode: %s", bm);
				ImGui::Text("Source 1: %s", s1);
				ImGui::Text("Source 2: %s", s2);
				ImGui::Text("Source 3: %s", s3);
			}


		});

		DrawComponent<LightComponent>("Light", entity, [&](LightComponent& component)
		{
			ImGui::Text("Type: %d", component.Type);
			if (component.Type != LightType::ET_Spotlight)
				ImGui::DragFloat("Strength",&component.Strength);
			else
				ImGui::DragFloat("Angle", &component.Angle);

			ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
		});
	}
	void SceneHierarchyPanel::LoadMeshInMRC(std::string& filepath, int meshID, MeshRendererComponent& component)
	{
		std::ifstream file(filepath.c_str());
		if (file.good())
		{
			Ref<Model> model = Model::Create(filepath);
			auto mesh = model->GetMesh(meshID);

			component.model = model;
			component.mesh = mesh; // TODO: only allow dragging of meshes not models
			component.material = model->GetMaterial(mesh->MaterialID);
		}
		else
		{
			auto errorMsg = filepath + " does not exist!";
			ET_CORE_ERROR(errorMsg);
		}
	}
}