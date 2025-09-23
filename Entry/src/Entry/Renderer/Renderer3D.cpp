#include "etpch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#ifdef ET_PLATFORM_3DS
//Shaders
#include "UnlitTexture_shbin.h" // Unlit Texture Shader
#include "LitTexture_shbin.h" // Lit Texture Shader
#endif // ET_PLATFORM_3DS

namespace Entry {

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;

#ifdef ET_PLATFORM_WINDOWS
        // Editor-only
        int EntityID = 0; // TODO: Use this after Render Batches for Meshes is implemented.
#endif // ET_PLATFORM_WINDOWS
    };

    struct RenderBatch {
        Ref <VertexArray> MeshVertexArray;
        Ref <VertexBuffer> MeshVertexBuffer;

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

        // Per Batch Limitations (Unused)
        //static const uint32_t MaxPolygons = 1024; // Max: 5298 (seems like CPU bottleneck)
        //static const uint32_t MaxVertices = MaxPolygons * 2;
        //static const uint16_t MaxIndices = MaxPolygons * 3;
        //static const uint32_t MaxBatches = 1; // Randomly selected tbh
        //bool AllowMultipleBatchesPerTexture = true;
        ////std::array<RenderBatch, MaxBatches> RenderBatches;
        //uint32_t IndexCount;
        //uint32_t m_VertexArray, m_VertexBuffer, m_IndexBuffer;

        static const uint32_t MaxTextureSlots = 4; // 3 texture units slots, 1 proceedural generated texture slot (set by PICA 200)
        std::vector<Ref<Shader>> Shaders;
        Ref <Texture2D> WhiteTexture;

        Renderer3D::Statistics Stats;

        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        Ref<Material> DefaultMaterial;
        ShaderProgram DefaultShader;
        ShaderProgram BindedShader;
    };

    static Renderer3DData s_Data;

	void Renderer3D::Init()
	{
        ET_PROFILE_FUNCTION();


#ifdef ET_PLATFORM_3DS
        // SHADERS
        s_Data.Shaders.push_back(Ref<Shader>(Shader::Create(LitTexture_shbin, LitTexture_shbin_size)));
        s_Data.Shaders.push_back(Ref<Shader>(Shader::Create(UnlitTexture_shbin, UnlitTexture_shbin_size)));
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS
        int32_t samplers[s_Data.MaxTextureSlots];
        for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
            samplers[i] = i;

        s_Data.Shaders.push_back(Ref<Shader>(Shader::Create("assets/shaders/LitTexture.glsl")));
        s_Data.Shaders.push_back(Ref<Shader>(Shader::Create("assets/shaders/UnlitTexture.glsl")));

        for (auto& s : s_Data.Shaders)
        {
            s->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);
        }

