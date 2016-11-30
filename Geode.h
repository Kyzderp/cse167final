#ifndef _GEODE_H_
#define _GEODE_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include "Node.h"

class Geode : public Node
{
public:
	Geode();
	~Geode();

	virtual void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	virtual void update();
};

#endif

