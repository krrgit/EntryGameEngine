#include "etpch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#ifdef ET_PLATFORM_3DS
//Shaders
// #include "vshader01_shbin.h" // Flat Color Shader
#include "vshader02_shbin.h" // Texture Shader
#include "phongshader_shbin.h" // Phong Shader
#include "normalcolorshader_shbin.h" // Normal Color Shader
#endif // ET_PLATFORM_3DS

//#include <GLFW/glfw3.h>
//#include <glad/glad.h>

namespace Entry {

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
    };

    struct RenderBatch {
        Ref <VertexArray> QuadVertexArray;
        Ref <VertexBuffer> QuadVertexBuffer;

        uint32_t IndexCount = 0;
        QuadVertex* VertexBufferBase = nullptr;
        QuadVertex* VertexBufferPtr = nullptr;

        Ref<Texture2D> BatchTexture;
    };

    struct Renderer3DData
    {
        // Total Limitations (CPU/Code bottleneck @ 60 fps)
        // Max Polygons: ~20736
        // Max Vertcies: ~41472
        // Max Indices: ~62208

        // Per Batch Limitations
        static const uint32_t MaxPolygons = 5298; // Max: 5298 (seems like CPU bottleneck)
        static const uint32_t MaxVertices = MaxPolygons * 2;
        static const uint16_t MaxIndices = MaxPolygons * 3;
        static const uint32_t MaxBatches = 16; // Randomly selected tbh
        static const uint32_t MaxTextureSlots = 4; // 3 texture units slots, 1 proceedural generated texture slot (set by PICA 200)
        bool AllowMultipleBatchesPerTexture = true;

        Ref <Shader> TextureShader;
        Ref <Shader> PhongShader;
        Ref <Shader> NormalColorShader;
        Ref <Texture2D> WhiteTexture;

        std::array<RenderBatch, MaxBatches> RenderBatches;
        uint32_t BatchSlotIndex = 1; // 0 = white texture

        uint32_t IndexCount;
        Renderer3D::Statistics Stats;

        uint32_t m_VertexArray, m_VertexBuffer, m_IndexBuffer;

    };

    static Renderer3DData s_Data;

	void Renderer3D::Init()
	{
        ET_PROFILE_FUNCTION();

        uint16_t quadIndices[s_Data.MaxIndices];

        uint16_t offset = 0;
        for (uint16_t i = 0; i < s_Data.MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        std::shared_ptr<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::Create(quadIndices, s_Data.MaxIndices));

        for (uint32_t i = 0; i < Renderer3DData::MaxBatches; ++i) {
            RenderBatch* batch = &s_Data.RenderBatches[i];

            batch->QuadVertexArray = VertexArray::Create();

            batch->QuadVertexBuffer.reset(VertexBuffer::Create(sizeof(QuadVertex) * s_Data.MaxVertices));
            batch->QuadVertexBuffer->SetLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float4, "a_Color" },
                { ShaderDataType::Float2, "a_TexCoord" }
                });
            batch->QuadVertexArray->AddVertexBuffer(batch->QuadVertexBuffer);
            batch->VertexBufferBase = new QuadVertex[s_Data.MaxVertices];
            batch->QuadVertexBuffer->Bind();

            batch->QuadVertexArray->SetIndexBuffer(squareIB);
            batch->QuadVertexArray->Bind();
        }

#ifdef ET_PLATFORM_3DS
        // SHADERS
        s_Data.TextureShader.reset(Shader::Create(vshader02_shbin, vshader02_shbin_size));
        s_Data.TextureShader->Bind();
        s_Data.PhongShader.reset(Shader::Create(phongshader_shbin, phongshader_shbin_size));
        s_Data.NormalColorShader.reset(Shader::Create(normalcolorshader_shbin, normalcolorshader_shbin_size));
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS
        int32_t samplers[s_Data.MaxBatches];
        for (uint32_t i = 0; i < s_Data.MaxBatches; i++)
            samplers[i] = i;

        s_Data.TextureShader.reset(Shader::Create("assets/shaders/Texture.glsl"));
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxBatches);
        s_Data.NormalColorShader.reset(Shader::Create("assets/shaders/NormalColor.glsl"));
