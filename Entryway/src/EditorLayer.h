#pragma once

#include "Entry.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "Entry/Renderer/EditorCamera.h"

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
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void NewScene();
        void OpenScene();
        void SaveScene();
        void SaveSceneAs();

        private:
            PerspectiveCameraController m_CameraController;
            Ref<Framebuffer> m_SceneFramebuffer;
            Ref<Framebuffer> m_GameFramebuffer;
            glm::vec2 m_SceneViewportSize = { 0, 0 };
            glm::vec2 m_GameViewportSize = { 0, 0 };
            glm::vec2 m_ViewportBounds[2];
        
            bool m_ViewportFocused = false, m_ViewportHovered = false;
            Ref<Scene> m_ActiveScene;

            bool m_PrimaryCamera = true;

            EditorCamera m_EditorCamera;

            std::string m_SceneFilePath = "";
            int m_GizmoType = -1;

            Entity m_HoveredEntity;

            // TEMP
            Entity m_ShieldEntity;
            Entity m_CameraEntity;
            Entity m_SecondCamera;

            // Panels
            SceneHierarchyPanel m_SceneHierarchyPanel;
            ContentBrowserPanel m_ContentBrowserPanel;
    };
}