#pragma once

#include "Core.h"

#include "Window.h"
#include "Entry/Core/LayerStack.h"
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

        void Close();
    private:
        uint16_t m_WindowCount = 2;
        Window* m_CurrentWindow;
        std::array<Scope<Window>, 2> m_Windows;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        LayerStack m_LayerStack;
        Timestep m_LastFrameTime = 0.0f;
    private:
        static Application* s_Instance;
    };

    // Define in Client
    Application* CreateApplication();
}