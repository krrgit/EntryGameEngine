#include "etpch.h"
#include "Citro3DTexture.h"

#include <algorithm>

#define MIN_TEX_DIMENSION 8

namespace Entry {

	// Helper function for loading a texture from memory
	static bool loadTextureFromMem(C3D_Tex* tex, C3D_TexCube* cube, const void* data, size_t size)
	{
		ET_PROFILE_FUNCTION();

		Tex3DS_Texture t3x = Tex3DS_TextureImport(data, size, tex, cube, false);
		if (!t3x)
			return false;

		ET_CORE_TRACE("Loaded texture file");
		// Delete the t3x object since we don't need it
		Tex3DS_TextureFree(t3x);
		return true;
	}

	Citro3DTexture2D::Citro3DTexture2D(uint32_t width, uint32_t height) 
		: m_Width(width > MIN_TEX_DIMENSION ? width: MIN_TEX_DIMENSION), m_Height(height > MIN_TEX_DIMENSION ? height: MIN_TEX_DIMENSION)
	{
		ET_PROFILE_FUNCTION();

		m_DataFormat = GPU_RGBA8;

		C3D_TexInitVRAM(&m_Texture, m_Width, m_Height, m_DataFormat);
		C3D_TexSetFilter(&m_Texture, GPU_NEAREST, GPU_NEAREST);
		C3D_TexSetWrap(&m_Texture, GPU_REPEAT, GPU_REPEAT);

		// Temp
		std::fill_n((uint32_t*)m_Texture.data, m_Width * m_Height, 0xffffffff);
	}

	Citro3DTexture2D::Citro3DTexture2D(const void* data, size_t size)
	{
		ET_PROFILE_FUNCTION();

		if (!loadTextureFromMem(&m_Texture, NULL, data, size))
			svcBreak(USERBREAK_PANIC);
		C3D_TexSetFilter(&m_Texture, GPU_NEAREST, GPU_NEAREST);
		C3D_TexSetWrap(&m_Texture, GPU_REPEAT, GPU_REPEAT);
	}

	Citro3DTexture2D::~Citro3DTexture2D()
	{
		ET_PROFILE_FUNCTION();

	}

	void Citro3DTexture2D::SetData(void* data, uint32_t size)
	{
		ET_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GPU_RGBA8 ? 4 : 3; // bytes per pixel
		ET_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		C3D_TexUpload(&m_Texture, data);
	}

	void Citro3DTexture2D::Bind(uint32_t slot)
	{
		ET_PROFILE_FUNCTION();

		C3D_TexEnv* env = C3D_GetTexEnv(0);
		C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

		C3D_TexBind(slot, &m_Texture);
	}
}