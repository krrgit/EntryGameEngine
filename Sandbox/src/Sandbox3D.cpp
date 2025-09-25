#include "Sandbox3D.h"
#include "imgui.h"
#include "Entry/Core/Input.h"
#include "Entry/Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>
#include <chrono>

#include "Entry/Scene/SceneSerializer.h"

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

    m_ActiveScene.reset(new Entry::Scene());
    
#if 0
    auto plane = m_ActiveScene->CreateEntity("Plane");
    plane.AddComponent<Entry::MeshRendererComponent>(Entry::Mesh::Create("assets/models/plane.obj"));

    m_ShieldEntity = m_ActiveScene->CreateEntity("Shield");
    m_ShieldEntity.GetComponent<Entry::TransformComponent>().Position = glm::vec3(0.0f, 2.0f, 0.0f);
    m_ShieldEntity.GetComponent<Entry::TransformComponent>().Scale = glm::vec3(0.02f, 0.02f, 0.02f);
    m_ShieldEntity.AddComponent<Entry::MeshRendererComponent>(Entry::Mesh::Create("assets/models/shield.obj"));

    m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
    m_CameraEntity.GetComponent<Entry::TransformComponent>().Position = glm::vec3(0.0f, 2.0f, 10.0f);
    auto& mainCam = m_CameraEntity.AddComponent<Entry::CameraComponent>();
    mainCam.Camera.SetViewportSize(400.0f, 240.0f);

    m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
    m_SecondCamera.GetComponent<Entry::TransformComponent>().Position = glm::vec3(2.0f, 2.0f, 5.0f);
    auto& cc = m_SecondCamera.AddComponent<Entry::CameraComponent>();
    cc.Camera.SetViewportSize(400.0f, 240.0f);
    cc.Primary = false;

    class CameraController : public Entry::ScriptableEntity
    {
    public:
        void OnCreate()
        {
            printf("CameraController::OnCreate\n");
        }

        void OnDestroy()
        {
        }

        void OnUpdate(Entry::Timestep ts)
        {
            auto& position = GetComponent<Entry::TransformComponent>().Position;
            float speed = 5.0f;

            if (Entry::Input::GetButton(Entry::KeyCode::PAD_DLEFT))
                position.x -= speed * ts;
            if (Entry::Input::GetButton(Entry::KeyCode::PAD_DRIGHT))
                position.x += speed * ts;
            if (Entry::Input::GetButton(Entry::KeyCode::PAD_DUP))
                position.z -= speed * ts;
            if (Entry::Input::GetButton(Entry::KeyCode::PAD_DDOWN))
                position.z += speed * ts;
        }
    };

    m_CameraEntity.AddComponent<Entry::NativeScriptComponent>().Bind<CameraController>();
#endif

    Entry::SceneSerializer serializer(m_ActiveScene);
    serializer.Deserialize("romfs:/assets/scenes/MaxLightTest.entry");

    m_ActiveScene->OnViewportResize(400, 240);

    m_CameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
    m_LightEntity = m_ActiveScene->GetLightEntity();
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
    // auto clip = m_CameraController.GetCamera().GetViewMatrix() * m_LightPosition;

    //lightVec.x = clip.x;
    //lightVec.y = clip.y;
    //lightVec.z = clip.z;
    //C3D_LightPosition(&light, &lightVec);

    Entry::Renderer3D::ResetStats();
    Entry::Renderer3D::SetStatsTimestep(ts);

    m_Rotation += ts.GetSeconds();
    m_Rotation = m_Rotation > 6.28f ? 0 : m_Rotation;

    // Update Scene
    m_ActiveScene->OnUpdateRuntime(ts, screenSide);
}

void Sandbox3D::OnImGuiRender() 
{
    ET_PROFILE_FUNCTION();
 
    ImGui::Begin("Stats");

    auto& lightTransform = m_LightEntity.GetComponent<Entry::TransformComponent>();
    auto& light = m_LightEntity.GetComponent<Entry::LightComponent>();

    ImGui::DragFloat3("Light Position", glm::value_ptr(lightTransform.Position));
    ImGui::ColorEdit3("Light Color", glm::value_ptr(light.Color));

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