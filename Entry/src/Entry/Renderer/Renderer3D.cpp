#include "etpch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

//Shader
// #include "vshader01_shbin.h" // Flat Color Shader
#include "vshader02_shbin.h" // Texture Shader

namespace Entry {

    struct Renderer3DStorage 
    {
        Ref <VertexArray> QuadVertexArray;
        Ref <VertexArray> CubeVertexArray;
        Ref <Shader> TextureShader;
        Ref <Texture2D> WhiteTexture;
    };

    static Renderer3DStorage* s_Data;


	void Renderer3D::Init()
	{
        ET_PROFILE_FUNCTION();

        s_Data = new Renderer3DStorage();

        // QUAD
        s_Data->QuadVertexArray = VertexArray::Create();

        float squareVertices[5 * 4] =
        {
           -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
            0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
            0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
           -0.5f,  0.5f,  0.0f, 0.0f, 1.0f
        };

        std::shared_ptr<VertexBuffer> squareVB;
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" }
        });
        s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

        u16 squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint16_t)));
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB); 

        // CUBE
        s_Data->CubeVertexArray = VertexArray::Create();

        // TODO: FIX TEX COORDS (maybe?)
        float cubeVertices[5* 4 * 6] = {
            // Front face
           -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
            0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
            0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
           -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

           // Back face
           -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
           -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
            0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            // Top face
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,

           // Bottom face
           -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, -0.5f,  0.5f, 1.0f, 1.0f,
           -0.5f, -0.5f,  0.5f, 0.0f, 1.0f,

           // Right face
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
            0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
            0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

            // Left face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f
         };

        std::shared_ptr<VertexBuffer> cubeVB;
        cubeVB.reset(VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));

        cubeVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" }
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

        
        // SHADERS
        s_Data->TextureShader.reset(Shader::Create(vshader02_shbin, vshader02_shbin_size));
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetInt("u_Texture", 0);

        // WHITE TEXTURE
        {
            ET_PROFILE_SCOPE("Create 8x8 White Texture");
            // Minimum texture size is 8 x 8 for C3D. Anything smaller doesn't show up.
            s_Data->WhiteTexture = Texture2D::Create(8, 8); 
            uint32_t whiteTextureData[8 * 8];
            std::fill_n(&whiteTextureData[0], 8 * 8, 0xffffffff);
            s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
        }

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
        ET_PROFILE_FUNCTION();

        delete s_Data;
	}

	void Renderer3D::BeginScene(const PerspectiveCamera& camera)
	{
        ET_PROFILE_FUNCTION();

        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
    }

	void Renderer3D::EndScene()
	{
        ET_PROFILE_FUNCTION();

	}

	void Renderer3D::DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color)
	{
        ET_PROFILE_FUNCTION();

        s_Data->TextureShader->SetFloat4("u_Color", color);
        s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0f);
        s_Data->WhiteTexture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        s_Data->TextureShader->SetMat4("u_Transform", transform);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color)
	{
        ET_PROFILE_FUNCTION();

        s_Data->TextureShader->SetFloat4("u_Color", color);
        s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0f);
        s_Data->WhiteTexture->Bind();
        
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        s_Data->TextureShader->SetMat4("u_Transform", transform);

        s_Data->CubeVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->CubeVertexArray);
	}


    void Renderer3D::DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        ET_PROFILE_FUNCTION();

        s_Data->TextureShader->SetFloat4("u_Color", tintColor);
        s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);
        texture->Bind(); 

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        s_Data->TextureShader->SetMat4("u_Transform", transform);


        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }
    
    void Renderer3D::DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        ET_PROFILE_FUNCTION();

        s_Data->TextureShader->SetFloat4("u_Color", tintColor);
        s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);
        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        s_Data->TextureShader->SetMat4("u_Transform", transform);

        s_Data->CubeVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->CubeVertexArray);
    }
}
