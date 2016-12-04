#ifndef _QUADPRISM_H_
#define _QUADPRISM_H_

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
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class QuadPrism
{
public:
	QuadPrism();
	~QuadPrism();

	int numBuildings;

	// n = negative, p = positive, assume it's centered at origin
	// This represents the 4 corners of the QuadPrism
	// Gonna go with right being positive x and up being positive z
	glm::vec3 nn;
	glm::vec3 np;
	glm::vec3 pn;
	glm::vec3 pp;

	glm::mat4 toWorld;

	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	void assignVertex(glm::vec3 point, glm::vec3 center);
	void addPoints(glm::vec3 one, glm::vec3 two, glm::vec3 three, glm::vec3 four);
	void makeQuadPrism();
	void makeBuffers();

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview;


	// Define the coordinates and indices needed to draw the cube. Note that it is not necessary
	// to use a 2-dimensional array, since the layout in memory is the same as a 1-dimensional array.
	// This just looks nicer since it's easy to tell what coordinates/indices belong where.
	std::vector<glm::vec3> bufferVertices;
	std::vector<unsigned int> bufferIndices;
	std::vector<float> colors;
};

#endif

