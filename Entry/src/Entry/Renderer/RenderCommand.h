#pragma once

#include "RendererAPI.h"

namespace Entry {
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const uint32_t color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear() {
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}