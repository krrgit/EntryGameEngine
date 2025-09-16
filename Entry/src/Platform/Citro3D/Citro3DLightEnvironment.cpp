#include "Citro3DLightEnvironment.h"

namespace Entry
{
	Citro3DLightEnvironment::Citro3DLightEnvironment()
	{
		C3D_LightEnvInit(&m_LightEnv);
	}

	void Citro3DLightEnvironment::Bind()
	{
		C3D_LightEnvBind(&m_LightEnv);
	}

	void Citro3DLightEnvironment::SetMaterial(Ref<Material> material)
	{
		C3D_Material* mat = reinterpret_cast<C3D_Material*>(&material->GetProps().Values);
		C3D_LightEnvMaterial(&m_LightEnv, mat);
	}

	int Citro3DLightEnvironment::LightInit(Ref<Light> light)
	{
		int id = C3D_LightInit((C3D_Light*)light->GetNativeLight(), &m_LightEnv);
		if (id == -1) return -1;

		light->SetID(id);
		light->SetParent((uint32_t)this);

		m_LightCount++;
		return 1;
	}

	void Citro3DLightEnvironment::LightDestroy(Ref<Light> light)
	{
		if (light->GetID() == -1) return;

		m_LightCount--;
	}

	void Citro3DLightEnvironment::SetSceneAmbientColor(glm::vec3 color)
	{
		C3D_LightEnvAmbient(&m_LightEnv, color.r, color.g, color.b);
	}
}
