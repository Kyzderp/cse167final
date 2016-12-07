#include "BoundedGroup.h"

BoundedGroup::BoundedGroup(glm::vec3 min, glm::vec3 max)
{
	minPoint = min;
	maxPoint = max;
}

void BoundedGroup::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{

	if (inCollision) {
		color = glm::vec3(1.0f, 0, 0);
	}

	for (int i = 0; i < children.size(); i++) {
		children[i]->draw(shaderProgram, C, color);
	}
}

bool BoundedGroup::checkBounds(glm::vec4 p) 
{

	return true;
}
