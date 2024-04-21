
#include "etpch.h"
#include "Renderer.h"

namespace Entry {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(PerspectiveCamera& camera)
	{
		m_SceneData->ProjectionMatrix = *camera.GetProjectionMatrix();
		m_SceneData->ViewMatrix = camera.GetViewMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{

		C3D_Mtx modelView;
		Mtx_Identity(&modelView);
		Mtx_Translate(&modelView, 0.0f, 0.0f, 1.0f, true);
		Mtx_Inverse(&modelView);

		shader->Bind();
		shader->UploadUniformMat4("u_Projection", &m_SceneData->ProjectionMatrix);
		shader->UploadUniformMat4("u_ModelView", &m_SceneData->ViewMatrix);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}