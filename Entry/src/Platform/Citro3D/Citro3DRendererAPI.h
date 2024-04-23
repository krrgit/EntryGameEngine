#pragma once

#include "Entry/Window.h"
#include "Entry/Renderer/RendererAPI.h"

namespace Entry {
	class Citro3DRendererAPI : public RendererAPI {
	public:
		virtual void SetClearColor(const uint32_t color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	private:
		uint32_t m_ClearColor;
		C3D_RenderTarget* m_RenderTarget;
	};
}