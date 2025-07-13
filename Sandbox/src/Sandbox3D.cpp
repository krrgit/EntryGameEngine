#include "Sandbox3D.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include <Platform/Citro3D/Citro3DShader.h>

//Shader
#include "vshader01_shbin.h"

Sandbox3D::Sandbox3D()
    : Layer("Sandbox3D"), m_CameraController(1.0f)
{

}

void Sandbox3D::OnAttach()
{
    m_SquareVA = Entry::VertexArray::Create();

    float squareVertices[5 * 4] =
    {
       -0.5f, -0.5f,  0.0f,
        0.5f, -0.5f,  0.0f,
        0.5f,  0.5f,  0.0f,
       -0.5f,  0.5f,  0.0f
    };

    std::shared_ptr<Entry::VertexBuffer> squareVB;
    squareVB.reset(Entry::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

    squareVB->SetLayout({
        { Entry::ShaderDataType::Float3, "a_Position" }
        });
    m_SquareVA->AddVertexBuffer(squareVB);

    m_FlatColor.reset(Entry::Shader::Create(vshader01_shbin, vshader01_shbin_size));

    u16 squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
    std::shared_ptr<Entry::IndexBuffer> squareIB;
    squareIB.reset(Entry::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint16_t)));
    m_SquareVA->SetIndexBuffer(squareIB);

    // Configure the first fragment shading substage to just pass through the vertex color
    // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);

    env = C3D_GetTexEnv(1);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);
}

void Sandbox3D::OnDetach()
{

}

void Sandbox3D::OnUpdate(Entry::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

    //Render
    Entry::Renderer::BeginScene(m_CameraController.GetCamera());

    C3D_Mtx transform;
    C3D_Mtx scale;
    Mtx_Identity(&scale);
    m_FlatColor->Bind(); 
    std::static_pointer_cast<Entry::Citro3DShader>(m_FlatColor)->UploadUniformFloat4("u_Color", m_SquareColor);

    Mtx_Identity(&transform);
    Mtx_Multiply(&transform, &transform, &scale);
    Entry::Renderer::Submit(m_FlatColor, m_SquareVA, transform);

    Entry::Renderer::EndScene();

}

void Sandbox3D::OnImGuiRender() 
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox3D::OnEvent(Entry::Event& event) 
{
	//m_CameraController.OnEvent(event);
}