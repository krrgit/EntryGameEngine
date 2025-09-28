// Lit Texture Shader

// FORMULAS
// Primary Color =  mat.emissive + 
//				    mat.ambient * scene.ambient + 
//					attenuation * LUT_FUNCTION * (L*N < 0 ? 0 : 1) * ShadowAttenuation * 
//					(mat.ambient * light.ambient + mat.diffuse * light.diffuse * dot(LightDir, Normal))

// Secondary Color = attenuation * spotlightFactor * ((mat.specular1 * (N*H ?) * geometric_factor0) + ((N*V?)(L*N?) *geometric_factor1)) * light.specular

#type vertex
#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Norm;
// layout(location = 3) in int a_EntityID; TODO: use when render batches are implemented

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
#define MAX_TEXENVS 6

#define RGB_A 2 // Separate RGB from A
#define RGBA 3 // RGBA Together

#define TYPE_DIRECTIONALLIGHT 0
#define TYPE_POINTLIGHT 1
#define TYPE_SPOTLIGHT 2


layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

struct Light 
{
    vec4 position;  // xyz = position in view space
	vec4 direction; // direction in view space
    vec4 color;		// rgb = color, a = intensity
    vec4 params;    // x = type, y = shininess, z = angle, w = unused
	ivec4 daLut[64];
	ivec4 spLut[64];
};

