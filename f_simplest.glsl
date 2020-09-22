#version 330


uniform sampler2D textureMap0;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny kolor piksela

//Zmienne interpolowane
in vec4 l_0;
in vec4 l_1;
in vec4 lo;
in vec4 n;
in vec4 v;
in vec2 iTexCoord0;

void main(void) {

	//ponowna normalizacja zmiennych interpolowanych
	vec4 ml_0 = normalize(l_0);
	vec4 ml_1 = normalize(l_1);
	vec4 mlo = normalize(lo);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);

	//wektory odbite w przestrzeni oka
	vec4 mr_0 = reflect(-ml_0, mn);
	vec4 mr_1 = reflect(-ml_1, mn);
	vec4 mro = reflect(-mlo, mn);

	vec4 kd = texture(textureMap0, iTexCoord0);
	vec4 ks = kd;

	//iloczyny skalarne
	float nl_0 = clamp(dot(mn, ml_0), 0, 1);
	float nl_1 = clamp(dot(mn, ml_1), 0, 1);
	float nlo = clamp(dot(mn, mlo), 0, 1);
	float rv_0 = pow(clamp(dot(mr_0, mv), 0, 1), 5);
	float rv_1 = pow(clamp(dot(mr_1, mv), 0, 1), 5);
	float rvo = pow(clamp(dot(mro, mv), 0, 1), 25); //25 - skupienie œwiat³a

	pixelColor = vec4(kd.rgb * nl_0, kd.a) + vec4(ks.rgb *rv_0, 0) + vec4(kd.rgb * nl_1, kd.a) + vec4(ks.rgb *rv_1, 0) + vec4(kd.rgb * nlo*vec3(0.83, 0.69, 0.1686), kd.a) + vec4(ks.rgb *rvo * vec3(0.83, 0.69, 0.1686), 0);
}