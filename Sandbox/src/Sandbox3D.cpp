#include "Sandbox3D.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

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


	Entry::Renderer3D::DrawQuad({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, m_SquareColor);
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