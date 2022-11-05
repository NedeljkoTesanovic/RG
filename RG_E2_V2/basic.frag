#version 330 core
in vec4 colorChannel; //Kanal iz Vertex shader-a
out vec4 fragColor; //Izlazna boja tjemena
void main()
{
	fragColor = colorChannel;
}