#endif // ET_PLATFORM_WINDOWS

        // Set Default Shader
        s_Data.DefaultShader = ShaderProgram(ShaderProgramEnum::Lit);
        s_Data.BindedShader = ShaderProgramEnum::NoShader;

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

        // Create Default Material
        MaterialValues defMatVal{ 0.8f, 0.8f, 0.8f };
        MaterialProps defMatProps;
        defMatProps.Values = defMatVal;
        defMatProps.Name = "Default Material";
        defMatProps.DiffuseMap = s_Data.WhiteTexture;
        defMatProps.shader = s_Data.DefaultShader;

        TexEnvProps texEnv0
        {
            // RGB
            ET_RGBA_Separate, ET_GPU_MODULATE, ET_GPU_TEXTURE0, ET_GPU_FRAGMENT_PRIMARY_COLOR, ET_GPU_PRIMARY_COLOR,
            // Alpha
            ET_GPU_REPLACE, ET_GPU_PRIMARY_COLOR, ET_GPU_PRIMARY_COLOR, ET_GPU_PRIMARY_COLOR
        };

        TexEnvProps texEnv1
        {
            ET_RGBA_Separate, ET_GPU_ADD, ET_GPU_PREVIOUS, ET_GPU_FRAGMENT_SECONDARY_COLOR, ET_GPU_PRIMARY_COLOR
        };

        s_Data.DefaultMaterial = Material::Create(defMatProps);
        s_Data.DefaultMaterial->SetTexEnvProps(texEnv0, 0);
        s_Data.DefaultMaterial->SetTexEnvProps(texEnv1, 1);
	}

	void Renderer3D::Shutdown()
	{
        ET_PROFILE_FUNCTION();
	}

    // Runtime BeginScene()
    void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform, uint16_t screenSide)
    {
        ET_PROFILE_FUNCTION();

        s_Data.m_ViewMatrix = glm::inverse(transform);
        s_Data.m_ProjectionMatrix = camera.GetProjection(screenSide);
        s_Data.m_ViewProjectionMatrix = s_Data.m_ProjectionMatrix * s_Data.m_ViewMatrix;

        s_Data.BindedShader = ShaderProgramEnum::NoShader; // Reset
        
        // Doesn't work on 3DS
        // Note: When setting values of a shader(i.e. u_Projection, previously set shaders are unset).
        // Atm we set it at runtime.
        //for (uint32_t i = 0; i < s_Data.Shaders.size(); ++i)
        //{
        //    s_Data.Shaders[i]->Bind();
        //    s_Data.Shaders[i]->SetMat4("u_Projection", s_Data.m_ProjectionMatrix);
        //}
    }

    // Editor BeginScene()
    void Renderer3D::BeginScene(const EditorCamera& camera, uint16_t screenSide)
    {
        ET_PROFILE_FUNCTION();

        s_Data.m_ViewMatrix = camera.GetViewMatrix();
        s_Data.m_ProjectionMatrix = camera.GetProjection(screenSide);
        s_Data.m_ViewProjectionMatrix = s_Data.m_ProjectionMatrix * s_Data.m_ViewMatrix;

        s_Data.BindedShader = ShaderProgramEnum::NoShader; // Reset
    }

    void Renderer3D::EndScene()
    {
        ET_PROFILE_FUNCTION();
    }

    void Renderer3D::DrawModel(Ref<Model> Model, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color)
    {
        ET_PROFILE_FUNCTION();

        s_Data.Shaders[0]->Bind();
        s_Data.Shaders[0]->SetFloat4("u_Color", color);

        glm::mat4 modelView = s_Data.m_ViewMatrix * glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), size);
        s_Data.Shaders[0]->SetMat4("u_ModelView", modelView);

        s_Data.WhiteTexture->Bind();
        Model->GetVertexArray()->Bind();

        for (auto mesh : Model->GetMeshes())
        {
            Model->GetMaterial(mesh.MaterialID)->Bind();
            RenderCommand::DrawIndexed(Model->GetVertexArray(), mesh.IndexCount, mesh.IndexOffset);
        }

        s_Data.Stats.PolygonCount += Model->GetPolygonCount();
        s_Data.Stats.VertexCount += Model->GetVertexCount();
        s_Data.Stats.IndexCount += Model->GetIndexCount();
        s_Data.Stats.DrawCalls++;
    }

    void Renderer3D::DrawModel(Ref<Model> Model, const glm::mat4& transform)
    {
        ET_PROFILE_FUNCTION();

        s_Data.Shaders[0]->Bind();
        s_Data.Shaders[0]->SetFloat4("u_Color", glm::vec4(1.0f));

        glm::mat4 modelView = s_Data.m_ViewMatrix * transform;
        s_Data.Shaders[0]->SetMat4("u_ModelView", modelView);

        Model->GetVertexArray()->Bind();

        for (auto mesh : Model->GetMeshes())
        {
            Model->GetMaterial(mesh.MaterialID)->Bind();
            RenderCommand::DrawIndexed(Model->GetVertexArray(), mesh.IndexCount, mesh.IndexOffset);
        }

        s_Data.Stats.PolygonCount += Model->GetPolygonCount();
        s_Data.Stats.VertexCount += Model->GetVertexCount();
        s_Data.Stats.IndexCount += Model->GetIndexCount();
        s_Data.Stats.DrawCalls++;
    }

    void Renderer3D::DrawMesh(MeshRendererComponent& mrc, const glm::mat4& transform)
    {
        ET_PROFILE_FUNCTION();

        if (!mrc.material) return;

        Material* meshMtl = mrc.material.get();
        Shader* meshShader = s_Data.Shaders[(int)meshMtl->GetShader()].get();

        // Only bind when not current shader
        if (meshMtl->GetShader() != s_Data.BindedShader)
        {
            meshShader->Bind();
            meshShader->SetMat4("u_Projection", s_Data.m_ProjectionMatrix);
            s_Data.BindedShader = meshMtl->GetShader();
        }

        meshMtl->Bind();
        mrc.model->GetVertexArray()->Bind();

        glm::mat4 modelView = s_Data.m_ViewMatrix * transform;
        meshShader->SetMat4("u_ModelView", modelView);
        meshShader->SetFloat4("u_Color", glm::vec4(1.0f));

        RenderCommand::DrawIndexed(mrc.model->GetVertexArray(), mrc.mesh->IndexCount, mrc.mesh->IndexOffset);

        s_Data.Stats.PolygonCount += mrc.mesh->PolygonCount;
        s_Data.Stats.VertexCount += mrc.mesh->VertexCount;
        s_Data.Stats.IndexCount += mrc.mesh->IndexCount;
        s_Data.Stats.DrawCalls++;
    }

    void Renderer3D::DrawMeshEntity(MeshRendererComponent& mrc, const glm::mat4& transform, int entityID)
    {
        if (!mrc.material)
            return;

        s_Data.Shaders[(int)mrc.material->GetShader()]->SetInt("u_EntityID", entityID);
        DrawMesh(mrc, transform);
    }

    void Renderer3D::ResetStats()
    {
        s_Data.Stats.DeltaTime = 0;
        s_Data.Stats.DrawCalls = 0;
        s_Data.Stats.IndexCount = 0;
        s_Data.Stats.PolygonCount = 0;
        s_Data.Stats.VertexCount = 0;
    }
    void Renderer3D::SetStatsTimestep(Timestep ts)
    {
        s_Data.Stats.DeltaTime = ts.GetMilliseconds();
    }
    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }

    Ref<Material> Renderer3D::GetDefaultMaterial()
    {
        return s_Data.DefaultMaterial;
    }

    ShaderProgram Renderer3D::GetDefaultShader()
    {
        return s_Data.DefaultShader;
    }

}
