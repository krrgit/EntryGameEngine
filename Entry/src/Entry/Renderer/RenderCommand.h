#pragma once

#include "RendererAPI.h"

#include <glm/glm.hpp>

namespace Entry {
	class RenderCommand
	{
	public:
		inline static void Init() {
			s_RendererAPI->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, void* window)
		{
			s_RendererAPI->SetViewport(x, y, width, height, window);
		}

		inline static void SetClearColor(const uint32_t color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear() {
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint16_t indexCount = 0, uint16_t indexOffset = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount, indexOffset);
		}

		inline static void DrawLines(const Ref<VertexArray>& vertexArray, uint16_t vertexCount)
		{
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}

		inline static void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}