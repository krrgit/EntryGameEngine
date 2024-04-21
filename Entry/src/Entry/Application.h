#pragma once

#include "Core.h"

#include "Window.h"
#include "Entry/LayerStack.h"
#include "Entry/Events/Event.h"
#include "Entry/Events/ApplicationEvent.h"

#include "Entry/Renderer/VertexArray.h"
#include "Entry/Renderer/Shader.h"
#include "Entry/Renderer/Buffer.h"

namespace Entry {

    class Application{
        public:
            Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        inline static Application& Get() { return *s_Instance; }
        inline Window& GetWindow() { return *m_Window; }
    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        LayerStack m_LayerStack;

        int uLoc_projection;
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

    private:
        static Application* s_Instance;
    };

    // Define in Client
    Application* CreateApplication();
}