#endif // ET_PLATFORM_WINDOWS

        // CREATE WHITE TEXTURE
        {
            ET_PROFILE_SCOPE("Create 8x8 White Texture");
            // Minimum texture size is 8 x 8 for C3D. Anything smaller doesn't show up.
            s_Data.WhiteTexture = Texture2D::Create(8, 8);
            uint32_t whiteTextureData[8 * 8];
            std::fill_n(&whiteTextureData[0], 8 * 8, 0xffffffff);
            s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
            s_Data.WhiteTexture->Bind(0);
        }

        // Set first texture slot to 0
        s_Data.RenderBatches[0].BatchTexture = s_Data.WhiteTexture;
	}

	void Renderer3D::Shutdown()
	{
        ET_PROFILE_FUNCTION();
	}

    void Renderer3D::BeginScene(const PerspectiveCamera& camera, uint16_t screenSide)
    {
#ifdef ET_PLATFORM_WINDOWS
        RenderCommand::SetClearColor(0x68B0D8FF);
        RenderCommand::Clear();
#endif // ET_PLATFORM_WINDOWS

        ET_PROFILE_FUNCTION();

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix(screenSide));
        s_Data.NormalColorShader->Bind();
        s_Data.NormalColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix(screenSide));
        
        s_Data.WhiteTexture->Bind(0);

        for (uint32_t i = 0; i < Renderer3DData::MaxBatches; ++i) {
            s_Data.RenderBatches[i].IndexCount = 0;
            s_Data.RenderBatches[i].VertexBufferPtr = s_Data.RenderBatches[i].VertexBufferBase;
            s_Data.RenderBatches[i].BatchTexture = nullptr;
        }

        s_Data.RenderBatches[0].BatchTexture = s_Data.WhiteTexture;
        s_Data.IndexCount = 0;
        s_Data.BatchSlotIndex = 1;
    }

    void Renderer3D::EndScene()
    {
        ET_PROFILE_FUNCTION();

        Flush();
    }

    void Renderer3D::Flush()
    {
        ET_PROFILE_FUNCTION();

        s_Data.TextureShader->Bind();

        // TODO: maybe set TexEnv here?
        for (uint32_t i = 0; i < s_Data.BatchSlotIndex; i++) {
            RenderBatch* batch = &s_Data.RenderBatches[i];
            uint32_t dataSize = (uint8_t*)batch->VertexBufferPtr - (uint8_t*)batch->VertexBufferBase;
            if (dataSize == 0) continue;
            batch->QuadVertexBuffer->SetData(batch->VertexBufferBase, dataSize);

            batch->QuadVertexArray->Bind();
            batch->BatchTexture->Bind(0);
            RenderCommand::DrawIndexed(batch->QuadVertexArray, batch->IndexCount);

            s_Data.Stats.DrawCalls++;
        }
    }

	void Renderer3D::DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color)
	{
        ET_PROFILE_FUNCTION();

        const int quadIndexCount = 6;
        int batchIndex = GetBatch(s_Data.WhiteTexture, quadIndexCount);
        if (batchIndex < 0) { return; }
        RenderBatch* batch = &s_Data.RenderBatches[batchIndex];

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f * size.x, -0.5f * size.y, 0 }) * rotation);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f * size.x, -0.5f * size.y, 0 }) * rotation);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f * size.x, 0.5f * size.y, 0 }) * rotation);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f * size.x, 0.5f * size.y, 0 }) * rotation);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        s_Data.Stats.PolygonCount += 2;
        s_Data.Stats.VertexCount += 4;
        s_Data.Stats.IndexCount += 6;

        //s_Data->TextureShader->SetFloat4("u_Color", color);
        //s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0f);
        //s_Data->WhiteTexture->Bind();

        //glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        //s_Data->TextureShader->SetMat4("u_Transform", transform);

        //s_Data->QuadVertexArray->Bind();
        //RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color)
	{
        ET_PROFILE_FUNCTION();

        const uint32_t cubeIndexCount = 36;
        int batchIndex = GetBatch(s_Data.WhiteTexture, cubeIndexCount);
        if (batchIndex < 0) { return; }
        RenderBatch* batch = &s_Data.RenderBatches[batchIndex];

        // Front Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Back Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Top Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Bottom Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Right Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Left Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        s_Data.Stats.PolygonCount += 12;
        s_Data.Stats.VertexCount += 24;
        s_Data.Stats.IndexCount += 36;

        //s_Data->TextureShader->SetFloat4("u_Color", color);
        //s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0f);
        //s_Data->WhiteTexture->Bind();
        //
        //glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        //s_Data->TextureShader->SetMat4("u_Transform", transform);

        //s_Data->CubeVertexArray->Bind();
        //RenderCommand::DrawIndexed(s_Data->CubeVertexArray);
	}


    void Renderer3D::DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        ET_PROFILE_FUNCTION();

        const glm::vec4 color(1.0f);

        const uint32_t quadIndexCount = 6;
        int batchIndex = GetBatch(texture, quadIndexCount);
        if (batchIndex < 0) { return; }
        RenderBatch* batch = &s_Data.RenderBatches[batchIndex];

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f * size.x, -0.5f * size.y, 0 }) * rotation);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f * size.x, -0.5f * size.y, 0 }) * rotation);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f * size.x, 0.5f * size.y, 0 }) * rotation);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f * size.x, 0.5f * size.y, 0 }) * rotation);
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        s_Data.Stats.PolygonCount += 2;
        s_Data.Stats.VertexCount += 4;
        s_Data.Stats.IndexCount += 6;

        //s_Data->TextureShader->SetFloat4("u_Color", tintColor);
        //s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);
        //texture->Bind(); 

        //glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        //s_Data->TextureShader->SetMat4("u_Transform", transform);


        //s_Data->QuadVertexArray->Bind();
        //RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }
    
    void Renderer3D::DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        ET_PROFILE_FUNCTION();

        const uint32_t cubeIndexCount = 36;
        int batchIndex = GetBatch(texture, cubeIndexCount);
        if (batchIndex < 0) { return; }
        RenderBatch* batch = &s_Data.RenderBatches[batchIndex];

        s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);

        // Front Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Back Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Top Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Bottom Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Right Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Left Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, 0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, -0.5f }) * rotation * size);
        batch->VertexBufferPtr->Color = tintColor;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        s_Data.Stats.PolygonCount += 12;
        s_Data.Stats.VertexCount += 24;
        s_Data.Stats.IndexCount += 36;

        //s_Data->TextureShader->SetFloat4("u_Color", tintColor);
        //s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);
        //texture->Bind();

        //glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        //s_Data->TextureShader->SetMat4("u_Transform", transform);

        //s_Data->CubeVertexArray->Bind();
        //RenderCommand::DrawIndexed(s_Data->CubeVertexArray);
    }

    void Renderer3D::DrawCube(const glm::vec3& position, glm::vec4& color)
    {
        ET_PROFILE_FUNCTION();

        const uint32_t cubeIndexCount = 36;
        int batchIndex = GetBatch(s_Data.WhiteTexture, cubeIndexCount);
        if (batchIndex < 0) { return; }
        RenderBatch* batch = &s_Data.RenderBatches[batchIndex];

        // Front Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Back Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Top Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Bottom Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Right Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, 0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ 0.5f, -0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        // Left Face
        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, -0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, 0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->VertexBufferPtr->Position = position + (glm::vec3({ -0.5f, 0.5f, -0.5f }));
        batch->VertexBufferPtr->Color = color;
        batch->VertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        batch->VertexBufferPtr++;

        batch->IndexCount += 6;
        s_Data.IndexCount += 6;

        s_Data.Stats.PolygonCount += 12;
        s_Data.Stats.VertexCount += 24;
        s_Data.Stats.IndexCount += 36;
    }

    void Renderer3D::DrawMesh(Ref<Mesh> mesh, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color)
    {
        ET_PROFILE_FUNCTION();

        s_Data.NormalColorShader->Bind();
        s_Data.NormalColorShader->SetFloat4("u_Color", color);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        s_Data.NormalColorShader->SetMat4("u_Transform", transform);

        //mesh->Bind();
        s_Data.WhiteTexture->Bind();
        mesh->GetVertexArray()->Bind();
        RenderCommand::DrawIndexed(mesh->GetVertexArray());
    }

    void Renderer3D::DrawMesh(Ref<Mesh> mesh, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        ET_PROFILE_FUNCTION();

        s_Data.PhongShader->SetFloat4("u_Color", tintColor);
        s_Data.PhongShader->SetFloat("u_TilingFactor", tilingFactor);
        s_Data.PhongShader->Bind();
        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        s_Data.PhongShader->SetMat4("u_Transform", transform);

        //mesh->Bind();
        mesh->GetVertexArray()->Bind();
        RenderCommand::DrawIndexed(mesh->GetVertexArray());
    }

    int Renderer3D::GetBatch(Ref<Texture2D> textureRef, uint32_t indexCount) {
        //Search for batch assigned to texture in slots
        int batchIndex = -1;
        bool foundButFull = false;
        for (uint32_t i = 0; i < s_Data.BatchSlotIndex; ++i) {
            if (*s_Data.RenderBatches[i].BatchTexture.get() == *textureRef.get()) {
                if (s_Data.RenderBatches[i].IndexCount + indexCount < Renderer3DData::MaxIndices) {
                    batchIndex = i;
                    break;
                }
                else {
                    foundButFull = true;
                }
            }
        }

        // Add batch if:
        // 2. Not found but there are open batch slots.
        // 1. Found but no space (AllowMultpleBatchesPerTexture needs to be enabled)
        // Otherwise don't draw.
        if (batchIndex == -1) {

            if (s_Data.BatchSlotIndex >= Renderer3DData::MaxBatches) {
                return -1;
            }

            if (foundButFull && !s_Data.AllowMultipleBatchesPerTexture) {
                return -1;
            }

            batchIndex = s_Data.BatchSlotIndex;
            s_Data.RenderBatches[s_Data.BatchSlotIndex].BatchTexture = textureRef;

            s_Data.BatchSlotIndex++;
        }

        return batchIndex;
    }

    void Renderer3D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }
    void Renderer3D::SetStatsTimestep(Timestep ts)
    {
        s_Data.Stats.DeltaTime = ts.GetMilliseconds();
    }
    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }

}
