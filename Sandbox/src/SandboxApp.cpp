#include <Entry.h>
#include <Entry/Core/EntryPoint.h>
#include <Entry/Core/Config.h>

#include <Platform/Citro3D/Citro3DShader.h>

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Sandbox3D.h"

// Compiled Shader Headers
#include "vshader00_shbin.h"
#include "vshader01_shbin.h"
#include "vshader02_shbin.h"

// Texture Headers
#include "Checkerboard_t3x.h"
#include "EntryLogo_t3x.h"


class ExampleLayer : public Entry::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1.0f)
	{
        m_Shader.reset(Entry::Shader::Create(vshader00_shbin, vshader00_shbin_size));

        m_VertexArray = Entry::VertexArray::Create();

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



        m_SquareVA = Entry::VertexArray::Create();

        float squareVertices[5 * 4] =
        {
           -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
            0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
            0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
           -0.5f,  0.5f,  0.0f, 0.0f, 1.0f
        };

        std::shared_ptr<Entry::VertexBuffer> squareVB;
        squareVB.reset(Entry::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            { Entry::ShaderDataType::Float3, "a_Position" },
            { Entry::ShaderDataType::Float2, "a_TexCoord" }
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        m_FlatColor.reset(Entry::Shader::Create(vshader01_shbin, vshader01_shbin_size));

        u16 squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<Entry::IndexBuffer> squareIB;
        squareIB.reset(Entry::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint16_t)));
        m_SquareVA->SetIndexBuffer(squareIB);



        m_FloorVA = Entry::VertexArray::Create();

        float floorVertices[4 * 3] =
        {
           5.0f, -0.75f,  5.0f,
          -5.0f, -0.75f,  5.0f,
          -5.0f, -0.75f, -5.0f,
           5.0f, -0.75f, -5.0f,
        };


        std::shared_ptr<Entry::VertexBuffer> floorVB;
        floorVB.reset(Entry::VertexBuffer::Create(floorVertices, sizeof(floorVertices)));

        floorVB->SetLayout({
            { Entry::ShaderDataType::Float3, "a_Position" }
            });
        m_FloorVA->AddVertexBuffer(floorVB);

        u16 floorIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<Entry::IndexBuffer> floorIB;
        floorIB.reset(Entry::IndexBuffer::Create(floorIndices, sizeof(floorIndices) / sizeof(uint16_t)));
        m_FloorVA->SetIndexBuffer(floorIB);


        m_TextureShader.reset(Entry::Shader::Create(vshader02_shbin, vshader02_shbin_size));
        m_Texture = Entry::Texture2D::Create(Checkerboard_t3x, Checkerboard_t3x_size);
        m_Texture->Bind();

        m_EntryLogo = Entry::Texture2D::Create(EntryLogo_t3x, EntryLogo_t3x_size);

        // Configure the first fragment shading substage to just pass through the vertex color
        // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
        C3D_TexEnv* env = C3D_GetTexEnv(0);
        C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);

        env = C3D_GetTexEnv(1);
        C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);
	}

	void OnUpdate(Entry::Timestep ts) override
	{
        //// Update
        //m_CameraController.OnUpdate(ts);

        ////Render
        //Entry::Renderer::BeginScene(m_CameraController.GetCamera());

        //C3D_Mtx transform;
        //C3D_Mtx scale;
        //Mtx_Identity(&scale);
        //Mtx_Scale(&scale, 0.1f, 0.1f, 0.1f);
        //std::static_pointer_cast<Entry::Citro3DShader>(m_FlatColor)->UploadUniformFloat4("u_Color", m_SquareColor);

        //for (int y = 0; y < 20; ++y) {
        //    for (int x = 0; x < 20; ++x) {
        //        Mtx_Identity(&transform);
        //        Mtx_Translate(&transform, x * 0.11f, y * 0.11f, 0.0f, true);
        //        Mtx_Multiply(&transform, &transform, &scale);
        //        Entry::Renderer::Submit(m_FlatColor, m_SquareVA, transform);
        //    }
        //}

        //// Ground
        //std::static_pointer_cast<Entry::Citro3DShader>(m_FlatColor)->UploadUniformFloat4("u_Color", m_floorColor);
        //Entry::Renderer::Submit(m_FlatColor, m_FloorVA);

        //// Triangle
        ////Entry::Renderer::Submit(m_Shader, m_VertexArray);

        //C3D_Mtx texturedQuadMtx;
        //Mtx_Identity(&texturedQuadMtx);
        //Mtx_Scale(&texturedQuadMtx, 1.5f, 1.5f, 1.5f);
        //Mtx_Translate(&texturedQuadMtx, 0, 0, 0.01f, true);
        //m_Texture->Bind();
        //Entry::Renderer::Submit(m_TextureShader, m_SquareVA, texturedQuadMtx);

        //Mtx_Translate(&texturedQuadMtx, 0.167f, -0.167f, 0.01f, true);
        //m_EntryLogo->Bind();
        //Entry::Renderer::Submit(m_TextureShader, m_SquareVA, texturedQuadMtx);

        //Entry::Renderer::EndScene();
	}

    virtual void OnImGuiRender() override 
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

	void OnEvent(Entry::Event& event) override
	{
        //m_CameraController.OnEvent(event);
	}

private:

    Entry::Ref<Entry::Shader> m_Shader;
    Entry::Ref<Entry::VertexArray> m_VertexArray;
    Entry::Ref<Entry::VertexBuffer> m_VertexBuffer;
    Entry::Ref<Entry::IndexBuffer> m_IndexBuffer;

    Entry::Ref<Entry::VertexArray> m_SquareVA;
    Entry::Ref<Entry::Shader> m_FlatColor, m_TextureShader;
    Entry::Ref<Entry::VertexArray> m_FloorVA;

    Entry::Ref<Entry::Texture2D> m_Texture;
    Entry::Ref<Entry::Texture2D> m_EntryLogo;


    Entry::PerspectiveCameraController m_CameraController;
    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
    glm::vec4 m_floorColor = { 0.1f, 0.1f, 0.1f, 1.0f};

};

class Sandbox : public Entry::Application
{
public:
    Sandbox()
    {
		//PushLayer(new ExampleLayer(), ET_WINDOW_TOP);
		PushLayer(new Sandbox3D(), ET_WINDOW_TOP);
#ifdef ET_LOG_ENABLED
		PushOverlay(new Entry::LogLayer(), ET_WINDOW_BOTTOM);
#endif
    }

    ~Sandbox()
    {
    }
};

Entry::Application* Entry::CreateApplication() 
{
    return new Sandbox();
}