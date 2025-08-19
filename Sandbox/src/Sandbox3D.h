#pragma once

#include "Entry.h"

class Sandbox3D : public Entry::Layer
{
    public:
        Sandbox3D();
        virtual ~Sandbox3D() = default;

        virtual void OnAttach() override;
		virtual void OnDetach() override;

    	void OnUpdate(Entry::Timestep ts, uint16_t screenSide = 0) override;
        virtual void OnImGuiRender() override ;
        void OnEvent(Entry::Event& event) override;

    private:
    Entry::PerspectiveCameraController m_CameraController;
    bool m_ShowImGui = true;

    // Temp
    Entry::Ref<Entry::Texture2D> m_CheckerboardTexture;
    Entry::Ref<Entry::Texture2D> m_EntryLogoTexture;
    Entry::Ref<Entry::Mesh> m_Teapot;
    
    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

    float m_Rotation = 0.0f;
};