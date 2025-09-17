// Lit Texture Shader

#type vertex
#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Norm;
// layout(location = 3) in int a_EntityID; TOOD: use when render batches are implemented

uniform mat4 u_Projection;
uniform vec4 u_Color;
uniform mat4 u_ModelView;
uniform int  u_EntityID;

out vec4 v_Color;
out vec2 v_TexCoord;
out vec3 v_Normal;    // in view space
out vec3 v_FragPos;   // in view space
flat out int v_EntityID;

void main()
{
	gl_Position = u_Projection * u_ModelView * vec4(a_Position, 1.0);
	    
	vec4 viewPos = u_ModelView * vec4(a_Position, 1.0);

	v_Color = u_Color;
	v_TexCoord = a_TexCoord;
    v_Normal = normalize(mat3(u_ModelView) * a_Norm); // transform normal to view space
    v_FragPos = viewPos.xyz;
	v_EntityID = u_EntityID;
}

#type fragment
#version 420 core

// 3DS Max Light Limit
#define MAX_LIGHTS 8

#define TYPE_SPOTLIGHT 2

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

struct Light 
{
    vec4 position;   // xyz = position in view space
	vec4 direction; // direction in view space
    vec4 color; // rgb = color, a = strength
    vec4 params;     // x = type, y = shininess, z = angle, w = unused
};

layout(std140, binding = 0) uniform LightData
{
    Light lights[MAX_LIGHTS];
	vec4 s_Ambient; // Scene Ambient
};

layout(std140, binding = 1) uniform MaterialData
{
    vec4 m_Ambient;
	vec4 m_Diffuse;
	vec4 m_Specular0;
	vec4 m_Specular1;
	vec4 m_Emissive;
};

in vec4 v_Color;
in vec2 v_TexCoord;
in vec3 v_Normal;    // in view space
in vec3 v_FragPos;   // in view space
in float v_TexIndex;
flat in int v_EntityID;

uniform sampler2D u_Textures[4];

void main()
{
	vec4 texColor = v_Color;
	switch(0)
	{
		case 0: texColor *= texture(u_Textures[0], v_TexCoord); break;
		case 1: texColor *= texture(u_Textures[1], v_TexCoord); break;
		case 2: texColor *= texture(u_Textures[2], v_TexCoord); break;
		case 3: texColor *= texture(u_Textures[3], v_TexCoord); break;
	}

	vec4 primaryColor = m_Emissive + (m_Ambient * s_Ambient);

	for(int i=0; i < MAX_LIGHTS; i++) 
	{
		int lightType = int(lights[i].params.x);
		float shininess = lights[i].params.y;
		float angle = lights[i].params.z;

		// if (lights[i].params.x < 1.0) continue;
		vec3 lightDir = normalize(lights[i].position.xyz - v_FragPos);
		float diff = max(dot(v_Normal, lightDir), 0.0);
		vec4 diffuse = diff * lights[i].color;
		diffuse.w = 1.0;

		float spotLightFactor = 1.0;
		if (lightType == TYPE_SPOTLIGHT) {
			float spotLightCutoff = (90.0 - lights[i].params.z * 0.5) / 90.0;
			float outerCutoff = (90.0 - (lights[i].params.z + 2)* 0.5) / 90.0;
			float theta = dot(lightDir, normalize(-lights[i].direction.xyz));
			float epsilon = spotLightCutoff - outerCutoff;
			spotLightFactor = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0); // Soft Edge Spotlight
			// spotLightFactor = theta > spotLightCutoff ? 1.0 : 0.0; // Hard Edge Spotlight
		}

		float d = length(lights[i].position.xyz - v_FragPos.xyz) * (lightType == 1 ? 1.0 : 0.0);
		float attenuation = 1.0 / (1.0 + 0.1 * d + 0.01 * d * d);

		primaryColor += attenuation * spotLightFactor * ((texColor * m_Diffuse * diffuse));
	}
	primaryColor.a = texColor.a;

	// Primary Color =  mat.emissive + 
	//				    mat.ambient * scene.ambient + 
	//					attenuation * LUT_FUNCTION * (L*N < 0 ? 0 : 1) * ShadowAttenuation * 
	//					(mat.ambient * light.ambient + mat.diffuse * light.diffuse * dot(LightDir, Normal))

	color = primaryColor;
	color2 = v_EntityID; // Entity ID placeholder
}