#include "etpch.h"
#include "Citro3DTexture.h"


namespace Entry {

	// Helper function for loading a texture from memory
	static bool loadTextureFromMem(C3D_Tex* tex, C3D_TexCube* cube, const void* data, size_t size)
	{
		Tex3DS_Texture t3x = Tex3DS_TextureImport(data, size, tex, cube, false);
		if (!t3x)
			return false;

		ET_CORE_TRACE("Loaded texture file");
		// Delete the t3x object since we don't need it
		Tex3DS_TextureFree(t3x);
		return true;
	}

	Citro3DTexture2D::Citro3DTexture2D(const void* data, size_t size)
	{
		if (!loadTextureFromMem(&m_Texture, NULL, data, size))
			svcBreak(USERBREAK_PANIC);
		C3D_TexSetFilter(&m_Texture, GPU_NEAREST, GPU_NEAREST);
	}

	Citro3DTexture2D::~Citro3DTexture2D()
	{

	}

	void Citro3DTexture2D::Bind(uint32_t slot)
	{
		C3D_TexEnv* env = C3D_GetTexEnv(0);
		C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

		C3D_TexBind(slot, &m_Texture);
	}
}