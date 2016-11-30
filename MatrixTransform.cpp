#include "MatrixTransform.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

MatrixTransform::MatrixTransform(glm::mat4 translateMat, glm::mat4 scaleMat)
{
	this->translate = translateMat;
	this->scale = scaleMat;
	angle = 0.0f;
	angleUp = 1;
	doAngle = 0;
	angleIncrement = 0.5f;
	rotate = glm::mat4(1.0f);

	color = glm::vec3(1.0f);
}

MatrixTransform::~MatrixTransform()
{

}

void MatrixTransform::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	//cout << "draw mat transform" << endl;
	for (int i = 0; i < children.size(); i++)
		children[i]->draw(shaderProgram, C * this->translate * rotate * this->scale, this->color);
}

void MatrixTransform::update()
{
	// herpderp angle
	if (doAngle)
	{
		if (angleUp)
		{
			if (angle > 50)
			{
				angleUp = 0;
				angle -= angleIncrement;
			}
			else
				angle += angleIncrement;
		}
		else
		{
			if (angle < -50)
			{
				angleUp = 1;
				angle += angleIncrement;
			}
			else
				angle -= angleIncrement;
		}

		// Rotate it about the X axis
		glm::mat4 translateToOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -1.5, 0.0));
		glm::mat4 rotateAboutOrigin = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), glm::vec3(1.0, 0.0, 0.0));
		rotate = glm::inverse(translateToOrigin) * rotateAboutOrigin * translateToOrigin;
		//rotate = rotateAboutOrigin;
	}

	// Children
	for (int i = 0; i < children.size(); i++)
		children[i]->update();
}