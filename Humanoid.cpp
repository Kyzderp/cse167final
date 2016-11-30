#include "Humanoid.h"
#include "Window.h"
#include <iostream>

using namespace std;

vector<glm::vec3> planes;

Humanoid::Humanoid(glm::mat4 transformMat) : MatrixTransform(transformMat, glm::mat4(1.0f))
{
	cout << "Creating a humanoid!" << endl;
	
	// normals for planes
	planes.push_back(glm::vec3(1.0, 0.0, 0.0));
	planes.push_back(glm::vec3(-1.0, 0.0, 0.0));
	planes.push_back(glm::vec3(0.0, 1.0, 0.0));
	planes.push_back(glm::vec3(0.0, -1.0, 0.0));
	planes.push_back(glm::vec3(0.0, 0.0, 1.0));
	planes.push_back(glm::vec3(0.0, 0.0, -1.0));

	// Create bounding sphere
	glm::mat4 boundsScale = glm::scale(glm::mat4(1.0f), glm::vec3(2.3f));
	glm::mat4 boundsTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
	MatrixTransform* bounds = new MatrixTransform(boundsTranslate, boundsScale);
	bounds->addChild(Window::wireSphere);
	bounds->color = glm::vec3(1.0, 0.0, 0.0);
	this->addChild(bounds);

	// Create a body
	glm::mat4 bodyScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 1.0, 0.25));
	MatrixTransform* body = new MatrixTransform(glm::mat4(1.0f), bodyScale);
	body->addChild(Window::cube);
	body->color = glm::vec3(0.5, 0.8, 0.5);
	this->addChild(body);

	// Create left leg
	glm::mat4 leftLegScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.5, 0.2));
	glm::mat4 leftLegTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6, -3.0, 0.0));
	MatrixTransform* leftLeg = new MatrixTransform(leftLegTranslate, leftLegScale);
	leftLeg->addChild(Window::cube);
	leftLeg->doAngle = 1;
	leftLeg->color = glm::vec3(0.2, 0.3, 0.5);
	this->addChild(leftLeg);

	// Create right leg
	glm::mat4 rightLegScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.5, 0.2));
	glm::mat4 rightLegTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.6, -3.0, 0.0));
	MatrixTransform* rightLeg = new MatrixTransform(rightLegTranslate, rightLegScale);
	rightLeg->addChild(Window::cube);
	rightLeg->doAngle = 1;
	rightLeg->angleUp = 0;
	rightLeg->color = glm::vec3(0.2, 0.3, 0.5);
	this->addChild(rightLeg);

	// Create left arm
	glm::mat4 leftArmScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.75, 0.2));
	glm::mat4 leftArmTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(-1.4, 0.5, 0.0));
	MatrixTransform* leftArm = new MatrixTransform(leftArmTranslate, leftArmScale);
	leftArm->addChild(Window::cube);
	leftArm->doAngle = 1;
	leftArm->angleUp = 0;
	leftArm->color = glm::vec3(0.7, 0.8, 0.8);
	this->addChild(leftArm);

	// Create right arm
	glm::mat4 rightArmScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.75, 0.2));
	glm::mat4 rightArmTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(1.4, 0.5, 0.0));
	MatrixTransform* rightArm = new MatrixTransform(rightArmTranslate, rightArmScale);
	rightArm->addChild(Window::cube);
	rightArm->doAngle = 1;
	rightArm->color = glm::vec3(0.7, 0.8, 0.8);
	this->addChild(rightArm);

	// Create head D:
	glm::mat4 headScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	glm::mat4 headTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 3.0, 0.0));
	MatrixTransform* head = new MatrixTransform(headTranslate, headScale);
	head->addChild(Window::sphere);
	head->color = glm::vec3(0.7, 0.5, 0.2);
	this->addChild(head);
}

Humanoid::~Humanoid()
{
	for (int i = 0; i < children.size(); i++)
		delete(children[i]);
}

void Humanoid::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	glm::mat4 matrix = C * this->translate * rotate * this->scale;

	// culling
	if (Window::cull)
	{
		glm::vec4 center = Window::P * Window::V * matrix * glm::vec4(0.0, 0.0, 0.0, 1.0);

		center = center / center.w;
		//cout << "center: " << center.x << " " << center.y << " " << center.z << " " << center.w << endl;
		float radius = 0.5f;
		for (int i = 0; i < planes.size(); i++)
		{
			float distance = glm::dot(glm::vec3(center), planes[i]) - 1.0f;
			//cout << distance << endl;
			if (distance > radius)
				return;
		}
	}
	
	for (int i = 0; i < children.size(); i++)
		children[i]->draw(shaderProgram, matrix, this->color);
}

void Humanoid::update()
{
	for (int i = 0; i < children.size(); i++)
		children[i]->update();
}

void Humanoid::addChild(Node* child)
{
	this->children.push_back(child);
}