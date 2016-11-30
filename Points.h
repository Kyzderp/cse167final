#ifndef _POINTS_H_
#define _POINTS_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include "Geode.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Points : public Geode
{
public:
	Points(int anchor);
	~Points();

	int isAnchor;

	Points* next;
	Points* prev;

	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;

	glm::mat4 toWorld;

	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	void drawSelect(GLuint shaderProgram);
	void update();
	void makePoints();

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
};

#endif