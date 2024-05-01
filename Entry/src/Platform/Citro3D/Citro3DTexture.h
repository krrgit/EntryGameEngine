#pragma once

#include "Entry/Renderer/Texture.h"

namespace Entry {
	class Citro3DTexture2D : public Texture2D 
	{
	public:
		Citro3DTexture2D(const void* data, size_t size);
		virtual ~Citro3DTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(uint32_t slot = 0) override;
	private:
		uint32_t m_Width, m_Height;
		int m_RendererID;

		C3D_Tex m_Texture;
	};

}