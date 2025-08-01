#include "etpch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

//Shader
// #include "vshader01_shbin.h" // Flat Color Shader
#include "vshader02_shbin.h" // Texture + Color Shader

namespace Entry {

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color; 
        glm::vec2 TexCoord;
    };

    struct RenderBatch {
        Ref <VertexBuffer> QuadVertexBuffer;

        uint32_t IndexCount = 0;
        QuadVertex* VertexBufferBase = nullptr;
        QuadVertex* VertexBufferPtr = nullptr;
    };

    struct Renderer3DData 
    {
        static const uint32_t MaxQuads = 50; // Per Batch atm
        static const uint32_t MaxVertices = MaxQuads * 4; // Per Batch atm
        static const uint16_t MaxIndices = MaxQuads * 6; // Per Batch atm
        static const uint32_t MaxTextureSlots = 3; // TODO: RenderCaps (3 per C3D_Context)

        Ref <Shader> TextureShader;
        Ref <Texture2D> WhiteTexture;

        std::array<RenderBatch, MaxTextureSlots> RenderBatches;

        Ref <VertexArray> QuadVertexArray;
        uint32_t QuadIndexCount;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture
        
        Renderer3D::Statistics Stats;
    };


    static Renderer3DData s_Data;

	void Renderer3D::Init()
	{
        ET_PROFILE_FUNCTION();

        s_Data.QuadVertexArray = VertexArray::Create();
        for (uint32_t i = 0; i < s_Data.RenderBatches.size();++i) {
            RenderBatch* batch = &s_Data.RenderBatches[i];
            batch->QuadVertexBuffer.reset(VertexBuffer::Create(sizeof(QuadVertex) * s_Data.MaxVertices));
            batch->QuadVertexBuffer->SetLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float4, "a_Color" },
                { ShaderDataType::Float2, "a_TexCoord" }
                });
            s_Data.QuadVertexArray->AddVertexBuffer(batch->QuadVertexBuffer);
            batch->VertexBufferBase = new QuadVertex[s_Data.MaxVertices];
        }

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
        s_Data.QuadVertexArray->SetIndexBuffer(squareIB); 

        // SHADERS
        s_Data.TextureShader.reset(Shader::Create(vshader02_shbin, vshader02_shbin_size));
        s_Data.TextureShader->Bind();

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

        // Set all textures slots to 0
        s_Data.TextureSlots[0] = s_Data.WhiteTexture;


        // Configure the first fragment shading substage to just pass through the vertex color
        // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
        C3D_TexEnv* env = C3D_GetTexEnv(0);
        C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

	}

	void Renderer3D::Shutdown()
	{
        ET_PROFILE_FUNCTION();

	}

	void Renderer3D::BeginScene(const PerspectiveCamera& camera)
	{
        ET_PROFILE_FUNCTION();

        s_Data.QuadVertexArray->Bind();

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
        
        s_Data.WhiteTexture->Bind(0);

        for (uint32_t i = 0; i < s_Data.RenderBatches.size(); ++i) {
            s_Data.RenderBatches[i].IndexCount = 0;
            s_Data.RenderBatches[i].VertexBufferPtr = s_Data.RenderBatches[i].VertexBufferBase;
        }

        s_Data.QuadIndexCount = 0;
        s_Data.TextureSlotIndex = 1;
    }

	void Renderer3D::EndScene()
	{
        ET_PROFILE_FUNCTION();

        Flush();
	}

    void Renderer3D::Flush() 
    {
        ET_PROFILE_FUNCTION();

        // TODO: maybe set TexEnv here?
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
            uint32_t dataSize = (uint8_t*)s_Data.RenderBatches[i].VertexBufferPtr - (uint8_t*)s_Data.RenderBatches[i].VertexBufferBase;
            if (dataSize == 0) continue;
            s_Data.RenderBatches[i].QuadVertexBuffer->SetData(s_Data.RenderBatches[i].VertexBufferBase, dataSize);

            s_Data.TextureSlots[i]->Bind(i);
            RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.RenderBatches[i].IndexCount);

            s_Data.Stats.DrawCalls++;
        }
    }

    void Renderer3D::FlushAndReset() {
        ET_PROFILE_FUNCTION();

        EndScene();

        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
            s_Data.RenderBatches[i].IndexCount = 0;
            s_Data.RenderBatches[i].VertexBufferPtr = s_Data.RenderBatches[i].VertexBufferBase;
        }

        s_Data.QuadIndexCount = 0;
        s_Data.TextureSlotIndex = 1;
    }

    void Renderer3D::DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color)
    {
        ET_PROFILE_FUNCTION();

        const int textureIndex = 0; // White Texture
        RenderBatch* batch = &s_Data.RenderBatches[textureIndex];

        if (batch->IndexCount + 6 >= Renderer3DData::MaxIndices) 
        {
            FlushAndReset();
        }

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
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color)
	{
        ET_PROFILE_FUNCTION();

        const int textureIndex = 0; // White Texture
        RenderBatch* batch = &s_Data.RenderBatches[textureIndex];

        if (batch->IndexCount + 36 >= Renderer3DData::MaxIndices)
        {
            // Issues occur here
            FlushAndReset();
            
        }

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount += 6;
	}

    void Renderer3D::DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        ET_PROFILE_FUNCTION();

        const glm::vec4 color(1.0f);

        const uint32_t indexCount = 6;

        //Search for texture in slots
        int textureIndex = 0;
        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; ++i) {
            if (*s_Data.TextureSlots[i].get() == *texture.get()) {
                textureIndex = i;
                break;
            }
        }

        // Add texture if not found
        if (textureIndex == 0) {
            if (s_Data.TextureSlotIndex >= Renderer3DData::MaxTextureSlots) {
                //FlushAndReset();
                return;
            }
            textureIndex = s_Data.TextureSlotIndex;
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
            texture->Bind(textureIndex);
        }
        else if (s_Data.RenderBatches[textureIndex].IndexCount + indexCount >= Renderer3DData::MaxIndices) {

            FlushAndReset();

            textureIndex = s_Data.TextureSlotIndex;
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
            texture->Bind(textureIndex);
        }

        RenderBatch* batch = &s_Data.RenderBatches[textureIndex];

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
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;

        
#if OLD_PATH
         s_Data.QuadTextureShader->SetFloat4("u_Color", tintColor);
        s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);
        texture->Bind(); 

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        s_Data.TextureShader->SetMat4("u_Transform", transform);


        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
    }
    
    void Renderer3D::DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        ET_PROFILE_FUNCTION();

        const uint32_t indexCount = 36;


        //Search for texture in slots
        int textureIndex = 0;
        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; ++i) {
            if (*s_Data.TextureSlots[i].get() == *texture.get()) {
                textureIndex = i;
                break;
            }
        }

        // Add texture if not found
        if (textureIndex == 0) {
            if (s_Data.TextureSlotIndex >= Renderer3DData::MaxTextureSlots) {
                //FlushAndReset();
                return;
            }
            textureIndex = s_Data.TextureSlotIndex;
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
            texture->Bind(textureIndex);
        }
        else if (s_Data.RenderBatches[textureIndex].IndexCount + indexCount >= Renderer3DData::MaxIndices) {
            return;

            FlushAndReset();

            textureIndex = s_Data.TextureSlotIndex;
            s_Data.TextureSlots[textureIndex] = texture;
            s_Data.TextureSlotIndex++;
            texture->Bind(textureIndex);
        }

        RenderBatch* batch = &s_Data.RenderBatches[textureIndex];

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

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
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount += 6;

        //s_Data.TextureShader->SetFloat4("u_Color", tintColor);
        //s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);
        //texture->Bind();

        //glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        //s_Data.TextureShader->SetMat4("u_Transform", transform);

        //s_Data.CubeVertexArray->Bind();
        //RenderCommand::DrawIndexed(s_Data.CubeVertexArray);
    }
    void Renderer3D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }
    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }
}
