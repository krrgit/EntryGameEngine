#pragma once

#include "VertexArray.h"

namespace Entry {
	class RendererAPI 
	{
	public:
		enum class API
		{
			None = 0, Citro3D = 1
		};

	public:
		virtual void SetClearColor(const uint32_t color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}