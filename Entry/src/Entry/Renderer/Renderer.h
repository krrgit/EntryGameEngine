#pragma once

#include "RenderCommand.h"
#include "PerspectiveCamera.h"
#include "Shader.h"

#include<glm/glm.hpp>

namespace Entry
{
    //const C3D_Mtx Identity = {
    //.r = {{0.0f, 0.0f, 0.0f, 1.0f},  // w,z,y,x
    //      {0.0f, 0.0f, 1.0f, 0.0f},  
    //      {0.0f, 1.0f, 0.0f, 0.0f},  
    //      {1.0f, 0.0f, 0.0f, 0.0f}}  
    //};

    class Renderer
    {
    public:
        static void Init();

        static void BeginScene(PerspectiveCamera& camera);
        static void EndScene();

        static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        struct SceneData {
            glm::mat4 ViewProjectionMatrix;
        };

        static SceneData* m_SceneData;
    };
}