
#include "etpch.h"
#include "Renderer.h"
#include "Renderer3D.h"

#include "Platform/Citro3D/Citro3DShader.h"

namespace Entry {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init() 
	{
		ET_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer3D::Init();
	}

	void Renderer::BeginScene(PerspectiveCamera& camera)
	{
		ET_PROFILE_FUNCTION();

		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();

		shader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}