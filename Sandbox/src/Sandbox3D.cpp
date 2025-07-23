#include "Sandbox3D.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include <Platform/Citro3D/Citro3DShader.h>


Sandbox3D::Sandbox3D()
    : Layer("Sandbox3D"), m_CameraController(1.0f)
{

}

void Sandbox3D::OnAttach()
{
    
}

void Sandbox3D::OnDetach()
{

}

void Sandbox3D::OnUpdate(Entry::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

    //Render
    Entry::Renderer3D::BeginScene(m_CameraController.GetCamera());

    // TODO: Add these functions - Shader::SetMat4, Shader:: SetFloat4

    C3D_Mtx transform;
    C3D_Mtx scale;
    Mtx_Identity(&scale);
    Mtx_Identity(&transform);
    Mtx_Multiply(&transform, &transform, &scale);

    Entry::Renderer3D::DrawQuad(transform, m_SquareColor);
    Entry::Renderer3D::EndScene();

}

void Sandbox3D::OnImGuiRender() 
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox3D::OnEvent(Entry::Event& event) 
{
	//m_CameraController.OnEvent(event);
}