#include "Sandbox3D.h"
#include "imgui.h"
#include "Entry/Core/Input.h"

#include <glm/gtc/type_ptr.hpp>
#include <chrono>

#ifdef ET_PLATFORM_3DS
// Texture Headers
#include "Checkerboard_t3x.h"
//#include "EntryLogo_t3x.h"
#endif // ET_PLATFORM_3DS


Sandbox3D::Sandbox3D()
    : Layer("Sandbox3D"), m_CameraController(1.0f)
{

}

void Sandbox3D::OnAttach()
{
	ET_PROFILE_FUNCTION();

    std::string meshPath = "monkey.obj";
#ifdef ET_PLATFORM_3DS
    m_CheckerboardTexture = Entry::Texture2D::Create(Checkerboard_t3x, Checkerboard_t3x_size);
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS
    m_CheckerboardTexture = Entry::Texture2D::Create("assets/textures/Checkerboard.png");
    meshPath = "assets/models/" + meshPath;
#endif // ET_PLATFORM_WINDOWS
    m_Teapot = Entry::Mesh::Create(meshPath);


 //   m_EntryLogoTexture = Entry::Texture2D::Create(EntryLogo_t3x, EntryLogo_t3x_size);


 //   Entry::FramebufferSpecification frameBufSpec;
 //   frameBufSpec.Width = 400;
 //   frameBufSpec.Height = 240;

 //   m_Framebuffer = Entry::Framebuffer::Create(frameBufSpec);
}

void Sandbox3D::OnDetach()
{
	ET_PROFILE_FUNCTION();

}

void Sandbox3D::OnUpdate(Entry::Timestep ts, uint16_t screenSide)
{
    ET_PROFILE_FUNCTION();
    // Update
    {
        ET_PROFILE_SCOPE("CameraController::OnUpdate");
        m_CameraController.OnUpdate(ts);
    }

    Entry::Renderer3D::ResetStats();

    m_Rotation += ts.GetSeconds();
    m_Rotation = m_Rotation > 6.28f ? 0 : m_Rotation;

    //Render
    Entry::Renderer3D::BeginScene(m_CameraController.GetCamera(), screenSide);
    glm::vec4 teapotColor(1.0f);
    Entry::Renderer3D::DrawQuad(glm::vec3(0.0f), glm::quat(glm::vec3(1.57f, 0.0f, 0.0f)), glm::vec3(10.0f), m_CheckerboardTexture);
    Entry::Renderer3D::DrawMesh(m_Teapot, glm::vec3(0.0f, 2.0f, -3.0f), glm::quat(glm::vec3(0.0f, m_Rotation, 0.0f)), glm::vec3(1.0f), teapotColor);
    Entry::Renderer3D::EndScene();
}

void Sandbox3D::OnImGuiRender() 
{
    ET_PROFILE_FUNCTION();
    
    if (!m_ShowImGui) { return; }
    
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
    //ImGui::Text("FPS: %.1f fps\nCPU: %.2f ms\nGPU: %.2f ms\n", 1000.0f / C3D_GetProcessingTime(), C3D_GetProcessingTime(), C3D_GetDrawingTime()); // Temp
    ImGui::Text("Draw Calls: %ld", stats.DrawCalls);
    
    ImGui::Text("Polygon Count: %ld", stats.PolygonCount);
    ImGui::Text("Vertices: %ld", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %ld", stats.GetTotalIndexCount());
    
    void* textureID = (void*) m_CheckerboardTexture->GetRendererID();
    ImGui::Image(textureID, ImVec2{ 128.0f, 128.0f });
    
    
    ImGui::End();
    
    ImGui::End();
}

void Sandbox3D::OnEvent(Entry::Event& event) 
{
	//m_CameraController.OnEvent(event);
}