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
#define MAX_LUTS 6

#define TYPE_DIRECTIONALLIGHT 0
#define TYPE_POINTLIGHT 1
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
	vec4 lightEnv_Params; // x = DA linear; y = DA quad; z = SP cutoff; w = SP softEdgeDegrees
	ivec4 luts[MAX_LUTS * 64];
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

int fetchLUT(int lutIndex, int entryIndex)
{
    int group = entryIndex / 4;
    int comp  = entryIndex % 4;
    return luts[lutIndex * 256 + group][comp];
}

float sampleLUTLinear(int lutId, float input, bool negative)
{
	// Emulating how the 3DS samples and interpolates luts.
    float u = negative ? (input * 0.5 + 0.5) : input;
    u = clamp(u, 0.0, 1.0);

    float fIdx = u * 255.0;
    int i0 = int(floor(fIdx));
    int i1 = min(i0 + 1, 255);
    float t = fIdx - float(i0);

    int v0 = fetchLUT(lutId, i0);
    int v1 = fetchLUT(lutId, i1);
	int denom = fetchLUT(lutId, 255); // divide by max value to map from [0,1]

    return mix(float(v0), float(v1), t) / float(denom);
}

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

	// Initialize Colors
	vec4 primaryColor = m_Emissive + (m_Ambient * s_Ambient);
	vec4 secondaryColor = vec4(0);

	// Define light variables (REMOVE)
	float da_linear =  0.1;
	float da_quad = 0.01;

	vec3 viewVec = normalize(-v_FragPos.xyz);

	for(int i=0; i < MAX_LIGHTS; i++) 
	{
		// Per Light Variables
		int lightType = int(lights[i].params.x);
		float shininess = lights[i].params.y;
		float sp_cutoff = lights[i].params.z;

		//Per Light Vectors
		vec3 lightDir = (lights[i].position.w == 0.0) ?
						normalize(lights[i].position.xyz) :				// Directional
						normalize(lights[i].position.xyz - v_FragPos);	// PointLight/Spotlight
		vec3 halfVec = normalize(lightDir + viewVec);
		vec3 spotlightVec = normalize(-lights[i].direction.xyz);

		// Diffuse
		float diff = max(dot(v_Normal, lightDir), 0.0);
		vec4 diffuse = diff * lights[i].color;
		diffuse.w = 1.0;

		// Spotlight Factor
		float spotLightFactor = 1.0;
		float attenuation = 1.0;
		if (lightType == TYPE_SPOTLIGHT) {
			float cutoff = (180.0 - sp_cutoff) / 180.0;
			float theta = dot(lightDir, spotlightVec);
			float epsilon = 0;
			// spotLightFactor = clamp((theta - cutoff) / epsilon, 0.0, 1.0); // Soft Edge Spotlight
			spotLightFactor = theta >= cutoff ? 1.0 : 0.0; // Hard Edge Spotlight

			float d = length(lights[i].position.xyz - v_FragPos.xyz);
			attenuation = 1.0 / (1.0 + da_linear * d + da_quad * d * d);
		} else if (lightType == TYPE_POINTLIGHT) 
		{
			float d = length(lights[i].position.xyz - v_FragPos.xyz);
			attenuation = 1.0 / (1.0 + da_linear * d + da_quad * d * d);
		}

		primaryColor += attenuation * spotLightFactor * ((m_Diffuse * diffuse));

		// LUT Inputs
		float nh = clamp(dot(v_Normal, halfVec), 0.0, 1.0); ///< Normal*HalfVector
		float vh = dot(viewVec, halfVec); ///< View*HalfVector
		float nv = dot(v_Normal, viewVec); ///< Normal*View
		float ln = dot(lightDir, v_Normal); ///< LightVector*Normal
		float sp = dot(-lightDir, spotlightVec); ///< -LightVector*SpotlightVector
		float cp; ///< cosine of phi

		// Specular 0 (Blinn–Phong)
		float spec0 = clamp(sampleLUTLinear(0, nh, false), 0.0, 1.0);
		secondaryColor += attenuation * spotLightFactor * (m_Specular0 * spec0) * lights[i].color;
	}
	primaryColor *= texColor;
	primaryColor += secondaryColor;
	primaryColor.a = texColor.a;

	// Primary Color =  mat.emissive + 
	//				    mat.ambient * scene.ambient + 
	//					attenuation * LUT_FUNCTION * (L*N < 0 ? 0 : 1) * ShadowAttenuation * 
	//					(mat.ambient * light.ambient + mat.diffuse * light.diffuse * dot(LightDir, Normal))

	// Secondary Color = attenuation * spotlightFactor * ((mat.specular1 * (N*H ?) * geometric_factor0) + ((N*V?)(L*N?) *geometric_factor1)) * light.specular
	//  Color = attenuation * spotlightFactor * ((mat.specular1 * (N*H ?)) + ((N*V?) * (L*N?)) ) * light.specular

	color = primaryColor;
	color2 = v_EntityID; // Entity ID placeholder
}