#include "GeodeN.h"

GeodeN::GeodeN()
{
	uProjection = glGetUniformLocation(Window::shaderProgram, "projection");
	uModelview = glGetUniformLocation(Window::shaderProgram, "modelview");
	matAmbientLoc = glGetUniformLocation(Window::shaderProgram, "material.ambient");
	matDiffuseLoc = glGetUniformLocation(Window::shaderProgram, "material.diffuse");
	matSpecularLoc = glGetUniformLocation(Window::shaderProgram, "material.specular");
	matShineLoc = glGetUniformLocation(Window::shaderProgram, "material.shininess");
}

GeodeN::~GeodeN()
{
	// Delete previously generated buffers.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &nVBO);
}

void GeodeN::draw(glm::mat4 C)
{
	//toWorld = C * toWorld;

	glm::mat4 modelview = Window::V * C * toWorld;

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	render();
}