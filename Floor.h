#ifndef _FLOOR_H_
#define _FLOOR_H_

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

class Floor : public Geode
{
public:
	Floor();
	~Floor();

	glm::mat4 toWorld;

	float size;

	void makeFloor();
	void makeRoads();
	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	void drawRoads(glm::mat4 C, glm::vec3 color);
	void update();
	static GLuint loadTexture(const GLchar* path);

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	std::vector<glm::vec3> roadVertices;
	std::vector<unsigned int> roadIndices;

	// These variables are needed for the shader program
	GLuint VBO, VAO, NBO, TBO, EBO;
	GLuint uProjection, uModelview;

	GLuint roadVAO, roadVBO, roadEBO;
};

#endif

