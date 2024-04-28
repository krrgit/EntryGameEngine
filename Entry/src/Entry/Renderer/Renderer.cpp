
#include "etpch.h"
#include "Renderer.h"

#include "Platform/Citro3D/Citro3DShader.h"

namespace Entry {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(PerspectiveCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = *camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const C3D_Mtx& transform)
	{

		shader->Bind();
		std::static_pointer_cast<Citro3DShader>(shader)->UploadUniformMat4("u_ViewProjection", &m_SceneData->ViewProjectionMatrix);
		std::static_pointer_cast<Citro3DShader>(shader)->UploadUniformMat4("u_Transform", &transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}