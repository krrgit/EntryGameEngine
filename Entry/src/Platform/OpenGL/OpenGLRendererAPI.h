#pragma once

#include "Entry/Core/Window.h"
#include "Entry/Renderer/RendererAPI.h"

namespace Entry {
	class OpenGLRendererAPI : public RendererAPI {
	public:
		virtual void Init() override;
		virtual void SetClearColor(const uint32_t color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint16_t indexCount = 0) override;
	private:
		uint32_t m_ClearColor;
	};
}