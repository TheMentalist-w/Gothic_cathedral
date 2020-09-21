#version 330


uniform sampler2D textureMap0;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

//Zmienne interpolowane
in vec4 n;
in vec4 l;
in vec4 lo;
in vec4 v;
in vec2 iTexCoord0;

void main(void) {
	vec4 ml = normalize(l);
	vec4 mlo = normalize(lo);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);

	//wektory odbite
	vec4 mr = reflect(-ml, mn);
	vec4 mro = reflect(-mlo, mn);

	vec4 kd = texture(textureMap0, iTexCoord0);
	vec4 ks = kd;


	float nl = clamp(dot(mn, ml), 0, 1);
	float nlo = clamp(dot(mn, mlo), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 25);
	float rvo = pow(clamp(dot(mro, mv), 0, 1), 25);

	pixelColor = vec4(kd.rgb * nl*vec3(1, 0.9, 0.9), kd.a) + vec4(ks.rgb *rv*vec3(1, 0.9, 0.9), 0) + vec4(kd.rgb * nlo*vec3(0.83, 0.69, 0.1686), kd.a) + vec4(ks.rgb *rvo * vec3(0.83, 0.69, 0.1686), 0);
}