#pragma once

#include "Entry/Core/Core.h"
#include "Material.h"
#include "Entry/Renderer/Light.h"

// 3DS Max Light Limit
#define MAX_LIGHTS 8

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

	class LightEnvironment
	{
	public:
		virtual ~LightEnvironment() = default;

		virtual void Bind() = 0;
		virtual void SetMaterial(Ref<Material> material) = 0;

		virtual int LightInit(Ref<Light> light) = 0;
		virtual void LightDestroy(Ref<Light> light) = 0;

		virtual int GetLightCount() = 0;

		virtual void SetSceneAmbientColor(glm::vec3 color) = 0;
		virtual glm::vec3 GetSceneAmbientColor() = 0;

		static Ref<LightEnvironment> Create();
	};
}