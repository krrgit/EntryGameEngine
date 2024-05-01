#pragma once

#include "Core.h"

#include "Window.h"
#include "Entry/LayerStack.h"
#include "Entry/Events/Event.h"
#include "Entry/Events/ApplicationEvent.h"
#include "Entry/ImGui/ImGuiLayer.h"

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
        Scope<Window> m_WindowTop;
        Scope<Window> m_WindowBottom;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        LayerStack m_LayerStack;
        TickCounter m_FrameTime;
    private:
        static Application* s_Instance;
    };

    // Define in Client
    Application* CreateApplication();
}