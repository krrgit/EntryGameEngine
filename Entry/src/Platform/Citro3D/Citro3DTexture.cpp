#include "etpch.h"
#include "Citro3DTexture.h"
#include "Entry/Utils/StringUtils.h"

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

	// Helper function for loading a texture from a t3x file
	static bool loadTextureFromFile(C3D_Tex* tex, C3D_TexCube* cube, const char* path)
	{
		FILE* f = fopen(path, "rb");
		if (!f)
			return false;

		Tex3DS_Texture t3x = Tex3DS_TextureImportStdio(f, tex, cube, false);
		fclose(f);
		if (!t3x)
			return false;

		// Delete the t3x object since we don't need it
		Tex3DS_TextureFree(t3x);
		return true;
	}

	bool GFXTexturePath(std::string& filename) {
		const std::string prefix = "romfs:/gfx/";
		const std::string from = ".png";
		const std::string to = ".t3x";

		// Extract the filename (strip the path)
		size_t lastSlashPos = filename.find_last_of("/\\");
		if (lastSlashPos != std::string::npos)
			filename = filename.substr(lastSlashPos + 1);

		// Replace .png with .t3x if present
		size_t pos = filename.rfind(from);
		if (pos != std::string::npos && pos == filename.size() - from.size()) {
			filename.replace(pos, from.size(), to);

			filename = prefix + filename;
			return true;
		}

		ET_CORE_ERROR("%s: Only .png texture files are supported!", filename.c_str());
		return false;
	}


	Citro3DTexture2D::Citro3DTexture2D(uint32_t width, uint32_t height) 
		: m_Width(width > MIN_TEX_DIMENSION ? width: MIN_TEX_DIMENSION), m_Height(height > MIN_TEX_DIMENSION ? height: MIN_TEX_DIMENSION)
	{
		ET_PROFILE_FUNCTION();

		ET_CORE_ASSERT(width < MIN_TEX_DIMENSION || height < MIN_TEX_DIMENSION, "Texture size too small. Minimum Size: 8x8");


		m_DataFormat = GPU_RGBA8;

		C3D_TexInitVRAM(&m_Texture, m_Width, m_Height, m_DataFormat);
		C3D_TexSetFilter(&m_Texture, GPU_NEAREST, GPU_NEAREST);
		C3D_TexSetWrap(&m_Texture, GPU_REPEAT, GPU_REPEAT);

		// Temp
		std::fill_n((uint32_t*)m_Texture.data, m_Width * m_Height, 0xffffffff);
	}

	Citro3DTexture2D::Citro3DTexture2D(const std::string& path)
	{
		// Note: to use, copy .t3x from /build to same filepath as texture file
		std::string gfxPath = path;
		bool correctFormat = GFXTexturePath(gfxPath);

		m_Name = ExtractFileName(path);

		ET_CORE_INFO("Texture: {0}", gfxPath.c_str());

		if (!correctFormat) return;

		if (!loadTextureFromFile(&m_Texture, NULL, gfxPath.c_str())) {
			ET_CORE_INFO("Could not load texture: \%s", gfxPath.c_str());
			//svcBreak(USERBREAK_PANIC);
		}
		C3D_TexSetFilter(&m_Texture, GPU_NEAREST, GPU_NEAREST);
		C3D_TexSetWrap(&m_Texture, GPU_REPEAT, GPU_REPEAT);
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

		//uint32_t bpp = m_DataFormat == GPU_RGBA8 ? 4 : 3; // bytes per pixel
		ET_CORE_ASSERT(size == m_Width * m_Height * (GPU_RGBA8 ? 4 : 3), "Data must be entire texture!");
		C3D_TexUpload(&m_Texture, data);
	}

	void Citro3DTexture2D::Bind(uint32_t slot)
	{
		ET_PROFILE_FUNCTION();

		C3D_TexEnv* env = C3D_GetTexEnv(0);

		C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_FRAGMENT_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

		C3D_TexBind(slot, &m_Texture);
	}
}