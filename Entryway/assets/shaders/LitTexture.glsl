// Unlit Texture Shader

#type vertex
#version 330 core

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
flat out int v_EntityID;

void main()
{
	gl_Position = u_Projection * u_ModelView * vec4(a_Position, 1.0);
	
	v_Color = u_Color;
	v_TexCoord = a_TexCoord;
	v_EntityID = u_EntityID;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_Color;
in vec2 v_TexCoord;
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
	color = texColor;
	color2 = v_EntityID; // Entity ID placeholder
}