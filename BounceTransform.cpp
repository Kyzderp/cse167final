#include "BounceTransform.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

BounceTransform::BounceTransform(glm::mat4 translateMat, glm::mat4 scaleMat) 
	: MatrixTransform(translateMat, scaleMat)
{
	angle = 0.0f;
	doAngle = 1;
	angleIncrement = 0.001f;
}

BounceTransform::~BounceTransform()
{

}

void BounceTransform::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	for (int i = 0; i < children.size(); i++)
		children[i]->draw(shaderProgram, C * this->translate * rotate * this->scale, this->color);
}

void BounceTransform::update()
{
	if (doAngle)
	{
		if (angleUp)
		{
			if (angle > 0.5f)
			{
				angleUp = 0;
				angle -= angleIncrement;
			}
			else
				angle += angleIncrement;
		}
		else
		{
			if (angle < -0.5f)
			{
				angleUp = 1;
				angle += angleIncrement;
			}
			else
				angle -= angleIncrement;
		}
		int dir = 2 * angleUp - 1;
		
		translate = translate * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, angleIncrement * dir, 0.0f));
	}

	// Children
	for (int i = 0; i < children.size(); i++)
		children[i]->update();
}