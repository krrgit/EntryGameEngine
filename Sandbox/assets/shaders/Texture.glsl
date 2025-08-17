// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform float u_TilingFactor;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TilingFactor;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TilingFactor = u_TilingFactor;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;

uniform sampler2D u_Textures[4];

void main()
{
	vec4 texColor = v_Color;
//	switch(int(v_TexIndex))
//	{
//		case 0: texColor *= texture(u_Textures[0], v_TexCoord); break;
//		case 1: texColor *= texture(u_Textures[1], v_TexCoord); break;
//		case 2: texColor *= texture(u_Textures[2], v_TexCoord); break;
//		case 3: texColor *= texture(u_Textures[3], v_TexCoord); break;
//	}
	texColor *= texture(u_Textures[0], v_TexCoord * v_TilingFactor);
	color = texColor;
}