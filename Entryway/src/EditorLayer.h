#pragma once

#include "Entry.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/LightEnvironmentPanel.h"

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
        void OpenScene(const std::filesystem::path& path); // C++17
        void SaveSceneAs();
        void SaveScene();

        void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);
        
        void OnScenePlay();
        void OnSceneStop();

        void OnDuplicateEntity();

        // UI Panels
        void UI_Toolbar();

        void SetPanelContexts(Ref<Scene> context);

        private:
            PerspectiveCameraController m_CameraController;

            Ref<Framebuffer> m_SceneFramebuffer;
            glm::vec2 m_SceneViewportSize = { 0, 0 };
            glm::vec2 m_ViewportBounds[2];

            bool m_ViewportFocused = false, m_ViewportHovered = false;

            Ref<Framebuffer> m_GameFramebuffer;
            glm::vec2 m_GameViewportSize = { 0, 0 };
        
            Ref<Scene> m_ActiveScene;
            Ref<Scene> m_EditorScene;

            bool m_PrimaryCamera = true;

            EditorCamera m_EditorCamera;

            std::filesystem::path m_EditorScenePath;
            int m_GizmoType = -1;

            enum class SceneState
            {
                Edit = 0, Play = 1
            };

            Entity m_HoveredEntity;

            // TEMP
            Entity m_ShieldEntity;
            Entity m_CameraEntity;
            Entity m_SecondCamera;

            // Panels
            SceneHierarchyPanel m_SceneHierarchyPanel;
            ContentBrowserPanel m_ContentBrowserPanel;
            LightEnvironmentPanel m_LightEnvironmentPanel;

            Ref<Texture2D> m_IconPlay, m_IconPause, m_IconAdvance;

            SceneState m_SceneState = SceneState::Edit;

    };
}