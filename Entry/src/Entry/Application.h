#pragma once

#include "Core.h"

#include "Window.h"
#include "Entry/LayerStack.h"
#include "Entry/Events/Event.h"
#include "Entry/Events/ApplicationEvent.h"

#include "Buffer.h"
#include "VertexArray.h"

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

        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<VertexArray> m_SquareVA;

        DVLB_s* vshader_dvlb;
        shaderProgram_s program;
        int uLoc_projection;
        C3D_Mtx projection;

        void* vbo_data;

        C3D_RenderTarget* target;

        C3D_AttrInfo m_AttrInfo;
        C3D_BufInfo m_BufInfo;

    private:
        static Application* s_Instance;
    };

    // Define in Client
    Application* CreateApplication();
}