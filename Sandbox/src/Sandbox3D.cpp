#include "Sandbox3D.h"
#include "imgui.h"
#include "Entry/Core/Input.h"

#include <glm/gtc/type_ptr.hpp>
#include <chrono>

//#include <citro3d.h>
//static C3D_LightEnv lightEnv;
//static C3D_Light light;
//static C3D_LightLut lut_Phong;
//static C3D_FVec lightVec;

Sandbox3D::Sandbox3D()
    : Layer("Sandbox3D"), m_CameraController(400.0f/ 240.0f, 80.0f)
{

}

void Sandbox3D::OnAttach()
{
	ET_PROFILE_FUNCTION();

    m_CheckerboardTexture = Entry::Texture2D::Create("assets/textures/Checkerboard.png");
    m_Plane = Entry::Mesh::Create("assets/models/plane.obj");
    std::string meshPath = "assets/models/shield.obj";
    m_Model = Entry::Mesh::Create(meshPath);

    // TO USE LIGHTS: Set Citro3D texenv to GPU_FRAGMENT_PRIMARY_COLOR
    //static C3D_Material* material = reinterpret_cast<C3D_Material*>(&m_Model->GetMaterial(0)->GetProps().Values);
    //static const C3D_Material material =
    //{
    //    { 0.2f, 0.2f, 0.2f }, //ambient
    //    { 0.4f, 0.4f, 0.4f }, //diffuse
    //    { 0.8f, 0.8f, 0.8f }, //specular0
    //    { 0.0f, 0.0f, 0.0f }, //specular1
    //    { 0.0f, 0.0f, 0.0f }, //emission
    //};

    //C3D_LightEnvInit(&lightEnv);
    //C3D_LightEnvBind(&lightEnv);
    //C3D_LightEnvMaterial(&lightEnv, material);

    //LightLut_Phong(&lut_Phong, 30);
    //C3D_LightEnvLut(&lightEnv, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &lut_Phong);

    //lightVec = FVec4_New(0.0f, 0.0f, -0.5f, 1.0f);

    //C3D_LightInit(&light, &lightEnv);
    //C3D_LightColor(&light, 1.0, 1.0, 1.0);
    //C3D_LightPosition(&light, &lightVec);
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

    //m_LightPosition.x = std::cos(m_Rotation) * 5;
    //m_LightPosition.y = 2.0f;
    //m_LightPosition.z = std::sin(m_Rotation) * 5;
    //auto clip = m_CameraController.GetCamera().GetViewMatrix() * m_LightPosition;

    //lightVec.x = clip.x;
    //lightVec.y = clip.y;
    //lightVec.z = clip.z;
    //C3D_LightPosition(&light, &lightVec);

    Entry::Renderer3D::ResetStats();
    Entry::Renderer3D::SetStatsTimestep(ts);

    m_Rotation += ts.GetSeconds();
    m_Rotation = m_Rotation > 6.28f ? 0 : m_Rotation;

    //Render
    Entry::Renderer3D::BeginScene(m_CameraController.GetCamera(), screenSide);
    glm::vec4 teapotColor(1.0f);
    //Entry::Renderer3D::DrawQuad(glm::vec3(0.0f), glm::quat(glm::vec3(1.57f, 0.0f, 0.0f)), glm::vec3(10.0f), m_CheckerboardTexture);
    Entry::Renderer3D::DrawMesh(m_Plane, glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::vec3(0.0f)), glm::vec3(1.0f), teapotColor);
    Entry::Renderer3D::DrawMesh(m_Model, glm::vec3(0.0f, 2.0f, 0.0f), glm::quat(glm::vec3(0.0f, m_Rotation, 0.0f)), glm::vec3(0.02f), teapotColor);
    Entry::Renderer3D::EndScene();

}

void Sandbox3D::OnImGuiRender() 
{
    ET_PROFILE_FUNCTION();
 
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::SliderFloat3("Light Position", glm::value_ptr(m_LightPosition), -10.0f, 10.0f);
    
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
    
    ImGui::End();
}

void Sandbox3D::OnEvent(Entry::Event& event) 
{
	//m_CameraController.OnEvent(event);
}