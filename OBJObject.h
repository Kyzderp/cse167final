// Nicholas Deagon A11583792
#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "Geode.h"


class OBJObject : public Geode
{
public:
	OBJObject(const char* filepath, const char *tex_filepath, glm::vec3 matAmb,
														glm::vec3 matDiff,
														glm::vec3 matSpec,
														float shiny);
	~OBJObject();

	std::vector<unsigned int> indices;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	glm::mat4 toWorld;
	glm::mat4 default;

	glm::vec3 matAmb;
	glm::vec3 matDiff;
	glm::vec3 matSpec;
	float shiny;

	GLuint VBO, VAO, EBO, nVBO, tcVBO;
	GLuint uProjection, uModelview;

	GLuint textureMap;

	glm::mat4 parse(const char* filepath);
	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	void update();
	void spin(float deg);
	void move(glm::vec3 movementVec);
	void scale(float factor);
	void rotateZ(float deg);
	void reset();
	void rotate(glm::vec3 axis, float deg);
	GLuint loadTexture(const GLchar * path);
};

#endif