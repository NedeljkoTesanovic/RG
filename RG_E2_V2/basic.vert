#version 330 core //Specifikacija verzije GLSL jezika (OpenGL verzija * 100) i profila (core/compatibility)
//Specifikacija ulaznih kanala
//Layout (location = x) govori da na taj kanal dolazi atribut ciji pokazivac ima indeks x (iz glVertexAttribPointer funkcije)
layout (location = 0) in vec2 aPos; //Pozicija tjemena
layout (location = 1) in vec4 aCol; //Boja tjemena
out vec4 colorChannel; //Kanal koji dijelimo sa Fragment shader-om i na koji prosljedjujemo boju
void main()
{
	colorChannel = aCol; //Boju ne koristimo, samo saljemo dalje do Fragment sejdera
	gl_Position = vec4(aPos.xy, 0.0, 1.0); //gl_Position je predefinisana vec4 promjenljiva koja opisuje prostornu poziciju tjemena
}