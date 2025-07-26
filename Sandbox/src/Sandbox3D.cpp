#include "Sandbox3D.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <chrono>

// Texture Headers
#include "Checkerboard_t3x.h"

template<typename Fn>
class Timer {
	public:
		Timer(const char* name, Fn&& func)
			: m_Name(name), m_Func(func), m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::steady_clock::now();
		}
	
		~Timer()
		{
			if (!m_Stopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::steady_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			m_Stopped = true;
			float duration = (end - start) * 0.001f;
			
			m_Func({ m_Name, duration });
		}

	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		Fn m_Func;
		bool m_Stopped;
};

template<typename Fn>
Timer<Fn> MakeTimer(const char* name, Fn&& func) {
	return Timer<Fn>(name, std::forward<Fn>(func));
}

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#define PROFILE_SCOPE(name) \
    auto CONCAT(timer, __LINE__) = MakeTimer(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })



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
	PROFILE_SCOPE("Sandbox3D::OnUpdate");
	
	// Update
	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}


	{
		PROFILE_SCOPE("Renderer Draw");

		m_Rotation += ts.GetSeconds();
		m_Rotation = m_Rotation > 6.28f ? 0 : m_Rotation;

		//Render
		Entry::Renderer3D::BeginScene(m_CameraController.GetCamera());
		Entry::Renderer3D::DrawQuad({ 0.0f, -0.5f, 0.0f }, glm::quat(glm::vec3(glm::radians(90.0f),0,0)), {10.0f, 10.0f, 1.0f}, m_CheckerboardTexture);
		Entry::Renderer3D::DrawQuad({ 2.0f, 1.0f, 0.0f }, glm::quat(glm::vec3(0)), { 1.0f, 1.0f, 1.0f }, m_SquareColor);
		Entry::Renderer3D::DrawCube({ 0.0f, 0.0f, 0.0f },  glm::quat(glm::vec3(0)), { 1.0f, 1.0f, 1.0f }, m_SquareColor);
		Entry::Renderer3D::DrawCube({-2.0f, 1.0f, -2.0f},  glm::quat(glm::vec3(m_Rotation, m_Rotation, 0)), {1.0f, 1.0f, 1.0f}, m_CheckerboardTexture);
		Entry::Renderer3D::EndScene();
	}
}

void Sandbox3D::OnImGuiRender() 
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	//ImGui::Text("Rotation: %.2f\n", m_Rotation);

	for (auto& result : m_ProfileResults) {
		ImGui::Text("%.3fms %s\n", result.Time, result.Name);
	}
	m_ProfileResults.clear();

	ImGui::End();
}

void Sandbox3D::OnEvent(Entry::Event& event) 
{
	//m_CameraController.OnEvent(event);
}