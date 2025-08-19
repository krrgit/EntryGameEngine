#pragma once

#include "Entry/Core/Window.h"
#include "Entry/Renderer/RendererAPI.h"

namespace Entry {
	class OpenGLRendererAPI : public RendererAPI {
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, void* window) override;

		virtual void SetClearColor(const uint32_t color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint16_t indexCount = 0) override;
	private:
	};
}