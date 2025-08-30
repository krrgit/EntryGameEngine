#include "EditorLayer.h"
#include "imgui.h"
#include "Entry/Core/Input.h"

#include <glm/gtc/type_ptr.hpp>
#include <chrono>

namespace Entry {

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(400.0f/ 240.0f, 80.0f)
    {

    }

    void EditorLayer::OnAttach()
    {
	    ET_PROFILE_FUNCTION();

        Entry::FramebufferSpecification frameBufSpec;
        frameBufSpec.Width = 400;
        frameBufSpec.Height = 240;
        m_Framebuffer = Entry::Framebuffer::Create(frameBufSpec);

        m_ActiveScene.reset(new Scene());

        auto plane = m_ActiveScene->CreateEntity("Plane");
        plane.AddComponent<MeshRendererComponent>(Entry::Mesh::Create("assets/models/plane.obj"));

        m_ShieldEntity = m_ActiveScene->CreateEntity("Shield");
        glm::mat4 shieldTransform(1.0f);
        shieldTransform = glm::translate(shieldTransform, glm::vec3(0.0f, 2.0f, 0.0f));
        shieldTransform = glm::scale(shieldTransform, glm::vec3(0.02f, 0.02f, 0.02f));
        m_ShieldEntity.GetComponent<TransformComponent>().Transform = shieldTransform;
        m_ShieldEntity.AddComponent<MeshRendererComponent>(Entry::Mesh::Create("assets/models/shield.obj"));

        m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
        glm::mat4 camTransform(1.0f);
        camTransform = glm::translate(camTransform, glm::vec3(0.0f, 2.0f, 2.0f));
        m_CameraEntity.GetComponent<TransformComponent>().Transform = camTransform;
        auto& mainCam = m_CameraEntity.AddComponent<CameraComponent>();
        mainCam.Camera.SetViewportSize(1280.0f, 720.0f);

        m_SecondCamera = m_ActiveScene->CreateEntity("Camera Entity");
        camTransform = glm::mat4(1.0f);
        camTransform = glm::translate(camTransform, glm::vec3(3.0f, 2.0f, 2.0f));
        m_SecondCamera.GetComponent<TransformComponent>().Transform = camTransform;
        auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
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
                auto& transform = GetComponent<TransformComponent>().Transform;
                float speed = 5.0f;

                if (Input::IsKeyPressed(KeyCode::A))
                    transform[3][0] -= speed * ts;
                if (Input::IsKeyPressed(KeyCode::D))
                    transform[3][0] += speed * ts;
                if (Input::IsKeyPressed(KeyCode::W))
                    transform[3][2] -= speed * ts;
                if (Input::IsKeyPressed(KeyCode::S))
                    transform[3][2] += speed * ts;
            }
        };

        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
        m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    }

    void EditorLayer::OnDetach()
    {
	    ET_PROFILE_FUNCTION();

    }

    void EditorLayer::OnUpdate(Entry::Timestep ts, uint16_t screenSide)
    {
        ET_PROFILE_FUNCTION();

        // Resize 
        FramebufferSpecification spec = m_Framebuffer->GetSpecification();
        if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero size framebuffer is invalid 
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        // Update
        if (m_ViewportFocused) {
            ET_PROFILE_SCOPE("CameraController::OnUpdate");
            m_CameraController.OnUpdate(ts);
        }

        m_Framebuffer->Bind();

        Entry::Renderer3D::ResetStats();
        Entry::Renderer3D::SetStatsTimestep(ts);

        // Update Scene
        m_ActiveScene->OnUpdate(ts, screenSide);
        m_Framebuffer->Unbind();
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
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
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
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                //ImGui::MenuItem("Padding", NULL, &opt_padding);
                //ImGui::Separator();

                if (ImGui::MenuItem("Exit")) Entry::Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }


        ImGui::Begin("Settings");
        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

        auto stats = Entry::Renderer3D::GetStats();
        ImGui::Text("Renderer3D Stats:");
#ifdef ET_PLATFORM_3DS
        ImGui::Text("FPS: %.1f fps\nCPU: %.2f ms\nGPU: %.2f ms\n", 1000.0f / (C3D_GetProcessingTime() + C3D_GetDrawingTime()), C3D_GetProcessingTime(), C3D_GetDrawingTime()); // Temp
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS
        ImGui::Text("FPS: %.1f fps\nDeltaTime: %.2f ms\n", 1000.0f / stats.DeltaTime, stats.DeltaTime);
#endif // ET_PLATFORM_WINDOWS

        ImGui::Text("Draw Calls: %ld", stats.DrawCalls);

        ImGui::Text("Polygon Count: %ld", stats.PolygonCount);
        ImGui::Text("Vertices: %ld", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %ld", stats.GetTotalIndexCount());

        ImGui::Separator();
        if (m_ShieldEntity) {
            ImGui::Text("%s", m_ShieldEntity.GetComponent<TagComponent>().Tag.c_str());
            
            ImGui::DragFloat3("Shield Position", glm::value_ptr(m_ShieldEntity.GetComponent<TransformComponent>().Transform[3]), 0.02f);
        }
            
        if (m_CameraEntity) {
            ImGui::Text("%s", m_CameraEntity.GetComponent<TagComponent>().Tag.c_str());
            ImGui::DragFloat3("Cam Position", glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3]), 0.02f);
        }
        if (ImGui::Checkbox("Camera A", &m_PrimaryCamera)) 
        {
            m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
            m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
        }

        {
            auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
            float camFOV = camera.GetPerspectiveFOV();
            if (ImGui::DragFloat("2nd Camera FOV", &camFOV, 0.1f, 0.0f, 180.0f)) {
                camera.SetPerspectiveFOV(camFOV);
            }
        }

        ImGui::End();
    
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();

        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || m_ViewportHovered);
        
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        void* textureID = (void*)m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image(textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{0, 1}, ImVec2{1,0});
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Entry::Event& event) 
    {
	    m_CameraController.OnEvent(event);
    }
}