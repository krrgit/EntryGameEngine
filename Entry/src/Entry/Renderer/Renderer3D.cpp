#include "etpch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include <Platform/Citro3D/Citro3DShader.h>


//Shader
#include "vshader01_shbin.h"

namespace Entry {

    struct Renderer3DStorage 
    {
        Ref <VertexArray> QuadVertexArray;
        Ref <Shader> FlatColorShader;
    };

    static Renderer3DStorage* s_Data;


	void Renderer3D::Init()
	{
        s_Data = new Renderer3DStorage();
        s_Data->QuadVertexArray = VertexArray::Create();

        float squareVertices[5 * 4] =
        {
           -0.5f, -0.5f,  0.0f,
            0.5f, -0.5f,  0.0f,
            0.5f,  0.5f,  0.0f,
           -0.5f,  0.5f,  0.0f
        };

        std::shared_ptr<VertexBuffer> squareVB;
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
        });
        s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

        s_Data->FlatColorShader.reset(Shader::Create(vshader01_shbin, vshader01_shbin_size));

        u16 squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint16_t)));
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

        // Configure the first fragment shading substage to just pass through the vertex color
        // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
        C3D_TexEnv* env = C3D_GetTexEnv(0);
        C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);

        env = C3D_GetTexEnv(1);
        C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);
	}

	void Renderer3D::Shutdown()
	{
        delete s_Data;
	}

	void Renderer3D::BeginScene(const PerspectiveCamera& camera)
	{
        s_Data->FlatColorShader->Bind();
        std::static_pointer_cast<Citro3DShader>(s_Data->FlatColorShader)->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix() );
    }

	void Renderer3D::EndScene()
	{
	}

	void Renderer3D::DrawQuad(const C3D_Mtx& transform, glm::vec4& color)
	{
        s_Data->FlatColorShader->Bind();

        std::static_pointer_cast<Entry::Citro3DShader>(s_Data->FlatColorShader)->UploadUniformFloat4("u_Color", color);
        std::static_pointer_cast<Citro3DShader>(s_Data->FlatColorShader)->UploadUniformMat4("u_Transform", &transform);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer3D::DrawCube(const C3D_Mtx& transform, glm::vec4& color)
	{
        s_Data->FlatColorShader->Bind();

        std::static_pointer_cast<Entry::Citro3DShader>(s_Data->FlatColorShader)->UploadUniformFloat4("u_Color", color);
        std::static_pointer_cast<Citro3DShader>(s_Data->FlatColorShader)->UploadUniformMat4("u_Transform", &transform);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}
}
