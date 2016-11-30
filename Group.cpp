#include "Group.h"

Group::Group()
{

}

Group::~Group()
{

}

void Group::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	for (int i = 0; i < children.size(); i++)
		children[i]->draw(shaderProgram, C, color);
}

void Group::update()
{
	for (int i = 0; i < children.size(); i++)
		children[i]->update();
}

void Group::addChild(Node* child)
{
	this->children.push_back(child);
}