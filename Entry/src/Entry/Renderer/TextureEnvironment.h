#pragma once

#ifndef BIT
#define BIT(n) (1U<<(n))
#endif

#include <vector>

namespace Entry
{
	// from citro3d (enums.h)
	typedef enum
	{
		ET_RGB = BIT(0),
		ET_Alpha = BIT(1),
		ET_RGBA = ET_RGB | ET_Alpha,
		ET_RGBA_Separate = 4,
	} TexEnvChannels;

	/// Texture combiner functions.
	typedef enum
	{
		ET_GPU_REPLACE = 0x00, ///< Replace.
		ET_GPU_MODULATE = 0x01, ///< Modulate.
		ET_GPU_ADD = 0x02, ///< Add.
		ET_GPU_ADD_SIGNED = 0x03, ///< Signed add.
		ET_GPU_INTERPOLATE = 0x04, ///< Interpolate.
		ET_GPU_SUBTRACT = 0x05, ///< Subtract.
		ET_GPU_DOT3_RGB = 0x06, ///< Dot3. Scalar result is written to RGB only.
		ET_GPU_DOT3_RGBA = 0x07, ///< Dot3. Scalar result is written to RGBA.
		ET_GPU_MULTIPLY_ADD = 0x08, ///< Multiply then add. ((s1 * s2) + s3)
		ET_GPU_ADD_MULTIPLY = 0x09, ///< Add then multiply. ((s1 + s2) * s3)
	} TexEnvBlendMode;

	typedef enum
	{
		ET_GPU_PRIMARY_COLOR = 0x00, ///< Primary color.
		ET_GPU_FRAGMENT_PRIMARY_COLOR = 0x01, ///< Primary fragment color.
		ET_GPU_FRAGMENT_SECONDARY_COLOR = 0x02, ///< Secondary fragment color.
		ET_GPU_TEXTURE0 = 0x03, ///< Texture unit 0.
		ET_GPU_TEXTURE1 = 0x04, ///< Texture unit 1.
		ET_GPU_TEXTURE2 = 0x05, ///< Texture unit 2.
		ET_GPU_TEXTURE3 = 0x06, ///< Texture unit 3.
		ET_GPU_PREVIOUS_BUFFER = 0x0D, ///< Previous buffer.
		ET_GPU_CONSTANT = 0x0E, ///< Constant value.
		ET_GPU_PREVIOUS = 0x0F, ///< Previous value.
	} TexEnvSource;

	struct TexEnvProps
	{
		TexEnvChannels Channels   = TexEnvChannels::ET_RGBA;
		TexEnvBlendMode BlendMode = TexEnvBlendMode::ET_GPU_REPLACE;
		TexEnvSource Source1 = TexEnvSource::ET_GPU_PREVIOUS;
		TexEnvSource Source2 = TexEnvSource::ET_GPU_PRIMARY_COLOR;
		TexEnvSource Source3 = TexEnvSource::ET_GPU_PRIMARY_COLOR;

		TexEnvBlendMode AlphaBlendMode = TexEnvBlendMode::ET_GPU_REPLACE;
		TexEnvSource AlphaSource1 = TexEnvSource::ET_GPU_PREVIOUS;
		TexEnvSource AlphaSource2 = TexEnvSource::ET_GPU_PRIMARY_COLOR;
		TexEnvSource AlphaSource3 = TexEnvSource::ET_GPU_PRIMARY_COLOR;

		TexEnvProps() = default;
		TexEnvProps(TexEnvChannels channels,
			TexEnvBlendMode blendMode,
			TexEnvSource source1,
			TexEnvSource source2,
			TexEnvSource source3)
			: Channels(channels), BlendMode(blendMode),
			Source1(source1), Source2(source2), Source3(source3)
		{
		}

		TexEnvProps(
			TexEnvChannels channels,
			TexEnvBlendMode blendMode,
			TexEnvSource source1,
			TexEnvSource source2,
			TexEnvSource source3,
			TexEnvBlendMode alphaBlendMode,
			TexEnvSource alphaSource1,
			TexEnvSource alphaSource2,
			TexEnvSource alphaSource3)
			: Channels(channels), BlendMode(blendMode),
			Source1(source1), Source2(source2), Source3(source3),
			AlphaBlendMode(alphaBlendMode), AlphaSource1(alphaSource1), 
			AlphaSource2(alphaSource2), AlphaSource3(alphaSource3)
		{
		}
	};

	struct TexEnvsSetup
	{
		std::vector<TexEnvProps> texEnvs;

		void Add(TexEnvProps& newTexEnv)
		{
			if (texEnvs.size() >= 6)
				ET_CORE_ASSERT("Tex Env Limit Reached.");

			texEnvs.push_back(newTexEnv);
		}

		void Remove()
		{
			texEnvs.pop_back();
		}

		void Set(TexEnvProps& props, int id)
		{
			texEnvs[id] = props;
		}

		TexEnvsSetup(const std::initializer_list<TexEnvProps>& _texEnvs)
			: texEnvs(_texEnvs)
		{
		}
	};

	// Preset TexEnvProps
	static TexEnvProps s_TexEnvLitProps[6]
	{
		{
			// RGB
			ET_RGBA_Separate, ET_GPU_MULTIPLY_ADD, ET_GPU_TEXTURE0, ET_GPU_FRAGMENT_PRIMARY_COLOR, ET_GPU_FRAGMENT_SECONDARY_COLOR,
			// Alpha
			ET_GPU_REPLACE, ET_GPU_TEXTURE0, ET_GPU_PRIMARY_COLOR, ET_GPU_PRIMARY_COLOR
		}
	};


	static TexEnvProps s_TexEnvUnlitProps[6]
	{
		{
			TexEnvChannels::ET_RGBA,
			TexEnvBlendMode::ET_GPU_MODULATE,
			TexEnvSource::ET_GPU_TEXTURE0,
			TexEnvSource::ET_GPU_PRIMARY_COLOR,
			TexEnvSource::ET_GPU_PRIMARY_COLOR
		}
	};
}