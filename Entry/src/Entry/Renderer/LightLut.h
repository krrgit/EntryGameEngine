#pragma once

#include <math.h>
#include <glm/glm.hpp>

namespace Entry
{

	typedef struct
	{
		uint32_t data[256];
	} LightLut;

	typedef struct
	{
		LightLut lut;
		float bias, scale;
	} LightLutDA;

	typedef float (*LightLutFunc)(float x, float param);
	typedef float (*LightLutFuncDA)(float dist, float arg0, float arg1);

	static inline float ET_quadratic_dist_attn(float dist, float linear, float quad)
	{
		return 1.0f / (1.0f + linear * dist + quad * dist * dist);
	}

	static inline float ET_quadratic_dist_attn_falloff(float dist, float range, float arg)
	{
		float falloff = range * 0.2f;
		float fadeMult = dist < range - falloff ? 1.0f : std::max(range - dist, 0.0f) / falloff;
		return fadeMult / (1.0f + 0.1f * dist + 0.01 * dist * dist);
	}

	static inline float ET_spot_step(float angle, float cutoff)
	{
		return angle >= cutoff ? 1.0f : 0.0f;
	}

	#define NUM_DIFFUSE_TONES 3
	static float ET_toon_diffuse(float x, float arg)
	{
		const float factor = NUM_DIFFUSE_TONES - 1;
		return std::floor(0.5f + x * factor) / factor;
	}

	#define NUM_SPECULAR_TONES 2
	static float ET_toon_specular(float x, float shininess)
	{
		const float factor = NUM_SPECULAR_TONES - 1;
		return std::floor(0.5f + std::pow(x, shininess) * factor) / factor;
	}

	static float ET_zeroes(float x, float arg)
	{
		return 0;
	}

	void ET_LightLut_FromArray(LightLut* lut, float* data);
	void ET_LightLut_FromFunc(LightLut* lut, LightLutFunc func, float param, bool negative);
	void ET_LightLutDA_Create(LightLutDA* lut, LightLutFuncDA func, float from, float to, float arg0, float arg1);
	void IntArrayToIvec4Array(uint32_t* in, glm::ivec4* out);

#define ET_LightLut_Phong(lut, shininess)		ET_LightLut_FromFunc((lut), powf, (shininess), false)
#define ET_LightLut_Spotlight(lut, angle)		ET_LightLut_FromFunc((lut), ET_spot_step, cosf(angle), true)
#define ET_LightLut_ToonDiffuse(lut, arg)		ET_LightLut_FromFunc((lut), ET_toon_diffuse, arg, false)
#define ET_LightLut_ToonSpecular(lut, angle)	ET_LightLut_FromFunc((lut), ET_toon_specular, arg, false)
#define ET_LightLut_Zeroes(lut, arg)			ET_LightLut_FromFunc((lut), ET_zeroes, arg, false)
#define ET_LightLutDA_Quadratic(lut, from, to, linear, quad) ET_LightLutDA_Create((lut), ET_quadratic_dist_attn, (from), (to), (linear), (quad))
#define ET_LightLutDA_Quadratic_Falloff(lut, from, to, range, arg) ET_LightLutDA_Create((lut), ET_quadratic_dist_attn_falloff, (from), (to), (range), (arg))
}