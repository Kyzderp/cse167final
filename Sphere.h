#ifndef _SPHERE_H_
#define _SPHERE_H_

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
#include "Skybox.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Sphere : public Geode
{
public:
	Sphere(int wireframe);
	~Sphere();

	glm::mat4 toWorld;

	int wireframe;

	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 cam_pos);
	void update();
	void makeSphere();
	void makeCircle(int slices, float radius, float z);

	unsigned char* loadPPM(const char* filename, int& width, int& height);
	GLuint loadCubemap(std::vector<const GLchar*> faces);

	// These variables are needed for the shader program
	GLuint VBO, VAO, NBO, EBO;
	GLuint uProjection, uModel, uView;

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
};

#endif

