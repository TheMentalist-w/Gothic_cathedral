#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;



//Atrybuty
in vec4 vertex;
in vec4 color;
in vec4 normal;
in vec2 texCoord0;


//Zmienne interpolowane
out vec4 ic;
out vec4 l;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;

void main(void) {
	vec4 lp = vec4(0, 0, -6, 1);
	l = normalize(V * lp - V * M * vertex);
	n = normalize(vec4(0, 0, 0, 1) - V * M * vertex);
	v = normalize(V * M * normal);
	iTexCoord0 = texCoord0;
	ic = color;

    gl_Position=P*V*M*vertex;
}
