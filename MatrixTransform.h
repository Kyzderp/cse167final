#ifndef _MATRIXTRANSFORM_H_
#define _MATRIXTRANSFORM_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include "Group.h"
#include <vector>

class MatrixTransform : public Group
{
public:
	MatrixTransform(glm::mat4 transformMat, glm::mat4 scaleMat);
	~MatrixTransform();

	glm::mat4 translate;
	glm::mat4 scale;
	glm::mat4 rotate;

	float angle;
	int angleUp;
	int doAngle;
	float angleIncrement;

	glm::vec3 color;

	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	void update();
};

#endif

