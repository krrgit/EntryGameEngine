// Renderer Grid Shader

#type vertex
#version 420 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;
uniform vec3 u_CameraWorldPos;
uniform float u_GridSize = 100.0;

out vec3 v_WorldPos;
out vec3 v_CameraWorldPos;
out float v_GridSize;

void main()
{
	vec3 vPos = a_Position * u_GridSize;
	vPos.x += u_CameraWorldPos.x; 
	vPos.z += u_CameraWorldPos.z; 
	gl_Position = u_Projection * u_ModelView * vec4(vPos, 1.0);

	v_WorldPos = vPos;
    v_CameraWorldPos = u_CameraWorldPos;
    v_GridSize = u_GridSize;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

in vec3 v_WorldPos;
in vec3 v_CameraWorldPos;
in float v_GridSize;

uniform float g_GridMinPixelsBetweenCells = 2.0;
uniform float g_GridCellSize = 1.0;
uniform vec4 g_GridColorThin = vec4(1.0, 1.0, 1.0, 0.5);
uniform vec4 g_GridColorThick = vec4(1.0, 1.0, 1.0, 0.75);

float log10(float x)
{
    float f = log(x) / log(10.0);
    return f;
}

float satf(float x)
{
    float f = clamp(x, 0.0, 1.0);
    return f;
}

vec2 satv(vec2 x)
{
    vec2 v = clamp(x, vec2(0.0), vec2(1.0));
    return v;
}

float max2(vec2 v)
{
    float f = max(v.x, v.y);
    return f;
}

void main()
{
	vec2 dvx = vec2(dFdx(v_WorldPos.x), dFdy(v_WorldPos.x));
    vec2 dvy = vec2(dFdx(v_WorldPos.z), dFdy(v_WorldPos.z));

    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);

    float l = length(dudv);

    float LOD = max(0.0, log10(l * g_GridMinPixelsBetweenCells / g_GridCellSize) + 1.0);

    float GridCellSizeLod0 = g_GridCellSize * pow(10.0, floor(LOD));
    float GridCellSizeLod1 = GridCellSizeLod0 * 10.0;
    float GridCellSizeLod2 = GridCellSizeLod1 * 10.0;

    dudv *= 1.5;

    vec2 mod_div_dudv = mod(v_WorldPos.xz, GridCellSizeLod0) / dudv;
    float Lod0a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

    mod_div_dudv = mod(v_WorldPos.xz, GridCellSizeLod1) / dudv;
    float Lod1a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)) );
    
    mod_div_dudv = mod(v_WorldPos.xz, GridCellSizeLod2) / dudv;
    float Lod2a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

    float LOD_fade = fract(LOD);
    vec4 Color;

    if (Lod2a > 0.0) {
        Color = g_GridColorThick;
        //Color.a *= Lod2a;
    } else {
        if (Lod1a > 0.0) {
            Color = g_GridColorThick; //mix(g_GridColorThick, g_GridColorThin, LOD_fade);
	        //Color.a *= Lod1a;
        } else {
            Color = g_GridColorThin;
	        //Color.a *= (Lod0a * (1.0 - LOD_fade));
            Color.a *= (Lod0a * (1.0 - LOD_fade));
        }
    }
    
    float OpacityFalloff = (1.0 - satf(length(v_WorldPos.xz - v_CameraWorldPos.xz) / v_GridSize));

    Color.a *= OpacityFalloff;

	o_Color = Color;
}