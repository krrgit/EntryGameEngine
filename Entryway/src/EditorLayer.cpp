#include "EditorLayer.h"
#include "imgui.h"
#include "Entry/Core/Input.h"

#include <glm/gtc/type_ptr.hpp>
#include <chrono>

#include "Entry/Scene/SceneSerializer.h"
#include "Entry/Utils/PlatformUtils.h"

#include <ImGuizmo.h>

#include "Entry/Math/MathUtils.h"

namespace Entry {

    // TODO: Once we have projects, change this
    extern const std::filesystem::path g_AssetPath;

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(400.0f/ 240.0f, 80.0f)
    {

    }

    void EditorLayer::OnAttach()
    {
	    ET_PROFILE_FUNCTION();

        m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
        m_IconPause = Texture2D::Create("Resources/Icons/PauseButton.png");
        m_IconAdvance = Texture2D::Create("Resources/Icons/StepButton.png");

        FramebufferSpecification sceneFrameBufSpec;
        sceneFrameBufSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
        sceneFrameBufSpec.Width = 400;
        sceneFrameBufSpec.Height = 240;
        m_SceneFramebuffer = Entry::Framebuffer::Create(sceneFrameBufSpec);

        FramebufferSpecification gameFrameBufSpec;
        gameFrameBufSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
        gameFrameBufSpec.Width = 400;
        gameFrameBufSpec.Height = 240;
        m_GameFramebuffer = Entry::Framebuffer::Create(sceneFrameBufSpec);

        m_ActiveScene.reset(new Scene());

        m_EditorCamera = EditorCamera(80.0f, 1.778f, 0.01f, 1000.0f);

#if 0

        auto plane = m_ActiveScene->CreateEntity("Plane");
        plane.AddComponent<MeshRendererComponent>(Entry::Model::Create("assets/models/plane.obj"));

        m_ShieldEntity = m_ActiveScene->CreateEntity("Shield");
        m_ShieldEntity.GetComponent<TransformComponent>().Position = glm::vec3(0.0f, 2.0f, 0.0f);
        m_ShieldEntity.GetComponent<TransformComponent>().Scale = glm::vec3(0.02f, 0.02f, 0.02f);
        m_ShieldEntity.AddComponent<MeshRendererComponent>(Entry::Model::Create("assets/models/shield.obj"));

        m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
        m_CameraEntity.GetComponent<TransformComponent>().Position = glm::vec3(0.0f, 2.0f, 10.0f);
        auto& mainCam = m_CameraEntity.AddComponent<CameraComponent>();
        mainCam.Camera.SetViewportSize(1280, 720);

        m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
        m_SecondCamera.GetComponent<TransformComponent>().Position = glm::vec3( 2.0f, 2.0f, 5.0f);
        auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
        cc.Camera.SetViewportSize(1280, 720);
        cc.Primary = false;

        class CameraController : public ScriptableEntity
        {
        public:
            void OnCreate() 
            {
            }

            void OnDestroy() 
            {
            }

            void OnUpdate(Timestep ts) 
            {
                auto& position = GetComponent<TransformComponent>().Position;
                float speed = 5.0f;

                if (Input::IsKeyPressed(KeyCode::A))
                    position.x -= speed * ts;
                if (Input::IsKeyPressed(KeyCode::D))
                    position.x += speed * ts;
                if (Input::IsKeyPressed(KeyCode::W))
                    position.z -= speed * ts;
                if (Input::IsKeyPressed(KeyCode::S))
                    position.z += speed * ts;
            }
        };

        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
        m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDetach()
    {
	    ET_PROFILE_FUNCTION();

    }

    void EditorLayer::OnUpdate(Entry::Timestep ts, uint16_t screenSide)
    {
        ET_PROFILE_FUNCTION();

        // Resize 
        FramebufferSpecification sceneSpec = m_SceneFramebuffer->GetSpecification();
        if (m_SceneViewportSize.x > 0.0f && m_SceneViewportSize.y > 0.0f && // zero size framebuffer is invalid 
            (sceneSpec.Width != m_SceneViewportSize.x || sceneSpec.Height != m_SceneViewportSize.y))
        {
            m_SceneFramebuffer->Resize((uint32_t)m_SceneViewportSize.x, (uint32_t)m_SceneViewportSize.y);
            m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);
            //m_CameraController.OnResize(m_SceneViewportSize.x, m_SceneViewportSize.y);
            //m_ActiveScene->OnViewportResize((uint32_t)m_SceneViewportSize.x, (uint32_t)m_SceneViewportSize.y);
        }

        FramebufferSpecification gameSpec = m_GameFramebuffer->GetSpecification();
        if (m_GameViewportSize.x > 0.0f && m_GameViewportSize.y > 0.0f && // zero size framebuffer is invalid 
            (gameSpec.Width != m_GameViewportSize.x || gameSpec.Height != m_GameViewportSize.y))
        {
            //m_CameraController.OnResize(m_SceneViewportSize.x, m_SceneViewportSize.y);
            m_GameFramebuffer->Resize((uint32_t)m_GameViewportSize.x, (uint32_t)m_GameViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_GameViewportSize.x, (uint32_t)m_GameViewportSize.y);
        }

        m_EditorCamera.OnUpdate(ts);

        Entry::Renderer3D::ResetStats();
        Entry::Renderer3D::SetStatsTimestep(ts);

        m_SceneFramebuffer->Bind();
        RenderCommand::SetClearColor(0x68B0D8FF);
        RenderCommand::Clear();

        // Clear our entity ID attachment to -1
        m_SceneFramebuffer->ClearAttachment(1, -1);

        // Update Scene
        m_ActiveScene->OnUpdateEditor(ts, screenSide, m_EditorCamera);

        ImVec2 mousePos = ImGui::GetMousePos();
        mousePos.x -= m_ViewportBounds[0].x;
        mousePos.y -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        mousePos.y = viewportSize.y - mousePos.y;

        int mouseX = (int)mousePos.x;
        int mouseY = (int)mousePos.y;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
           int pixelData = m_SceneFramebuffer->ReadPixel(1, mouseX, mouseY);
           m_HoveredEntity = pixelData <= -1 ? Entity() : Entity((ECS::Entity)pixelData, m_ActiveScene.get());
        }

