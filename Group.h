#ifndef _GROUP_H_
#define _GROUP_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include "Node.h"
#include <vector>

class Group : public Node
{
public:
	Group();
	~Group();

	std::vector<Node*> children;

	void addChild(Node* child);

	virtual void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	virtual void update();
};

#endif

