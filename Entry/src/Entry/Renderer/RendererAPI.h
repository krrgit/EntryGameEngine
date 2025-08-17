#pragma once

#include "VertexArray.h"

namespace Entry {
	class RendererAPI 
	{
	public:
		enum class API
		{
			None = 0, Citro3D = 1, OpenGL = 2
		};

	public:
		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, void* window) = 0; // for Windows/Editor
		virtual void SetClearColor(const uint32_t color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint16_t indexCount = 0) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}