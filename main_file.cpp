/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "cylinder2.h"
#include "cylinder0.h"
#include "stainedGlass8.h"
#include "stainedGlass7.h"
#include "stainedGlass6.h"
#include "stainedGlass5.h"
#include "stainedGlass4.h"
#include "stainedGlass3.h"
#include "stainedGlass2.h"
#include "stainedGlass1.h"
#include "plane18.h"
#include "plane014.h"
#include "plane014v1.h"
#include "plane3.h"
#include "doors.h"
#include "cube10.h"
#include "cube3.h"
#include "circle562.h"
#include "circle50.h"
#include "circle238.h"
#include "circle48.h"
#include "circle46.h"
#include "circle45.h"
#include "circle44.h"
#include "circle43.h"
#include "circle3.h"
#include "circle500.h"
#include "circleMesh.h"
#include "candle.h"
#include "candlev1.h"

float speed = 0;
float w_speed = 0;
float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;
GLuint tex[28];
ShaderProgram* sp;
glm::vec3 c_position = glm::vec3(0.0f, -15.0f, 0.0f);

const float bounds[] = { -3, 3, -14, -9,		// min x, max x, min z, max z

	// altair columns
	-9.5, -4, -21, -15.5,
	4, 9.5, -21, -15.5,

	// cols
	11, 14.5, -11, -6,
	-14.5, -11, -11, -6,
	11, 14.5, 6, 11,
	-14.5, -11, 6, 11,
	11, 14.5, 19.5, 27,
	-14.5, -11, 19.5, 27,
	11, 14.5, 35.5, 43,
	-14.5, -11, 35.5, 43,

	// benches
	1, 11, 2, 25,
	-11, -1, 2, 25,

	-14, -11, 50, 55,
	11, 14, 50, 55
};

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
		if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
		if (key == GLFW_KEY_UP) speed_y = PI / 2;
		if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
		if (key == GLFW_KEY_W) w_speed = 1.0;
		if (key == GLFW_KEY_S) w_speed = -1.0;
		if (key == GLFW_KEY_A) speed = 2.0;
		if (key == GLFW_KEY_D) speed = -2.0;

		if (key == GLFW_KEY_E) printf("x: %.3f   y: %.3f   z: %.3f\n", c_position.x, c_position.y, c_position.z);
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x = 0;
		if (key == GLFW_KEY_RIGHT) speed_x = 0;
		if (key == GLFW_KEY_UP) speed_y = 0;
		if (key == GLFW_KEY_DOWN) speed_y = 0;
		if (key == GLFW_KEY_A || key == GLFW_KEY_D) speed = 0.0;
		if (key == GLFW_KEY_W || key == GLFW_KEY_S) w_speed = 0.0;
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(.2f, .2f, .2f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	tex[0] = readTexture("stainedGlass2.png");
	tex[1] = readTexture("stainedGlass6.png");
	tex[2] = readTexture("stainedGlass5.png");
	tex[3] = readTexture("stainedGlass7.png");
	tex[4] = readTexture("stainedGlass8.png");
	tex[5] = readTexture("stainedGlass3.png");
	tex[6] = readTexture("stainedGlass2.png");
	tex[7] = readTexture("stainedGlass4.png");
	tex[8] = readTexture("stainedGlass1.png");
	tex[9] = readTexture("TextureAtlasEntree.png");
	tex[10] = readTexture("TextureAtlasAutel.001.png");
	tex[11] = readTexture("TextureAtlasObjetOr.png");
	tex[12] = readTexture("TextureAtlasPiedCol.png");
	tex[13] = readTexture("TextureAtlasAutelSol.png");
	tex[14] = readTexture("TextureAtlasColExt.png");
	tex[15] = readTexture("TextureAtlasColInt.png");
	tex[16] = readTexture("TextureAtlasSol.png");
	tex[17] = readTexture("Door.png");
	tex[18] = readTexture("TextureAtlasMurBasExt.png");
	tex[19] = readTexture("TextureAtlasMurBasInt.png");
	tex[20] = readTexture("TextureAtlasArche.png");
	tex[21] = readTexture("TextureAtlasMurHaut.png");
	tex[22] = readTexture("TextureAtlasDebord.png");
	tex[23] = readTexture("TextureAtlasPlafond.png");
	tex[24] = readTexture("TextureAtlasBanc.png");
	tex[25] = readTexture("TextureAtlasArcheExt.png");
	tex[26] = readTexture("graal.png");
	tex[27] = readTexture("cANDLECOLOR.png");
}

