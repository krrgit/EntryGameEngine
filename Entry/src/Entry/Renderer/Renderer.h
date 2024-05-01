#pragma once

#include "RenderCommand.h"
#include "PerspectiveCamera.h"
#include "Shader.h"

#include<glm/glm.hpp>

namespace Entry
{
    const C3D_Mtx Identity = {
    .r = {{0.0f, 0.0f, 0.0f, 1.0f},  // w,z,y,x
          {0.0f, 0.0f, 1.0f, 0.0f},  
          {0.0f, 1.0f, 0.0f, 0.0f},  
          {1.0f, 0.0f, 0.0f, 0.0f}}  
    };

    class Renderer
    {
    public:
        static void Init();

        static void BeginScene(PerspectiveCamera& camera);
        static void EndScene();

        static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const C3D_Mtx& transform = Identity);

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        struct SceneData {
            C3D_Mtx ViewProjectionMatrix;
        };

        static SceneData* m_SceneData;
    };
}