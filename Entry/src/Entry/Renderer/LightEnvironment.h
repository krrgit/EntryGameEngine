#pragma once

#include "Entry/Core/Core.h"
#include "Material.h"
#include "Entry/Renderer/Light.h"
#include "Entry/Renderer/LightLut.h"

// 3DS Max Light Limit
#define MAX_LIGHTS 8
#define MAX_LUTS 6
#define MAX_TEXENV 6

namespace Entry
{
	/// LUT IDs.
	typedef enum
	{
		ET_LUT_D0 = 0, ///< D0 LUT.
		ET_LUT_D1 = 1, ///< D1 LUT.
		ET_LUT_SP = 2, ///< Spotlight LUT.
		ET_LUT_FR = 3, ///< Fresnel LUT.
		ET_LUT_RB = 4, ///< Reflection-Blue LUT.
		ET_LUT_RG = 5, ///< Reflection-Green LUT.
		ET_LUT_RR = 6, ///< Reflection-Red LUT.
		ET_LUT_DA = 7, ///< Distance attenuation LUT.
	} ET_LIGHTLUTID;

	/// LUT inputs.
	typedef enum
	{
		ET_LUTINPUT_NH = 0, ///< Normal*HalfVector
		ET_LUTINPUT_VH = 1, ///< View*HalfVector
		ET_LUTINPUT_NV = 2, ///< Normal*View
		ET_LUTINPUT_LN = 3, ///< LightVector*Normal
		ET_LUTINPUT_SP = 4, ///< -LightVector*SpotlightVector
		ET_LUTINPUT_CP = 5, ///< cosine of phi
	} ET_LIGHTLUTINPUT;

	enum class LutFuncType
	{
		None,
		Pow,			// val = pow(x, arg0)
		Spotlight,		// hardEdge ? (angle >= cutoff ? 1.0f : 0.0f) : (clamp((theta - cutoff) / epsilon, 0.0, 1.0))
		ToonDiffuse,    // val = step(arg0, x)
		ToonSpecular,   // val = step(arg0, x)
		Custom			// user-defined callback
	};

	struct LutFuncArgs
	{
		float powExponent = 30.0f;
		float spotlightCutoff = 90.0f;
		float toonShininess = 0.5f;
	};

	struct LutConfig
	{
		ET_LIGHTLUTID id;
		ET_LIGHTLUTINPUT input = ET_LIGHTLUTINPUT::ET_LUTINPUT_NV;
		bool negative = false;

		LutFuncType funcType = LutFuncType::None;
		LutFuncArgs funcArgs;
	};

	class LightEnvironment
	{
	public:
		virtual ~LightEnvironment() = default;

		virtual void Bind() = 0;
		virtual void BindMaterial(Material* material) = 0;

		virtual int LightInit(Ref<Light> light) = 0;
		virtual void LightDestroy(Ref<Light> light) = 0;

		virtual int GetLightCount() = 0;

		virtual void SetSceneAmbientColor(glm::vec3 color) = 0;
		virtual glm::vec3 GetSceneAmbientColor() = 0;

		virtual void ConfigureLut(LutConfig& config) = 0;
		virtual LutConfig& GetLutConfig(ET_LIGHTLUTID id) = 0;

		static Ref<LightEnvironment> Create();
	};
}