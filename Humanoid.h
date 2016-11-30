#ifndef _Humanoid_H_
#define _Humanoid_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include "MatrixTransform.h"
#include <vector>

class Humanoid : public MatrixTransform
{
public:
	Humanoid(glm::mat4 transformMat);
	~Humanoid();

	void addChild(Node* child);

	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	void update();
};

#endif

