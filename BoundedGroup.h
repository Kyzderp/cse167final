#ifndef _BOUNDEDGROUP_H_
#define _BOUNDEGROUP_H_

#include "group.h"
#include "Window.h"

class BoundedGroup : public Group {
public:
	BoundedGroup(glm::vec3 minPoint, glm::vec3 maxPoint);

	glm::vec3 minPoint, maxPoint;
	bool inCollision;

	virtual void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	virtual void update() { Group::update(); }
	bool checkBounds(glm::mat4 C);
};

#endif
