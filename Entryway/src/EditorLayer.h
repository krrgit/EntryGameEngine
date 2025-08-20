#pragma once

#include "Entry.h"

namespace Entry {
    class EditorLayer : public Layer
    {
        public:
            EditorLayer();
            virtual ~EditorLayer() = default;

            virtual void OnAttach() override;
		    virtual void OnDetach() override;

    	    void OnUpdate(Timestep ts, uint16_t screenSide = 0) override;
            virtual void OnImGuiRender() override ;
            void OnEvent(Event& event) override;

        private:
        PerspectiveCameraController m_CameraController;
        Ref<Framebuffer> m_Framebuffer;
        glm::vec2 m_ViewportSize = { 0,0 };
        
        bool m_ViewportFocused = false, m_ViewportHovered = false;

        // Temp
        Ref<Texture2D> m_CheckerboardTexture;
        Ref<Texture2D> m_EntryLogoTexture;

        Ref<Mesh> m_Teapot;

        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

        float m_Rotation = 0.0f;
    };
}