layout(std140, binding = 0) uniform LightData
{
    Light lights[MAX_LIGHTS];
	vec4 s_Ambient; // Scene Ambient
	ivec4 lightEnv_Params[MAX_LUTS]; // x = input; y = unused; z = SP cutoff; w = SP softEdgeDegrees
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

layout(std140, binding = 2) uniform TexEnvData
{
	ivec4 texEnvConfig[MAX_TEXENVS]; // x = RGBA or RGB + A
	ivec4 texEnvRGBInputs[MAX_TEXENVS]; // x = func, y = source1, z = source2, w = source3
	ivec4 texEnvAlphaInputs[MAX_TEXENVS]; // x = func, y = source1, z = source2, w = source3
};


in vec4 v_Color;
in vec2 v_TexCoord;
in vec3 v_Normal;    // in view space
in vec3 v_FragPos;   // in view space
in float v_TexIndex;
flat in int v_EntityID;

uniform sampler2D u_Textures[4];

// TexEnv Sources
vec4 primaryColor;
vec4 secondaryColor;
vec4 previous = vec4(0);

// LUT Vectors
vec3 lut_Normal;
vec3 lut_HalfVec;
vec3 lut_SpotlightVec;
vec3 lut_LightDir;
vec3 lut_ViewVec;

int fetchLUT(int lutIndex, int entryIndex)
{
    int group = entryIndex / 4;
    int comp  = entryIndex % 4;
    return luts[lutIndex * 256 + group][comp];
}

int fetchLocalLUT(int lightIndex, int lutIndex, int entryIndex)
{
    int group = entryIndex / 4;
    int comp  = entryIndex % 4;
    return lutIndex == 0 ? lights[lightIndex].daLut[group][comp] : lights[lightIndex].spLut[group][comp];
}

vec2 unpackLUT(int raw) {
    int val  = raw & 0xFFF;       // low 12 bits
    int slope = (raw >> 12) & 0xFFF;
    // handle sign: slope is signed 12-bit (range –2048..2047)
    if ((slope & 0x800) != 0) slope |= ~0xFFF;
    return vec2(float(val), float(slope));
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
	
	int raw = fetchLUT(lutId, i0);
	vec2 unpacked = unpackLUT(raw);
	float val = unpacked.x;
	float slope = unpacked.y;
	int raw_denom = fetchLUT(lutId, 255);
	float denom = unpackLUT(raw_denom).x;
	
	return (val + slope * t) / denom;
}

// Spotlight Lut
float sampleSPLutLinear(int lightIndex, float input) 
{
    float u = (input * 0.5 + 0.5);
    u = clamp(u, 0.0, 1.0);

	float fIdx = u * 255.0;
    int i0 = int(floor(fIdx));
    float t = fIdx - float(i0);

    int raw = fetchLocalLUT(lightIndex, 1, i0); // 1 = SP Lut
	int val  = ~raw & 0xFFF;       // low 12 bits
    int slope = (~raw >> 12) & 0xFFF;
    // handle sign: slope is signed 12-bit (range –2048..2047)
    if ((slope & 0x800) != 0) slope |= ~0xFFF;

    return (i0 >= 128) ? 0 : (float(val) + float(slope) * t) / 4095.0;
}

float sampleDALutLinear(int lightIndex, float input) 
{
	float u = clamp(input, 0.0, 1.0);
	float fIdx = u * 255.0;
    int i0 = int(floor(fIdx));
    float t = fIdx - float(i0);

    int raw = fetchLocalLUT(lightIndex, 0, i0); // 0 = DA Lut
	int val  = raw & 0xFFF;       // low 12 bits
    int slope = (raw >> 12) & 0xFFF;

    return fIdx < 255 ?(float(val) + float(1.0/slope) * t) / 2048.0 : 0.0;
}

float GetInput(int inputID) 
{
	switch(inputID) 
	{
	case 0: return clamp(dot(lut_Normal, lut_HalfVec), 0.0, 1.0);	///< Normal*HalfVector
	case 1: return dot(lut_ViewVec, lut_HalfVec);					///< View*HalfVector
	case 2: return dot(lut_Normal, lut_ViewVec);					///< Normal*View
	case 3: return dot(lut_LightDir, lut_Normal);					///< LightVector*Normal
	case 4: return dot(-lut_LightDir, lut_SpotlightVec);			///< -LightVector*SpotlightVector
	case 5: return 0; // TODO										///< cosine of phi
	}
	return 0;
}

vec4 GetSource(int sourceID) 
{
	switch(sourceID) 
	{
	case 0: return vec4(1.0); // What is primary color? white for now
	case 1: return primaryColor;
	case 2: return secondaryColor;
	case 3: return texture(u_Textures[0], v_TexCoord);
	case 4: return texture(u_Textures[1], v_TexCoord);
	case 5: return texture(u_Textures[2], v_TexCoord);
	case 6: return texture(u_Textures[3], v_TexCoord);
	case 13: break; // Previous Buffer ?
	case 14: break; // Constant Value ?
	case 15: return previous;
	default:
	break;
	}

	return vec4(1.0);
}

vec4 CombineSources(int combineFunc, vec4 s1, vec4 s2, vec4 s3) {
	switch(combineFunc) {
	case 0: return s1;					// REPLACE
	case 1: return s1 * s2;				// MODULATE
	case 2: return s1 + s2;				// ADD
	case 3: return s1 + s2 - vec4(0.5); // ADD_SIGNED
	case 4: return mix(s1,s2,s3);		// INTERPOLATE
	case 5: return s1 - s2;				// SUBTRACT
	case 6:								// DOT3_RGB
	    {
            float d = dot(s1.rgb * 2.0 - 1.0, s2.rgb * 2.0 - 1.0);
            return vec4(d, d, d, 1.0);
        }
	case 7:								// DOT3_RGBA 
		{
            float d = dot(s1.rgb * 2.0 - 1.0, s2.rgb * 2.0 - 1.0);
            return  vec4(d, d, d, d);
        }
	case 8: return s1 * s2 + s3;		// MULTIPLY_ADD
	case 9: return (s1 + s2) * s3;		// ADD_MULTIPLY
	}

	return s1;
}

float CombineAlpha(int combineFunc, float s1, float s2, float s3) {
    switch(combineFunc) {
    case 0: return s1;				// REPLACE
    case 1: return s1 * s2;			// MODULATE
    case 2: return s1 + s2;			// ADD
    case 3: return s1 + s2 - 0.5;	// ADD_SIGNED
    case 4: return mix(s1, s2, s3);	// INTERPOLATE
    case 5: return s1 - s2;			// SUBTRACT
    case 6: return s1 * s2;			// DOT3_RGB makes no sense here → usually MODULATE
    case 7: return s1 * s2;			// DOT3_RGBA also doesn’t apply → clamp or fallback
    case 8: return s1 * s2 + s3;	// MULTIPLY_ADD
    case 9: return (s1 + s2) * s3;	// ADD_MULTIPLY
    }
    return s1;
}

vec4 ComputeTexEnvOutput(int texEnvID) {
	// RGB
	int combineFunc = texEnvRGBInputs[texEnvID].x;
	int s1ID = texEnvRGBInputs[texEnvID].y;
	int s2ID = texEnvRGBInputs[texEnvID].z;
	int s3ID = texEnvRGBInputs[texEnvID].w;
	vec4 source1 = GetSource(s1ID);
	vec4 source2 = GetSource(s2ID);
	vec4 source3 = GetSource(s3ID);

	// Alpha
	int aCombineFunc = texEnvAlphaInputs[texEnvID].x;
	int aS1ID = texEnvAlphaInputs[texEnvID].y;
	int aS2ID = texEnvAlphaInputs[texEnvID].z;
	int aS3ID = texEnvAlphaInputs[texEnvID].w;
	float aSource1 = GetSource(aS1ID).a;
	float aSource2 = GetSource(aS2ID).a;
	float aSource3 = GetSource(aS3ID).a;

	vec4 result = CombineSources(combineFunc, source1, source2, source3);
	result.a = texEnvConfig[texEnvID].x == RGB_A ? CombineAlpha(aCombineFunc, aSource1, aSource2, aSource3) : result.a;
	previous = result;
	return result;
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
	primaryColor = m_Emissive + (m_Ambient * s_Ambient);
	secondaryColor = vec4(0,0,0,1);

	// Define light variables (REMOVE)
	float da_linear =  0.1;
	float da_quad = 0.01;
	
	// Per Fragment Vectors
	lut_ViewVec = normalize(-v_FragPos.xyz);
	lut_Normal = v_Normal;

	for(int i=0; i < MAX_LIGHTS; i++) 
	{
		// Per Light Variables
		int lightType = int(lights[i].params.x);
		float strength = lights[i].color.a;
		float sp_cutoff = lights[i].params.z; // TODO: Remove

		//Per Light Vectors
		lut_LightDir = (lights[i].position.w == 0.0) ?
						normalize(lights[i].position.xyz) :				// Directional
						normalize(lights[i].position.xyz - v_FragPos);	// PointLight/Spotlight
		lut_HalfVec = normalize(lut_LightDir + lut_ViewVec);
		lut_SpotlightVec = normalize(-lights[i].direction.xyz);

		// Diffuse
		float diff = max(dot(lut_Normal, lut_LightDir), 0.0);
		vec4 diffuse = diff * lights[i].color * strength;
		diffuse.w = 1.0;

		// Spotlight Factor
		float spotLightFactor = 1.0;
		float attenuation = 1.0;
		if (lightType == TYPE_SPOTLIGHT) {

			float theta = dot(lut_LightDir, lut_SpotlightVec);
			spotLightFactor = sampleSPLutLinear(i, GetInput(4));

			float d = length(lights[i].position.xyz - v_FragPos.xyz);
			float r = lights[i].params.y;
			attenuation = sampleDALutLinear(i,d / r);
		} else if (lightType == TYPE_POINTLIGHT) 
		{
			float d = length(lights[i].position.xyz - v_FragPos.xyz);
			float r = lights[i].params.y;
			attenuation = sampleDALutLinear(i,d / r);
		}

		primaryColor += attenuation * spotLightFactor * ((m_Diffuse * diffuse));

		// LUT Inputs
		float spec0LutInput = GetInput(lightEnv_Params[0].x);

		// Specular 0 (Blinn–Phong)
		float spec0 = clamp(sampleLUTLinear(0, spec0LutInput, false), 0.0, 1.0);
		secondaryColor += attenuation * spotLightFactor * (m_Specular0 * spec0) * lights[i].color;
	}

	vec4 outputColor = vec4(0);
	for (int i=0;i<6;i++) {
		outputColor = ComputeTexEnvOutput(i);
	}

	color = outputColor;
	color2 = v_EntityID; // Entity ID (Viewport Selection)
}