//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	glDeleteTextures(28, tex);
	delete sp;
}

float dist(glm::vec3& cam, glm::mat3& p) {
	// camera position, polygon

	// calc normal
	glm::vec3 norm = glm::normalize(glm::cross(p[1] - p[0], p[2] - p[0]));

	// D = -P dot N
	float d = glm::dot(glm::vec3(-(p[0].x + p[1].x + p[2].x) / 3, -(p[0].y + p[1].y + p[2].y) / 3, -(p[0].z + p[1].z + p[2].z) / 3),
		norm);

	if (glm::abs(norm.z) > .1f) printf(".");

	// calc distance
	return glm::abs(glm::dot(cam, norm) + d);
}

void colision_detection(glm::vec3& position, float& angle) {
	position = glm::vec3(glm::clamp(position.x, -21.0f, 21.0f), position.y,
		glm::clamp(position.z, -26.0f, 53.0f));
	if (position.z < 17.0f / 9 * position.x - 146.0f / 3) {
		position.x = (9.0f * position.z + 438.0f) / 17;
	}
	if (position.z < -17.0f / 8 * position.x - 429.0f / 8) {
		position.z = -17.0f / 8 * position.x - 429.0f / 8;
	}

	if (position.x > -9 && position.x < 9 && position.z > -20 && position.z < -3.5) {
		if (position.x < -8.5)
			position.y = -14.0f + 2.0f * (8.5f + position.x);
		else if (position.x > 8.5)
			position.y = -14.0f + 2.0f * (8.5f - position.x);

		else if (position.z > -4)
			position.y = -14.0f - 2.0f * (4.0f + position.z);
		else if (position.z < -19.5)
			position.y = -14.0f + 2.0f * (19.5f + position.z);

		else position.y = -14;
	}
	else position.y = -15;


	for (int i = 0; i < 15 * 4; i += 4) {
		if (position.x > bounds[i] && position.x < bounds[i + 1] && position.z > bounds[i + 2] && position.z < bounds[i + 3]) {
			printf("%d/%.1f: %.2f %.2f\t\t%.2f\t", i / 4, bounds[i], position.x, position.z, angle);
			if (abs(cos(angle)) > 0.01f) {
				if (abs(sin(angle)) < 0.01f) {	// perpendicular to an edge
					printf(" s0 ");
					if (cos(angle) > 0)
						position.z = bounds[i + 2];
					else
						position.z = bounds[i + 3];
					printf("%.2f %.2f\n", position.x, position.z);
					continue;
				}
				// possible to calc. tangent
				float coef_dir = tan(angle);
				if (sin(angle) > 0 && cos(angle) > 0) {
					printf(" ++ ");
					if ((position.x - coef_dir * position.z) < (bounds[i] - coef_dir * bounds[i + 2]))
						position.x = bounds[i];
					else
						position.z = bounds[i + 2];
				}
				else if (sin(angle) > 0 && cos(angle) < 0) {
					printf(" +- ");
					if ((position.x - coef_dir * position.z) < (bounds[i] - coef_dir * bounds[i + 3]))
						position.x = bounds[i];
					else
						position.z = bounds[i + 3];
				}
				else if (sin(angle) < 0 && cos(angle) > 0) {
					printf(" -+ ");
					if ((position.x - coef_dir * position.z) > (bounds[i + 1] - coef_dir * bounds[i + 2]))
						position.x = bounds[i + 1];
					else
						position.z = bounds[i + 2];
				}
				else {		// if (sin(angle) < 0 && cos(angle) < 0)
					printf(" -- ");
					if ((position.x - coef_dir * position.z) > (bounds[i + 1] - coef_dir * bounds[i + 3]))
						position.x = bounds[i + 1];
					else
						position.z = bounds[i + 3];
				}
			}
			else {
				printf(" c0 ");
				if (sin(angle) > 0)
					position.x = bounds[i];
				else
					position.x = bounds[i + 1];
			}
			printf("%.2f %.2f\n", position.x, position.z);
		}
	}

	//for (int i = 0; i < 13 * 4; i += 4) {
	//	if (position.x > bounds[i] && position.x < bounds[i + 1] && position.z > bounds[i + 2] && position.z < bounds[i + 3]) {
	//		printf("%d/%.1f: %.2f %.2f\t\t%.2f\t\t", i / 4, bounds[i], position.x, position.z, angle);

	//		if (angle < 0.785f || angle > 5.5f) position.z = bounds[i + 2];
	//		else if (angle < 2.356f) position.x = bounds[i];
	//		else if (angle < 3.927f) position.z = bounds[i + 3];
	//		else position.x = bounds[i + 1];

	//		printf("%.2f %.2f\n", position.x, position.z);
	//	}
	//}
}


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float position, float angle, float rotation, float *upDown) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	while (angle < 0) angle += 2.0f * PI;
	while (angle > 2 * PI) angle -= 2.0f * PI;

	glm::vec3 direction = glm::vec3(sin(angle) * 5.0, 0.0f, cos(angle) * 5.0);
	c_position = c_position + direction * position;

	colision_detection(c_position, angle);

	glm::mat4 V = glm::lookAt(
		c_position,
		direction + c_position,
		glm::vec3(0.0f, 1.0f, 0.0f)); //kierunek w górę na oś y

	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 100.0f); //Wylicz macierz rzutowania


	sp->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));



	//Drawing elements
	//Columns

	glm::mat4 M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.0f, 20.0f, 0.0f));
	M = glm::rotate(M, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cylinder2Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cylinder2Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cylinder2TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[15]);

	glDrawArrays(GL_TRIANGLES, 0, cylinder2VertexCount); //Draw


	//Columns pedestals

	glm::mat4 MC0 = M;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MC0));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cylinder0Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cylinder0Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cylinder0TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[12]);

	glDrawArrays(GL_TRIANGLES, 0, cylinder0VertexCount); //Draw


	//plane014

	glm::mat4 M014 = glm::mat4(1.0f);
	M014 = glm::translate(M014, glm::vec3(0.0f, -17.0f, -12.5f));
	M014 = glm::rotate(M014, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M014 = glm::rotate(M014, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M014));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, plane014Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, plane014Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, plane014TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[13]);

	glDrawArrays(GL_TRIANGLES, 0, plane014VertexCount); //Draw


	//plane014v1

	glm::mat4 M014V1 = M014;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M014V1));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, plane014v1Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, plane014v1Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, plane014v1TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[13]);

	glDrawArrays(GL_TRIANGLES, 0, plane014v1VertexCount); //Draw

	//plane18

	glm::mat4 MP18 = M014;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MP18));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, plane018Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, plane018Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, plane018TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[10]);

	glDrawArrays(GL_TRIANGLES, 0, plane018VertexCount); //Draw



	//circleMesh
	//graal

	glm::mat4 MCM = glm::mat4(1.0f);
	MCM = glm::translate(MCM, glm::vec3(0.0f, -15.0f, -12.5f));
	MCM = glm::rotate(MCM, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	MCM = glm::rotate(MCM, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	MCM = glm::rotate(MCM, 90.0f * PI / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	MCM = glm::rotate(MCM, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MCM));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circleMeshVertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circleMeshNormals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circleMeshTexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[26]);

	glDrawArrays(GL_TRIANGLES, 0, circleMeshVertexCount); //Draw




	//circle3

	glm::mat4 MC3 = glm::mat4(1.0f);
	MC3 = glm::translate(MC3, glm::vec3(0.0f, -17.3f, -12.5f));
	MC3 = glm::scale(MC3, glm::vec3(0.135f, 0.135f, 0.135f));
	MC3 = glm::rotate(MC3, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	MC3 = glm::rotate(MC3, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MC3));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle3Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle3Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle3TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[11]);

	glDrawArrays(GL_TRIANGLES, 0, circle3VertexCount); //Draw


	//circle48
	// floor

	glm::mat4 M48 = glm::mat4(1.0f);
	M48 = glm::translate(M48, glm::vec3(0.0f, 15.0f, -7.8f));
	M48 = glm::rotate(M48, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M48 = glm::rotate(M48, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M48));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle48Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle48Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle48TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[16]);

	glDrawArrays(GL_TRIANGLES, 0, circle48VertexCount); //Draw


	//plane3

	glm::mat4 MP3 = glm::mat4(1.0f);
	MP3 = glm::translate(MP3, glm::vec3(0.0f, 14.5f, 0.0f));
	MP3 = glm::scale(MP3, glm::vec3(0.55f, 0.55f, 0.55f));
	MP3 = glm::rotate(MP3, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	MP3 = glm::rotate(MP3, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MP3));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, plane3Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, plane3Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, plane3TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[24]);

	glDrawArrays(GL_TRIANGLES, 0, plane3VertexCount); //Draw


	//circle43
	// arcs between columns - "fronts"

	glm::mat4 M43 = glm::mat4(1.0f);
	M43 = glm::translate(M43, glm::vec3(0.0f, 15.6f, -8.0f));
	M43 = glm::rotate(M43, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M43 = glm::rotate(M43, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M43));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle43Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle43Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle43TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[19]);

	glDrawArrays(GL_TRIANGLES, 0, circle43VertexCount); //Draw

	//circle50
	// arcs between columns - lower part

	glm::mat4 MC50 = M43;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MC50));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle50Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle50Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle50TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[20]);

	glDrawArrays(GL_TRIANGLES, 0, circle50VertexCount); //Draw


	//circle45

	glm::mat4 M45 = glm::mat4(1.0f);
	M45 = glm::translate(M45, glm::vec3(0.1f, 15.0f, -8.0f));
	M45 = glm::rotate(M45, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M45 = glm::rotate(M45, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M45));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle45Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle45Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle45TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[14]);

	glDrawArrays(GL_TRIANGLES, 0, circle45VertexCount); //Draw



	//circle46

	glm::mat4 M46 = glm::mat4(1.0f);
	M46 = glm::translate(M46, glm::vec3(18.5f, -9.5f, -18.5f));
	M46 = glm::rotate(M46, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M46 = glm::rotate(M46, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M46));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle46Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle46Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle46TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[18]);

	glDrawArrays(GL_TRIANGLES, 0, circle46VertexCount); //Draw


	//stainedGlass8

	glm::mat4 MS8 = glm::mat4(1.0f);
	MS8 = glm::translate(MS8, glm::vec3(0.0f, 24.0f, -0.2f));
	MS8 = glm::rotate(MS8, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	MS8 = glm::rotate(MS8, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MS8));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stainedGlass8Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stainedGlass8Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, stainedGlass8TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[4]);

	glDrawArrays(GL_TRIANGLES, 0, stainedGlass8VertexCount); //Draw


	//stainedGlass5

	glm::mat4 MS5 = glm::mat4(1.0f);
	MS5 = glm::translate(MS5, glm::vec3(-19.0f, -10.0f, -19.0f));
	MS5 = glm::scale(MS5, glm::vec3(12.0f, 12.0f, 12.0f));
	MS5 = glm::rotate(MS5, PI, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	MS5 = glm::rotate(MS5, -60.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MS5));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stainedGlass5Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stainedGlass5Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, stainedGlass5TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[2]);

	glDrawArrays(GL_TRIANGLES, 0, stainedGlass5VertexCount); //Draw


	//stainedGlass3

	glm::mat4 MS3 = glm::mat4(1.0f);
	MS3 = glm::translate(MS3, glm::vec3(0.0f, 11.0f, -20.0f));
	MS3 = glm::scale(MS3, glm::vec3(18.0f, 18.0f, 18.0f));
	MS3 = glm::rotate(MS3, PI, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	MS3 = glm::rotate(MS3, PI, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MS3));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stainedGlass3Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stainedGlass3Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, stainedGlass3TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[5]);

	glDrawArrays(GL_TRIANGLES, 0, stainedGlass3VertexCount); //Draw


	//stainedGlass1

	glm::mat4 MS1 = glm::mat4(1.0f);
	MS1 = glm::translate(MS1, glm::vec3(-13.5f, 10.0f, 0.0f));
	MS1 = glm::scale(MS1, glm::vec3(16.5f, 16.5f, 16.5f));
	MS1 = glm::rotate(MS1, PI, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	MS1 = glm::rotate(MS1, -90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MS1));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stainedGlass1Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stainedGlass1Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, stainedGlass1TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[8]);

	glDrawArrays(GL_TRIANGLES, 0, stainedGlass1VertexCount); //Draw



	//stainedGlass2

	glm::mat4 MS2 = glm::mat4(1.0f);
	MS2 = glm::translate(MS2, glm::vec3(13.5f, 10.0f, 47.0f));
	MS2 = glm::scale(MS2, glm::vec3(17.5f, 17.5f, 17.5f));
	MS2 = glm::rotate(MS2, PI, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	MS2 = glm::rotate(MS2, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MS2));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stainedGlass2Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stainedGlass2Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, stainedGlass2TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[6]);

	glDrawArrays(GL_TRIANGLES, 0, stainedGlass2VertexCount); //Draw


	//stainedGlass4

	glm::mat4 MS4 = glm::mat4(1.0f);
	MS4 = glm::translate(MS4, glm::vec3(10.0f, 10.0f, -13.5f));
	MS4 = glm::scale(MS4, glm::vec3(17.0f, 17.0f, 17.0f));
	MS4 = glm::rotate(MS4, PI, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	MS4 = glm::rotate(MS4, 60.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MS4));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stainedGlass4Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stainedGlass4Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, stainedGlass4TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[7]);

	glDrawArrays(GL_TRIANGLES, 0, stainedGlass4VertexCount); //Draw

	//stainedGlass4v1

	glm::mat4 MS4v1 = glm::mat4(1.0f);
	MS4v1 = glm::translate(MS4v1, glm::vec3(-10.0f, 10.0f, -13.5f));
	MS4v1 = glm::scale(MS4v1, glm::vec3(17.0f, 17.0f, 17.0f));
	MS4v1 = glm::rotate(MS4v1, PI, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	MS4v1 = glm::rotate(MS4v1, -60.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MS4v1));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stainedGlass4Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stainedGlass4Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, stainedGlass4TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[7]);

	glDrawArrays(GL_TRIANGLES, 0, stainedGlass4VertexCount); //Draw


	//stainedGlass7

	glm::mat4 MS7 = glm::mat4(1.0f);
	MS7 = glm::translate(MS7, glm::vec3(19.0f, -10.0f, -18.9f));
	MS7 = glm::scale(MS7, glm::vec3(12.0f, 12.0f, 12.0f));
	MS7 = glm::rotate(MS7, PI, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	MS7 = glm::rotate(MS7, 60.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MS7));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stainedGlass7Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stainedGlass7Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, stainedGlass7TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[3]);

	glDrawArrays(GL_TRIANGLES, 0, stainedGlass7VertexCount); //Draw

	//stainedGlass6

	glm::mat4 MS6 = glm::mat4(1.0f);
	MS6 = glm::translate(MS6, glm::vec3(0.3f, -10.0f, -30.0f));
	MS6 = glm::scale(MS6, glm::vec3(12.0f, 12.0f, 12.0f));
	MS6 = glm::rotate(MS6, PI, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MS6));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stainedGlass6Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stainedGlass6Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, stainedGlass6TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[1]);

	glDrawArrays(GL_TRIANGLES, 0, stainedGlass6VertexCount); //Draw


	//cube3
	// entree

	glm::mat4 M3 = glm::mat4(1.0f);
	M3 = glm::translate(M3, glm::vec3(0.0f, 15.5f, 5.0f));
	M3 = glm::rotate(M3, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M3 = glm::rotate(M3, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M3));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cube3Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cube3Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cube3TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[9]);

	glDrawArrays(GL_TRIANGLES, 0, cube3VertexCount); //Draw


	//doors

	glm::mat4 MD = glm::mat4(1.0f);
	MD = glm::translate(MD, glm::vec3(0.0f, -11.7f, 55.0f));
	MD = glm::scale(MD, glm::vec3(14.0f, 14.0f, 14.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MD));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, doorsVertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, doorsNormals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, doorsTexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[17]);

	glDrawArrays(GL_TRIANGLES, 0, doorsVertexCount); //Draw


	//circle562

	glm::mat4 M562 = glm::mat4(1.0f);
	M562 = glm::translate(M562, glm::vec3(0.0f, 16.5f, -19.3f));
	M562 = glm::rotate(M562, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M562 = glm::rotate(M562, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M562));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle562Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle562Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle562TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[21]);

	glDrawArrays(GL_TRIANGLES, 0, circle562VertexCount); //Draw



	//Cube10

	glm::mat4 M10 = glm::mat4(1.0f);
	M10 = glm::translate(M10, glm::vec3(0.0f, 15.8f, 5.0f));
	M10 = glm::rotate(M10, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M10 = glm::rotate(M10, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M10));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cube10Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cube10Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cube10TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[22]);

	glDrawArrays(GL_TRIANGLES, 0, cube10VertexCount); //Draw


	//Circle238

	glm::mat4 M238 = glm::mat4(1.0f);
	M238 = glm::translate(M238, glm::vec3(0.1f, -9.8f, -29.0f));
	M238 = glm::rotate(M238, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M238 = glm::rotate(M238, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M238));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle238Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle238Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle238TexCoords0);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[25]);

	glDrawArrays(GL_TRIANGLES, 0, circle238VertexCount); //Draw


	//circle500

	glm::mat4 M500 = glm::mat4(1.0f);
	M500 = glm::translate(M500, glm::vec3(-9.2f, 14.5f, -13.3f));
	M500 = glm::rotate(M500, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M500 = glm::rotate(M500, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M500));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle500Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle500Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle500TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[25]);

	glDrawArrays(GL_TRIANGLES, 0, circle500VertexCount);


	//circle44

	glm::mat4 M44 = glm::mat4(1.0f);
	M44 = glm::translate(M44, glm::vec3(0.0f, 14.3f, -8.0f));
	M44 = glm::rotate(M44, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	M44 = glm::rotate(M44, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M44));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, circle44Vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, circle44Normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, circle44TexCoords1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[23]);

	glDrawArrays(GL_TRIANGLES, 0, circle44VertexCount);

	//
	//TODO: add light source according to the position of candles
	//
	glm::vec3 candle_position = glm::vec3(0, 0, 0);
	glm::vec3 flame_position = glm::vec3(0, 0, 0);

	for (int i = 0; i < 4; i++) {
		//candle
		if (i == 0) {
			candle_position = glm::vec3(-2.0f, -15.0f + upDown[i], -11.0f);
			flame_position = glm::vec3(-2.0f, -15.2f + upDown[i], -11.0f);
		}
		else if (i == 1) {
			candle_position = glm::vec3(2.0f, -15.0f + upDown[i], -11.0f);
			flame_position = glm::vec3(2.0f, -15.2f + upDown[i], -11.0f);
		}
		else if (i == 2) {
			candle_position = glm::vec3(-2.0f, -15.0f + upDown[i], -13.5f);
			flame_position = glm::vec3(-2.0f, -15.2f + upDown[i], -13.5f);
		}
		else {
			candle_position = glm::vec3(2.0f, -15.0f + upDown[i], -13.5f);
			flame_position = glm::vec3(2.0f, -15.2f + upDown[i], -13.5f);
		}
		glm::mat4 MC = glm::mat4(1.0f);
		MC = glm::translate(MC, candle_position);
		MC = glm::scale(MC, glm::vec3(0.1f, 0.1f, 0.1f));
		MC = glm::rotate(MC, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
		MC = glm::rotate(MC, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MC));

		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, candleVertices);

		glEnableVertexAttribArray(sp->a("normal"));
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, candleNormals);

		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, candleTexCoords0);

		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[27]);

		glDrawArrays(GL_TRIANGLES, 0, candleVertexCount); //Draw

		//flame
		
		glm::mat4 MCF = glm::mat4(1.0f);
		MCF = glm::translate(MCF, flame_position);
		MCF = glm::scale(MCF, glm::vec3(0.1f, 0.1f, 0.1f));
		MCF = glm::rotate(MCF, 90.0f * PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
		MCF = glm::rotate(MCF, 90.0f * PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MCF));

		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, candlev1Vertices);

		glEnableVertexAttribArray(sp->a("normal"));
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, candlev1Normals);

		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, candlev1TexCoords0);

		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[27]);

		glDrawArrays(GL_TRIANGLES, 0, candlev1VertexCount); //Draw
	}
	
	
	glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
	glDisableVertexAttribArray(sp->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu normal

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float position = 0;
	float angle = 0;
	float rotation = 0;
	float upDown[4] = { 0, 0.5, 1, 1.5 };
	float up_speed[4] = { 1.0, 1.0, 1.0, 1.0 };
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		position = w_speed * glfwGetTime();
		angle += speed * glfwGetTime();
		rotation += PI * glfwGetTime();
		for (int i = 0; i < 4; i++) {
			if (upDown[i] >= 2) up_speed[i] = -1.0;
			else if (upDown[i] <= 0) up_speed[i] = 1.0;
			upDown[i] += up_speed[i] * glfwGetTime();
		}
		glfwSetTime(0); //Zeruj timer
		drawScene(window, position, angle, rotation, upDown); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
