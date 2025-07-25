#pragma once

#include "Entry.h"

class Sandbox3D : public Entry::Layer
{
    public:
        Sandbox3D();
        virtual ~Sandbox3D() = default;

        virtual void OnAttach() override;
		virtual void OnDetach() override;

    	void OnUpdate(Entry::Timestep ts) override;
        virtual void OnImGuiRender() override ;
        void OnEvent(Entry::Event& event) override;

    private:
    Entry::PerspectiveCameraController m_CameraController;

    // Temp
    Entry::Ref<Entry::VertexArray> m_SquareVA;
    Entry::Ref<Entry::Shader> m_FlatColor;

    Entry::Ref<Entry::Texture2D> m_CheckerboardTexture;
    Entry::Ref<Entry::Texture2D> m_EntryLogoTexture;
    
    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

    float m_Rotation = 0.0f;

    struct ProfileResult {
        const char* Name;
        float Time;
    };

    std::vector<ProfileResult> m_ProfileResults;

};