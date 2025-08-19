// Normal Color Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Norm;

uniform mat4 u_ViewProjection;
uniform mat4 u_Color;
uniform mat4 u_Transform;

out vec4 v_Color;
out vec2 v_TexCoord;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	
	v_Color = vec4((a_Norm.x + 1) / 2.0, (a_Norm.y + 1) / 2.0, (a_Norm.z + 1) / 2.0, 1.0);
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;

void main()
{
	color = v_Color;
}