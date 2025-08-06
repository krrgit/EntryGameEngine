#pragma once

#include "RendererAPI.h"

namespace Entry {
	class RenderCommand
	{
	public:
		inline static void Init() {
			s_RendererAPI->Init();
		}
		inline static void SetClearColor(const uint32_t color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear() {
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint16_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}