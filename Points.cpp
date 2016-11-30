#include "Points.h"
#include "Window.h"
#include <iostream>
#include "Bezier.h"

using namespace std;


Points::Points(int anchor)
{
	this->isAnchor = anchor;
	toWorld = glm::mat4(1.0f);

	makePoints();

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4 * 3, vertices.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * 4 * 3, colors.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

Points::~Points()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Points::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	glUseProgram(shaderProgram);

	glPointSize(20.0f);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	GLuint colorLoc = glGetUniformLocation(shaderProgram, "uColor");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniform3f(colorLoc, color.x, color.y, color.z);
	// Now draw the Points. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size());
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Points::drawSelect(GLuint shaderProgram)
{
	glUseProgram(shaderProgram);

	glPointSize(20.0f);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	// Now draw the Points. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size());
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Points::update()
{
	makePoints();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4 * 3, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Points::makePoints()
{
	vertices.clear();
	colors.clear();
	for (int i = 0; i < Window::track->children.size(); i++)
	{
		Bezier* curve = (Bezier*)Window::track->children[i];
		if (this->isAnchor)
		{
			vertices.push_back(curve->p0);
			colors.push_back(glm::vec3((8.0f + i * 8.0f)/255.0f)); // 8, 16, 24, 32, 40, 48, 56, 64, 72, 80
			//cout << "anchor: " << curve->p0.x << " " << curve->p0.y << " " << curve->p0.z << endl;
		}
		else
		{
			vertices.push_back(curve->p1);
			vertices.push_back(curve->p2);
			colors.push_back(glm::vec3((88.0f + i * 16.0f)/255.0f)); // 88, 104, etc
			colors.push_back(glm::vec3((88.0f + i * 16.0f + 8.0f)/255.0f)); // 96, 112, etc
			//cout << "pushback " << i << " " << (88.0f + i * 16.0f) << " " << (88.0f + i * 16.0f + 8.0f) << endl;
			//cout << "handle: " << curve->p1.x << " " << curve->p1.y << " " << curve->p1.z << endl;
			//cout << "handle: " << curve->p2.x << " " << curve->p2.y << " " << curve->p2.z << endl;
		}
	}
}