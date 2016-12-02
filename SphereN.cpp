#include "SphereN.h"
#define _USE_MATH_DEFINES
#include <math.h>

SphereN::SphereN(glm::vec3 c, float r, float slice, float stack, float width)
{
	this->RADIUS = r;
	this->SLICES = slice;
	this->STACKS = stack;

	color = c;
	colorSpec = c / 1.4f;
	
	for (int s = 0; s <= SLICES; s++) {
		float anglee = (M_PI) * s / SLICES;

		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), anglee, glm::vec3(0, 1.0f, 0));

		for (int i = 0; i < SEGMENTS; i++) {
			float angle = 2 * M_PI * i / SEGMENTS,
				next_angle = 2 * M_PI * (i + 1) / SEGMENTS;
			float x1 = cos(angle) * RADIUS;
			float y1 = sin(angle) * RADIUS;
			float x2 = cos(next_angle) * RADIUS;
			float y2 = sin(next_angle) * RADIUS;
			glm::vec4 newPos1 = rot * glm::vec4(x1, y1, 0, 1);
			glm::vec4 newPos2 = rot * glm::vec4(x2, y2, 0, 1);

			points.push_back(glm::vec3(newPos1.x, newPos1.y, newPos1.z));
			points.push_back(glm::vec3(newPos2.x, newPos2.y, newPos2.z));
		}
	}

	for (int s = 0; s <= STACKS; s++) {

		float anglee = (M_PI / 2)* s / STACKS;

		float radius = cos(anglee) * RADIUS;
		float y = sin(anglee) * RADIUS;

		for (int i = 0; i < SEGMENTS; i++) {
			float angle = 2 * M_PI * i / SEGMENTS,
				next_angle = 2 * M_PI * (i + 1) / SEGMENTS;
			float x1 = cos(angle) * radius;
			float y1 = sin(angle) * radius;
			float x2 = cos(next_angle) * radius;
			float y2 = sin(next_angle) * radius;

			points.push_back(glm::vec3(x1, y, y1));
			points.push_back(glm::vec3(x2, y, y2));

			points.push_back(glm::vec3(x1, -y, y1));
			points.push_back(glm::vec3(x2, -y, y2));
		}
	}

	glLineWidth(width);

	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &nVBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * 4, points.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, nVBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * 4, points.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void SphereN::render()
{
	glBindVertexArray(VAO);

	glUniform3f(Geode::matAmbientLoc, color.x, color.y, color.z);
	glUniform3f(Geode::matDiffuseLoc, color.x, color.y, color.z);
	glUniform3f(Geode::matSpecularLoc, colorSpec.x, colorSpec.y, colorSpec.z);
	glUniform1f(Geode::matShineLoc, shiny);

	glDrawArrays(GL_LINES, 0, points.size());

	glBindVertexArray(0);
}

void SphereN::update()
{

}