#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;



//Atrybuty
in vec4 vertex;
in vec4 normal;
in vec2 texCoord0;


//Zmienne interpolowane
out vec4 l_0;
out vec4 l_1;
out vec4 lo;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;

void main(void) {
	//Ÿród³a œwiat³a
	vec4 lpo = vec4(1.5, 0, 0, 1); // pochodnia
	vec4 lp_0 = vec4(0, 13, -19, 1); // œwiat³o g³ówne
	vec4 lp_1 = vec4(0, 13, 0, 1); // œwiat³o g³ówne 1


	lo = normalize(lpo - V * M * vertex);
	l_0 = normalize(V * lp_0 - V * M * vertex);
	l_1 = normalize(V * lp_1 - V * M * vertex);

	n = normalize(V * M * normal);

	//wektor do obserwatora
	v = normalize(vec4(0, 0, 0, 1) - V * M * vertex);

	iTexCoord0 = texCoord0;

    gl_Position=P*V*M*vertex;
}
