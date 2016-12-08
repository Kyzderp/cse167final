#include "BoundedGroup.h"

BoundedGroup::BoundedGroup(glm::vec3 min, glm::vec3 max)
{
	minPoint = min;
	maxPoint = max;
}

void BoundedGroup::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	color = glm::vec3(0, 1.0f, 0);

	if (checkBounds(C)) {
		color = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	for (int i = 0; i < children.size(); i++) {
		children[i]->draw(shaderProgram, C, color);
	}
}

bool BoundedGroup::checkBounds(glm::mat4 C) 
{
    // From: http://www.miguelcasillas.com/?p=30

	// box1 = orange
	// box2 = banana

	glm::vec3 bananaMax = glm::vec3(glm::vec4(maxPoint, 1.0f) * C);
	glm::vec3 bananaMin = glm::vec3(glm::vec4(minPoint, 1.0f) * C);

	//Check if Box1's max is greater than Box2's min and Box1's min is less than Box2's max
  return(Window::orangeMax.x > bananaMin.x &&
		Window::orangeMin.x < bananaMax.x &&
		Window::orangeMax.y > bananaMin.y &&
		Window::orangeMin.y < bananaMax.y &&
		Window::orangeMax.z > bananaMin.z &&
		Window::orangeMin.z < bananaMax.z);

	//If not, it will return false
}
