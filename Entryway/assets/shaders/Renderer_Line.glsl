// Unlit Texture Shader

#type vertex
#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 3) in int a_EntityID;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;

out vec4 v_Color;
flat out int v_EntityID;

void main()
{
	gl_Position = u_Projection * u_ModelView * vec4(a_Position, 1.0);
	
	v_Color = a_Color;
	v_EntityID = a_EntityID;
}

#type fragment
#version 420 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec4 v_Color;
flat in int v_EntityID;

void main()
{
	o_Color = v_Color;
	o_EntityID = -1;// v_EntityID; 
}