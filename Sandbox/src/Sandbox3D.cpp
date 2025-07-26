#include "Sandbox3D.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

// Texture Headers
#include "Checkerboard_t3x.h"

Sandbox3D::Sandbox3D()
    : Layer("Sandbox3D"), m_CameraController(1.0f)
{

}

void Sandbox3D::OnAttach()
{
	m_CheckerboardTexture = Entry::Texture2D::Create(Checkerboard_t3x, Checkerboard_t3x_size);
}

void Sandbox3D::OnDetach()
{

}

void Sandbox3D::OnUpdate(Entry::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

	m_Rotation += ts.GetSeconds();
	m_Rotation = m_Rotation > 6.28f ? 0 : m_Rotation;

    //Render
	glm::vec4 grayColor(0.5f);
    Entry::Renderer3D::BeginScene(m_CameraController.GetCamera());
	Entry::Renderer3D::DrawQuad({ 0.0f, -0.5f, 0.0f }, glm::quat(glm::vec3(glm::radians(90.0f),0,0)), {10.0f, 10.0f, 1.0f}, m_CheckerboardTexture);
	Entry::Renderer3D::DrawQuad({ 2.0f, 1.0f, 0.0f },  glm::quat(glm::vec3(0)), { 1.0f, 1.0f, 1.0f }, m_SquareColor);
	Entry::Renderer3D::DrawCube({ 0.0f, 0.0f, 0.0f },  glm::quat(glm::vec3(0)), { 1.0f, 1.0f, 1.0f }, m_SquareColor);
	Entry::Renderer3D::DrawCube({-2.0f, 1.0f, -2.0f},  glm::quat(glm::vec3(m_Rotation, m_Rotation, 0)), {1.0f, 1.0f, 1.0f}, m_CheckerboardTexture);
    Entry::Renderer3D::EndScene();

}

void Sandbox3D::OnImGuiRender() 
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::InputFloat("Rotation", &m_Rotation, 0, 90);
	ImGui::End();
}

void Sandbox3D::OnEvent(Entry::Event& event) 
{
	//m_CameraController.OnEvent(event);
}