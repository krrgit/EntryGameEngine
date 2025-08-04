#include "Sandbox3D.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <chrono>

// Texture Headers
#include "Checkerboard_t3x.h"

Sandbox3D::Sandbox3D()
    : Layer("Sandbox3D"), m_CameraController(1.0f)
{

}

void Sandbox3D::OnAttach()
{
	ET_PROFILE_FUNCTION();

	m_CheckerboardTexture = Entry::Texture2D::Create(Checkerboard_t3x, Checkerboard_t3x_size);

	m_Teapot = Entry::Mesh::Create("teapot.obj");
}

void Sandbox3D::OnDetach()
{
	ET_PROFILE_FUNCTION();

}

void Sandbox3D::OnUpdate(Entry::Timestep ts)
{
	ET_PROFILE_FUNCTION();
	
	// Update
	{
		ET_PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}

	{
		ET_PROFILE_SCOPE("Renderer Draw");

		m_Rotation += ts.GetSeconds();
		m_Rotation = m_Rotation > 6.28f ? 0 : m_Rotation;

		//Render
		Entry::Renderer3D::BeginScene(m_CameraController.GetCamera());
		Entry::Renderer3D::DrawQuad({ 0.0f, -0.5f, 0.0f }, glm::quat(glm::vec3(glm::radians(90.0f),0,0)), {10.0f, 10.0f, 1.0f}, m_CheckerboardTexture, 1.25f);
		//Entry::Renderer3D::DrawCube({-2.0f, 1.0f, -2.0f},  glm::quat(glm::vec3(m_Rotation, m_Rotation, 0)), glm::vec3(1.0f), m_CheckerboardTexture, 0.5f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		//Entry::Renderer3D::DrawQuad({ 2.0f, 1.0f, 0.0f }, glm::quat(glm::vec3(0, m_Rotation, 0)), glm::vec3(1.0f), m_SquareColor);
		//Entry::Renderer3D::DrawCube({ 0.0f, 0.0f, 0.0f }, glm::quat(glm::vec3(0)), glm::vec3(1.0f), m_SquareColor);

		Entry::Renderer3D::DrawMesh(m_Teapot, { 0.0f, 0.0f, 0.0f }, glm::quat(glm::vec3(0, m_Rotation, 0)), glm::vec3(0.5f), m_SquareColor);
		//Entry::Renderer3D::DrawMesh(m_Teapot, { 0.0f, 0.0f, 0.0f },  glm::quat(glm::vec3(0, m_Rotation, 0)), glm::vec3(.3f), m_CheckerboardTexture);

		Entry::Renderer3D::EndScene();
	}
}

void Sandbox3D::OnImGuiRender() 
{
	ET_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	//ImGui::Text("Rotation: %.2f\n", m_Rotation);

	ImGui::End();
}

void Sandbox3D::OnEvent(Entry::Event& event) 
{
	//m_CameraController.OnEvent(event);
}