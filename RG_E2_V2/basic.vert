#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec4 inColour;

out vec4 colour;

uniform vec3 offset;

void main()
{
	gl_Position = vec4(inPos + offset, 1.0);
	colour = inColour;
}