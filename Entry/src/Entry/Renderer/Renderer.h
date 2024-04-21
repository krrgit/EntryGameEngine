#pragma once

#include "RenderCommand.h"
#include "PerspectiveCamera.h"
#include "Shader.h"

namespace Entry
{
    class Renderer
    {
    public:
        static void BeginScene(PerspectiveCamera& camera);
        static void EndScene();

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        struct SceneData {
            C3D_Mtx ProjectionMatrix;
            C3D_Mtx ViewMatrix;
        };

        static SceneData* m_SceneData;
    };
}