#include "Bezier.h"
#include "Window.h"
#include <iostream>

using namespace std;

Bezier::Bezier(Bezier* previous, glm::vec3 point3, int id)
{
	this->id = id;
	toWorld = glm::mat4(1.0f);

	float x, y, z;
	if (previous == NULL)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	else
	{
		x = previous->p3.x;
		y = previous->p3.y;
		z = previous->p3.z;
		this->prev = previous;
		previous->next = this;
	}

	p0 = glm::vec3(x, y, z);
	p3 = point3;
	// Interpolate for p1 and p2 just so it isn't super ugly
	p1 = (p3 - p0) * 0.3f + p0;
	p2 = (p3 - p0) * 0.7f + p0;

	// But if previous exists, need to line up with its p2
	if (prev != NULL)
		p1 = p0 + prev->p3 - prev->p2;

	makeBezier();

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

	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

Bezier::~Bezier()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Bezier::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	glUseProgram(shaderProgram);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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
	// Now draw the Bezier. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	glDrawArrays(GL_LINES, 0, (GLsizei)vertices.size() * 3);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Bezier::update()
{
		makeBezier();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4 * 3, vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Bezier::makeBezier()
{
	vertices.clear();
	glm::vec3 prev = makePoint(0.0f);
	glm::vec3 curr;
	for (int segment = 1; segment < 150; segment++)
	{
		float t = segment / 150.0f;
		curr = makePoint(t);
		vertices.push_back(prev);
		vertices.push_back(curr);
		prev = curr;
		//cout << curr.x << " " << curr.y << " " << curr.z << endl;
	}
}

glm::vec3 Bezier::makePoint(float t)
{
	float c0 = (1.0f - t) * (1.0f - t) * (1.0f - t); // 1 * t^0 * (1 - t)^3
	float c1 = 3 * t * (1.0f - t) * (1.0f - t); // 3 * t * (1 - t)^2
	float c2 = 3 * t * t * (1.0f - t); // 3 * t^2 * (1 - t)
	float c3 = t * t * t; // 1 * t^3 * (1 - t)^0

	glm::mat4x3 controls = glm::mat4x3(p0, p1, p2, p3);
	glm::vec4 constants = glm::vec4(c0, c1, c2, c3);

	glm::vec3 point =  controls * constants;

	if (point.y > Window::highestPoint.y)
	{
		Window::highestPoint = point;
		Window::highestTime = (float)this->id + t;
	}

	return point;
}

void Bezier::setPrev(Bezier* previous)
{
	this->prev = previous;
	previous->next = this;
	this->p0 = previous->p3;

	cout << "new prev: " << p0.x << " " << p0.y << " " << p0.z << endl;

	p1 = p0 + prev->p3 - prev->p2;

	this->update();
}

void Bezier::moveAnchor(glm::vec3 move)
{
	p0 = p0 + move;
	prev->p3 = p0;
	prev->p2 = prev->p2 + move;
	p1 = p1 + move;

	update();
	prev->update();

	Window::anchors->update();
	Window::handles->update();
	Window::lines->update();
}

void Bezier::move1(glm::vec3 move)
{
	p1 = p1 + move;
	prev->p2 = p0 - p1 + p0;

	Window::track->update();

	Window::anchors->update();
	Window::handles->update();
	Window::lines->update();
}

void Bezier::move2(glm::vec3 move)
{
	p2 = p2 + move;
	next->p1 = p3 - p2 + p3;

	Window::track->update();

	Window::anchors->update();
	Window::handles->update();
	Window::lines->update();
}