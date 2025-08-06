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

	Entry::Renderer3D::ResetStats();

	{
		ET_PROFILE_SCOPE("Renderer Draw");

		m_Rotation += ts.GetSeconds();
		m_Rotation = m_Rotation > 6.28f ? 0 : m_Rotation;

		//Render
		glm::vec4 redColor = { 1.0f, 0.2f, 0.3f, 1.0f };
		Entry::Renderer3D::BeginScene(m_CameraController.GetCamera());
		//Entry::Renderer3D::DrawQuad({ 0.0f, -0.5f, 0.0f }, glm::quat(glm::vec3(glm::radians(90.0f),0,0)), {8.0f, 8.0f, 1.0f}, m_CheckerboardTexture);
		//Entry::Renderer3D::DrawCube({-2.0f, 1.0f, -2.0f},  glm::quat(glm::vec3(m_Rotation, m_Rotation, 0)), glm::vec3(1.0f), m_CheckerboardTexture, 0.5f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		//Entry::Renderer3D::DrawQuad({ 2.0f, 1.0f, 0.0f }, glm::quat(glm::vec3(0, m_Rotation, 0)), glm::vec3(1.0f), m_SquareColor);
		//Entry::Renderer3D::DrawQuad({ -2.0f, 1.0f, 0.0f }, glm::quat(glm::vec3(0, m_Rotation, 0)), glm::vec3(1.0f), redColor);
		//Entry::Renderer3D::DrawQuad({ 0.0f, 2.0f, 0.0f }, glm::quat(glm::vec3(0, 0, m_Rotation)), glm::vec3(1.0f), m_CheckerboardTexture);
		//Entry::Renderer3D::DrawCube({ 0.0f, 0.0f, 0.0f },  glm::quat(glm::vec3(0)), glm::vec3(1.0f), m_SquareColor);

		float side = 12.0f;
		float scale = 3.0f;

		for (float z = 0; z < side; z++) {
			for (float y = 0; y < side; y++) {
				for (float x = 0; x < side; x++) {
					glm::vec4 color = glm::vec4((x / side) * 0.5f + 0.5f, (z / side) * 0.5f + 0.5f, (y / side) * 0.5f + 0.5f, 1.0f);
					//Entry::Renderer3D::DrawCube({ (x - (side / 2.0f)) * 2, (z + sin(m_Rotation + x + y + z) * 0.2f) * 2, (y - (side / 2.0f)) * 2 }, glm::quat(glm::vec3(m_Rotation + x + z, m_Rotation + y + z, 0)), glm::vec3(1.0f), m_CheckerboardTexture, 0.5f, color);
					//Entry::Renderer3D::DrawCube({ (x - (side / 2.0f)) * 2, (z + sin(m_Rotation + x + y + z) * 0.2f) * 2, (y - (side / 2.0f)) * 2 }, glm::quat(glm::vec3(m_Rotation + x + z, m_Rotation + y + z, 0)), glm::vec3(1.0f), color);
					Entry::Renderer3D::DrawCube({ (x - (side / 2.0f)) * scale, (z + sin(m_Rotation + x + y + z) * 0.2f) * scale, (y - (side / 2.0f)) * scale }, color);
				}
			}
		}
		
		Entry::Renderer3D::EndScene();
	}
}

void Sandbox3D::OnImGuiRender() 
{
	ET_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	auto stats = Entry::Renderer3D::GetStats();
	ImGui::Text("Renderer3D Stats:");
	ImGui::Text("Draw Calls: %ld", stats.DrawCalls);
	ImGui::Text("Batches: %ld", stats.GetTotalBatchCount());
	ImGui::Text("Polygon Count: %ld", stats.PolygonCount);
	ImGui::Text("Vertices: %ld", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %ld", stats.GetTotalIndexCount());

	ImGui::End();
}

void Sandbox3D::OnEvent(Entry::Event& event) 
{
	//m_CameraController.OnEvent(event);
}