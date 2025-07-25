#include "etpch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

//Shader
#include "vshader01_shbin.h"

namespace Entry {

    struct Renderer3DStorage 
    {
        Ref <VertexArray> QuadVertexArray;
        Ref <VertexArray> CubeVertexArray;
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

        // CUBE
        s_Data->CubeVertexArray = VertexArray::Create();

        float cubeVertices[3* 4 * 6] = {
            // Back face
           -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
           -0.5f,  0.5f,  0.5f,

           // Front face
           -0.5f, -0.5f, -0.5f,
           -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,

            // Top face
            -0.5f,  0.5f, -0.5f, 
            -0.5f,  0.5f,  0.5f, 
             0.5f,  0.5f,  0.5f, 
             0.5f,  0.5f, -0.5f, 

           // Bottom face
           -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
           -0.5f, -0.5f,  0.5f,

           // Left face
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,

            // Right face
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f
         };

        std::shared_ptr<VertexBuffer> cubeVB;
        cubeVB.reset(VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));

        cubeVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
            });
        s_Data->CubeVertexArray->AddVertexBuffer(cubeVB);

        u16 cubeIndices[] = {
            0, 1, 2, 0, 2, 3,    // back
            4, 5, 6, 4, 6, 7,    // front
            8, 9, 10, 8, 10, 11,   // top
            12, 13, 14, 12, 14, 15,   // bottom
            16, 17, 18, 16, 18, 19,   // left
            20, 21, 22, 20, 22, 23    // right
        };
        std::shared_ptr<IndexBuffer> cubeIB;
        cubeIB.reset(IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint16_t)));
        s_Data->CubeVertexArray->SetIndexBuffer(cubeIB);

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
        s_Data->FlatColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix() );
    }

	void Renderer3D::EndScene()
	{
	}

	void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec3& size, glm::vec4& color)
	{
        s_Data->FlatColorShader->Bind();
        s_Data->FlatColorShader->SetFloat4("u_Color", color);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);
        s_Data->FlatColorShader->SetMat4("u_Transform", transform);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, glm::vec4& color)
	{
        s_Data->FlatColorShader->Bind();
        s_Data->FlatColorShader->SetFloat4("u_Color", color);
        
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);
        s_Data->FlatColorShader->SetMat4("u_Transform", transform);

        s_Data->CubeVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->CubeVertexArray);
	}
}
