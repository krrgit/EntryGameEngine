#pragma once

#include "Entry.h"
#include "Panels/SceneHierarchyPanel.h"

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

        Ref<Scene> m_ActiveScene;

        Entity m_ShieldEntity;
        Entity m_CameraEntity;
        Entity m_SecondCamera;

        bool m_PrimaryCamera = true;

        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

        float m_Rotation = 0.0f;

        // Panels
        SceneHierarchyPanel m_SceneHierarchyPanel;
    };
}