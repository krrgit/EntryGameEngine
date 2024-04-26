#pragma once

#include "Core.h"

#include "Window.h"
#include "Entry/LayerStack.h"
#include "Entry/Events/Event.h"
#include "Entry/Events/ApplicationEvent.h"

#include "Entry/Renderer/VertexArray.h"
#include "Entry/Renderer/Shader.h"
#include "Entry/Renderer/Buffer.h"
#include "Entry/Renderer/PerspectiveCamera.h"

#define ET_WINDOW_TOP 0
#define ET_WINDOW_BOTTOM 1


namespace Entry {

    class Application{
        public:
            Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer, int window);
        void PushOverlay(Layer* layer, int window);

        inline static Application& Get() { return *s_Instance; }
        inline Window& GetWindow() { return *m_CurrentWindow; }
    private:
        Window* m_CurrentWindow;
        std::unique_ptr<Window> m_WindowTop;
        std::unique_ptr<Window> m_WindowBottom;
        bool m_Running = true;
        LayerStack m_LayerStack;

        int uLoc_projection;
        int uLoc_modelView;
        int uLoc_ViewProjection;
        C3D_Mtx projection;

        void* vbo_data;

        C3D_AttrInfo m_AttrInfo;
        C3D_BufInfo m_BufInfo;

        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;

        std::shared_ptr<VertexArray> m_SquareVA;
        std::shared_ptr<Shader> m_BlueShader;

        PerspectiveCamera m_Camera;
        glm::vec3 m_CamPos = {0.0f, 0.0f, 1.0f};
        glm::vec4 m_CamRot = {0.0f, 0.0f, 0.0f, 0.0f};

    private:
        static Application* s_Instance;
    };

    // Define in Client
    Application* CreateApplication();
}