        m_SceneFramebuffer->Unbind();

        m_GameFramebuffer->Bind();
        RenderCommand::SetClearColor(0x68B0D8FF);
        RenderCommand::Clear();
        
        switch (m_SceneState)
        {
        case SceneState::Edit:
            m_ActiveScene->OnUpdateEditorInGame(ts, screenSide);
            break;
        case SceneState::Play:
            m_ActiveScene->OnUpdateRuntime(ts, screenSide); // TODO: Fix when play button is properly setup
            break;
        }

        m_GameFramebuffer->Unbind();
    }

    void EditorLayer::OnImGuiRender()
    {
        ET_PROFILE_FUNCTION();

        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos({ viewport->WorkPos.x, viewport->WorkPos.y});
            ImGui::SetNextWindowSize({ viewport->WorkSize.x, viewport->WorkSize.y});
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 340.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::SetCursorPosY(52.0f); // Reserve Space for Toolbar 
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
                    OpenScene();

                ImGui::Separator();

                if (ImGui::MenuItem("Save", "Ctrl + S"))
                    SaveScene();

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                ImGui::Separator();

                if (ImGui::MenuItem("Exit")) Entry::Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();

        ImGui::Begin("Stats"); // BEGIN: Stats Panel

        //std::string name = "None";
        //if (m_HoveredEntity)
        //{
        //    name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
        //}
        //ImGui::Text("Hovered Entity: %s", name.c_str());


        auto stats = Entry::Renderer3D::GetStats();

        std::string hovered = m_HoveredEntity ? m_HoveredEntity.GetName() : "None";
        ImGui::Text("Hovered:, %s", hovered.c_str());
        
        ImGui::Text("FPS: %.1f fps\nDeltaTime: %.2f ms\n", 1000.0f / stats.DeltaTime, stats.DeltaTime);


        ImGui::Text("Draw Calls: %ld", stats.DrawCalls);

        ImGui::Text("Polygon Count: %ld", stats.PolygonCount);
        ImGui::Text("Vertices: %ld", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %ld", stats.GetTotalIndexCount());

        ImGui::End(); // END: Stats Panel

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
        ImGui::Begin("Scene"); // BEGIN: Scene Panel

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
        
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_SceneViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        void* textureID = (void*)m_SceneFramebuffer->GetColorAttachmentRendererID(0);
        ImGui::Image(textureID, ImVec2{ m_SceneViewportSize.x, m_SceneViewportSize.y }, ImVec2{0, 1}, ImVec2{1,0});

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                const wchar_t* path = (const wchar_t*)payload->Data;
                if (wcsstr(path, L".entry") != 0)
                {
                    OpenScene(std::filesystem::path(g_AssetPath) / path);
                } 
                // TODO: Add other file types here
            }
            ImGui::EndDragDropTarget();
        }

        // Gizmos
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_GizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            // Camera
            //auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
            //const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
            //glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
            //const glm::mat4& cameraProjection = camera.GetProjection();

            // Editor Camera
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();
            const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();

            //Entity Transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4& transform = tc.GetTransform();

            // Snapping
            bool snap = Input::IsKeyPressed(KeyCode::LeftControl);
            float snapValue = m_GizmoType != ImGuizmo::OPERATION::ROTATE ? 0.5f : 45.0f; // 0.5f for Position/Scale; 45 degrees for Rotation.

            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                nullptr, snap ? snapValues : nullptr);

            // Disable when moving editor camera
            bool MoveEditorCameraLeftClick = (Input::IsKeyPressed(Key::LeftAlt) && Input::IsMouseButtonPressed(MouseCode::Button0));
            ImGuizmo::Enable(!MoveEditorCameraLeftClick);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 position, rotation, scale;
                MathUtils::DecomposeTransform(transform, position, rotation, scale);

                glm::vec3 deltaRotation = rotation - tc.Rotation;
                tc.Position = position;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }
        ImGui::End(); // END: Scene Panel
        ImGui::PopStyleVar();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
        ImGui::Begin("Game [Top Screen]"); // BEGIN: Game [Top Screen] Panel

        ImVec2 gameViewportPanelSize = ImGui::GetContentRegionAvail();
        static float cameraAspectRatio = 400.0f / 240.0f;
        float panelAspectRatio = gameViewportPanelSize.x / gameViewportPanelSize.y;

        m_GameViewportSize = (cameraAspectRatio > panelAspectRatio) ?
            glm::vec2{ gameViewportPanelSize.x, gameViewportPanelSize.x / cameraAspectRatio } :
            glm::vec2{ gameViewportPanelSize.y * cameraAspectRatio, gameViewportPanelSize.y };


        void* gameTextureID = (void*)m_GameFramebuffer->GetColorAttachmentRendererID(0);
        ImGui::SetCursorPosX((gameViewportPanelSize.x - m_GameViewportSize.x) * 0.5f); // Center Horizontally
        ImGui::Image(gameTextureID, ImVec2{ m_GameViewportSize.x, m_GameViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1,0 });
        ImGui::End(); // Game [Top Screen] Panel
        ImGui::PopStyleVar();
        
        UI_Toolbar();

        ImGui::End();
    }


    void EditorLayer::UI_Toolbar()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos({ viewport->WorkPos.x, viewport->WorkPos.y + 20.0f });
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowSize({ viewport->WorkSize .x , 30.0f});
        ImGui::SetNextWindowSizeConstraints({ viewport->WorkSize.x , 30.0f }, { viewport->WorkSize.x , 30.0f });
        //ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        window_flags |= ImGuiWindowFlags_NoDocking;


        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

        ImGui::Begin("##Toolbar", nullptr, window_flags);
        
        float buttonSize = 20.0f;
        Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconPause;
        ImVec4 activeButtonColor = ImVec4(0.24f, 0.52f, 0.88f, 1.0f);
        
        auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = m_SceneState == SceneState::Edit ? colors[ImGuiCol_ButtonHovered] : activeButtonColor;
        const auto& buttonActive = m_SceneState == SceneState::Edit ? colors[ImGuiCol_ButtonActive] : activeButtonColor;
        const auto& button = m_SceneState == SceneState::Edit ? colors[ImGuiCol_Button] : activeButtonColor;

        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (buttonSize * 0.5f));
        ImGui::SetCursorPosY((ImGui::GetWindowContentRegionMax().y * 0.5f) - (buttonSize * 0.5f));

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(button.x, button.y, button.z, 0.5f));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 1));

        if (ImGui::ImageButton("##Play Button", icon->GetRendererID(), { buttonSize, buttonSize }, ImVec2(0,0), ImVec2(1,1)))
        {
            if (m_SceneState == SceneState::Edit)
                OnScenePlay();
            else if (m_SceneState == SceneState::Play)
                OnSceneStop();
        }
        
        ImGui::PopStyleVar(); // Button Style
        ImGui::PopStyleColor(3); // Button Color

        ImGui::PopStyleVar(4); // Toolbar Style
        ImGui::End();

    }

    void EditorLayer::OnEvent(Entry::Event& event) 
    {
	    m_CameraController.OnEvent(event);
        m_EditorCamera.OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(ET_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(ET_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }
    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.GetRepeatCount() > 0) { return false; }

        bool ctrlPressed = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shiftPressed = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

        // Shortcuts
        switch (e.GetKeyCode())
        {
        case KeyCode::N:
            if (ctrlPressed) 
                NewScene();
            break; 
        case KeyCode::O:
            if (ctrlPressed) 
                OpenScene();
            break;
        case KeyCode::S:
            if (ctrlPressed)
            {
                if (shiftPressed) 
                    SaveSceneAs();
                else 
                    SaveScene();
            }
            break;
        // Scene Commands
        case KeyCode::D:
            if (ctrlPressed)
                OnDuplicateEntity();
            break;
        case KeyCode::Delete:
            m_HoveredEntity = m_SceneHierarchyPanel.GetSelectedEntity() == m_HoveredEntity ? Entity() : m_HoveredEntity;
            ImGuizmo::SetNotOver();
            m_SceneHierarchyPanel.DeleteSelectedEntity();
            break;
        default:
            break;
        }

        if (m_ViewportFocused || m_ViewportHovered)
        {
            // Gizmos
            switch (e.GetKeyCode()) 
            {
                case KeyCode::Q:
                m_GizmoType = -1;
                break;
                case KeyCode::W:
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
                case KeyCode::E:
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
                case KeyCode::R:
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
            default:
            break;
            }
        }
        return false;
    }
    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == MouseCode::Button0)
        {
            
            if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
            m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
        }
        return false;
    }
    void EditorLayer::NewScene()
    {
        m_ActiveScene.reset(new Scene());
        m_SceneFramebuffer->Resize((uint32_t)m_SceneViewportSize.x, (uint32_t)m_SceneViewportSize.y);
        m_ActiveScene->OnViewportResize((uint32_t)m_SceneViewportSize.x, (uint32_t)m_SceneViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        m_EditorScenePath = std::filesystem::path();
    }
    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("Entry Scene (*.entry)\0*.entry\0");
        if (!filepath.empty())
        {
            OpenScene(filepath);
        }
    }
    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        if (m_SceneState != SceneState::Edit)
            OnSceneStop();

        Ref<Scene> newScene;
        newScene.reset(new Scene());

        SceneSerializer serializer(newScene);
        
        if (serializer.Deserialize(path.string()))
        {
            m_EditorScene = newScene;
            m_EditorScene->OnViewportResize((uint32_t)m_SceneViewportSize.x, (uint32_t)m_SceneViewportSize.y);
            m_EditorScenePath = path;

            m_ActiveScene = m_EditorScene;
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        }
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("Entry Scene (*.entry)\0*.entry\0");
        if (!filepath.empty())
        {
            SerializeScene(m_ActiveScene, m_EditorScenePath);
            m_EditorScenePath = filepath;
        }

    }

    void EditorLayer::SaveScene()
    {
        if (!m_EditorScenePath.empty())
            SerializeScene(m_ActiveScene, m_EditorScenePath);
        else
            SaveSceneAs();
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
    {
        SceneSerializer serializer(scene);
        serializer.Serialize(path.string());
    }

    void EditorLayer::OnScenePlay()
    {
        m_SceneState = SceneState::Play;
        m_ActiveScene = Scene::Copy(m_EditorScene);
        //m_RuntimeScene->OnRuntimeStart(); // TODO
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnSceneStop()
    {
        m_SceneState = SceneState::Edit;
        //m_ActiveScene->OnRuntimeStop(); // TODO
        m_ActiveScene = m_EditorScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDuplicateEntity()
    {
        //if (m_SceneState != SceneState::Edit) // Comment out for now...
        //    return;

        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity)
        {
            auto newEntity = m_ActiveScene->DuplicateEntity(selectedEntity);
            m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
        }
    }
}