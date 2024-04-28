#include <Entry.h>
#include <Platform/Citro3D/Citro3DShader.h>

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Entry::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.0f, 1.0f, -1.0f, 1.0f)
	{
        m_Shader.reset(Entry::Shader::Create(0));

        m_VertexArray.reset(Entry::VertexArray::Create());

        float vertices[] = {
            0.0f,  0.5f,  0.2f, 0.8f, 0.8f, 0.2f, 1.0f,
            -0.5f, -0.5f, 0.2f, 0.8f, 0.2f, 0.8f, 1.0f,
            0.5f,  -0.5f, 0.2f, 0.2f, 0.2f, 0.8f, 1.0f,
        };

        m_VertexBuffer.reset(Entry::VertexBuffer::Create(vertices, sizeof(vertices)));

        m_VertexBuffer->SetLayout({
            {Entry::ShaderDataType::Float3, "a_Position"},
            {Entry::ShaderDataType::Float4, "a_Color"}
            });

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        uint16_t indices[] = { 0, 1, 2 };
        m_IndexBuffer.reset(Entry::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint16_t)));

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_SquareVA.reset(Entry::VertexArray::Create());

        float squareVertices[4 * 3] =
        {
            0.5f,  0.5f,  0.0f,
           -0.5f,  0.5f,  0.0f,
           -0.5f, -0.5f,  0.0f,
            0.5f, -0.5f,  0.0f,
        };


        std::shared_ptr<Entry::VertexBuffer> squareVB;
        squareVB.reset(Entry::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            { Entry::ShaderDataType::Float3, "a_Position" }
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        m_FlatColor.reset(Entry::Shader::Create(1));

        u16 squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<Entry::IndexBuffer> squareIB;
        squareIB.reset(Entry::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint16_t)));
        m_SquareVA->SetIndexBuffer(squareIB);

        // Configure the first fragment shading substage to just pass through the vertex color
        // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
        C3D_TexEnv* env = C3D_GetTexEnv(0);
        C3D_TexEnvInit(env);
        C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);
	}

	void OnUpdate(Entry::Timestep ts) override
	{
        glm::vec2 cp = Entry::Input::GetJoystickPos();

        glm::vec3 forward = m_Camera.forward;
        forward.y = 0;
        glm::vec3 right = m_Camera.right;
        right.y = 0;
        m_CamPos = m_CamPos + forward * (cp.y * m_CameraMoveSpeed * ts) + (right * (cp.x * m_CameraMoveSpeed * ts));
        int LandR = (Entry::Input::GetButton(ET_KEY_R) ? 1 : 0) - (Entry::Input::GetButton(ET_KEY_L) ? 1 : 0);
        m_CamPos.y += LandR * m_CameraVertSpeed * ts;

        int cStickX = (Entry::Input::GetButton(ET_KEY_CSTICK_LEFT) ? 1 : 0) - (Entry::Input::GetButton(ET_KEY_CSTICK_RIGHT) ? 1 : 0);
        int cStickY = (Entry::Input::GetButton(ET_KEY_CSTICK_UP) ? 1 : 0) - (Entry::Input::GetButton(ET_KEY_CSTICK_DOWN) ? 1 : 0);

        m_CamRot = glm::vec4(m_CamRot.x + (cStickY * m_CameraRotationSpeed * ts), m_CamRot.y + (cStickX * m_CameraRotationSpeed * ts), m_CamRot.z, m_CamRot.w);

        m_Camera.SetPosition(m_CamPos);
        m_Camera.SetRotation(m_CamRot);

        Entry::Renderer::BeginScene(m_Camera);

        C3D_Mtx transform;
        C3D_Mtx scale;
        Mtx_Identity(&scale);
        Mtx_Scale(&scale, 0.1f, 0.1f, 0.1f);

        std::static_pointer_cast<Entry::Citro3DShader>(m_FlatColor)->UploadUniformFloat4("u_Color", m_SquareColor);

        for (int y = 0; y < 20; ++y) {
            for (int x = 0; x < 20; ++x) {
                Mtx_Identity(&transform);
                Mtx_Translate(&transform, x * 0.11f, y * 0.11f, 0.0f, true);
                Mtx_Multiply(&transform, &transform, &scale);
                Entry::Renderer::Submit(m_FlatColor, m_SquareVA, transform);
            }
        }

        Entry::Renderer::Submit(m_Shader, m_VertexArray);

        Entry::Renderer::EndScene();
	}

    virtual void OnImGuiRender() override 
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

	void OnEvent(Entry::Event& event) override
	{

	}

private:

    std::shared_ptr<Entry::Shader> m_Shader;
    std::shared_ptr<Entry::VertexArray> m_VertexArray;
    std::shared_ptr<Entry::VertexBuffer> m_VertexBuffer;
    std::shared_ptr<Entry::IndexBuffer> m_IndexBuffer;

    std::shared_ptr<Entry::VertexArray> m_SquareVA;
    std::shared_ptr<Entry::Shader> m_FlatColor;

    Entry::PerspectiveCamera m_Camera;
    glm::vec3 m_CamPos = { 0.0f, 0.0f, 1.0f };
    glm::vec4 m_CamRot = { 0.0f, 0.0f, 0.0f, 0.0f };
    float m_CameraMoveSpeed = 6.0f;
    float m_CameraVertSpeed = 3.0f;
    float m_CameraRotationSpeed = 120.0f;
    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f};

};

class Sandbox : public Entry::Application
{
public:
    Sandbox()
    {
		PushLayer(new ExampleLayer(), ET_WINDOW_TOP);
		PushOverlay(new Entry::LogLayer(), ET_WINDOW_BOTTOM);
    }

    ~Sandbox()
    {
    }
};

Entry::Application* Entry::CreateApplication() 
{
    return new Sandbox();
}