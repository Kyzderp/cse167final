#include "Block.h"
#include "Window.h"
#include <iostream>

using namespace std;

GLuint BlockTexture;

Block::Block(glm::vec3 one, glm::vec3 two, glm::vec3 three, glm::vec3 four)
{
	toWorld = glm::mat4(1.0f);

	// Figure out which one is most negative, most positive, etc
	// Average them all and call that the center
	glm::vec3 center = one + two + three + four;
	center = center * 0.25f;
	// This probably breaks if the shape is really weird, but for city it should be ok
	assignVertex(one, center);
	assignVertex(two, center);
	assignVertex(three, center);
	assignVertex(four, center);

	makeBlock();

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
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bufferVertices) * 3 * 4, bufferVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

					 // We've sent the vertex data over to OpenGL, but there's still something missing.
					 // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufferIndices), bufferIndices, GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

Block::~Block()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Block::assignVertex(glm::vec3 point, glm::vec3 center)
{
	glm::vec3 relative = point - center;
	if (relative.x < 0)
	{
		if (relative.z < 0)
			nn = point;
		else
			np = point;
	}
	else
	{
		if (relative.z < 0)
			pn = point;
		else
			pp = point;
	}
}

void Block::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	//cout << "draw block!" << endl;

	glUseProgram(shaderProgram);
	glFrontFace(GL_CCW);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * C * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	GLuint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
	glUniform3f(colorLoc, color.x, color.y, color.z);
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	
	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Block::makeBlock()
{
	float width = 4.0f;
	// First need to make room for the road, so get the point 1 away from actual vertex
	nn = nn + glm::normalize((np - nn) + (pn - nn)) * width;
	pn = pn + glm::normalize((nn - pn) + (pp - pn)) * width;
	np = np + glm::normalize((nn - np) + (pp - np)) * width;
	pp = pp + glm::normalize((np - pp) + (pn - pp)) * width;

	// Now goes in buffer
	bufferVertices.push_back(np);
	bufferVertices.push_back(pp);
	bufferVertices.push_back(pp + glm::vec3(0.0f, 2.0f, 0.0f));
	bufferVertices.push_back(np + glm::vec3(0.0f, 2.0f, 0.0f));

	bufferVertices.push_back(nn);
	bufferVertices.push_back(pn);
	bufferVertices.push_back(pn + glm::vec3(0.0f, 2.0f, 0.0f));
	bufferVertices.push_back(nn + glm::vec3(0.0f, 2.0f, 0.0f));

	/*[8][3] = {
		// "Front" vertices
		{ -2.0, -2.0,  2.0 },{ 2.0, -2.0,  2.0 },{ 2.0,  2.0,  2.0 },{ -2.0,  2.0,  2.0 },
		// "Back" vertices
		{ -2.0, -2.0, -2.0 },{ 2.0, -2.0, -2.0 },{ 2.0,  2.0, -2.0 },{ -2.0,  2.0, -2.0 }
	};*/

	makeBuildings();
}

void Block::makeBuildings()
{
	float width = glm::min(
		glm::min(glm::length(np - nn), glm::length(np - pp)), 
		glm::min(glm::length(pn - pp), glm::length(pn - nn))
	);
	width = width * 0.4;

	glm::vec3 inner_nn = nn + glm::normalize((np - nn) + (pn - nn)) * width;
	glm::vec3 inner_pn = pn + glm::normalize((nn - pn) + (pp - pn)) * width;
	glm::vec3 inner_np = np + glm::normalize((nn - np) + (pp - np)) * width;
	glm::vec3 inner_pp = pp + glm::normalize((np - pp) + (pn - pp)) * width;

	// This is really bad

	// bottom
	glm::vec3 left_n = nn + glm::normalize(pn - nn) * width;
	glm::vec3 right_n = pn + glm::normalize(nn - pn) * width;

	// left
	glm::vec3 n_down = nn + glm::normalize(np - nn) * width;
	glm::vec3 n_up = np + glm::normalize(nn - np) * width;

	// right
	glm::vec3 p_down = pn + glm::normalize(pp - pn) * width;
	glm::vec3 p_up = pp + glm::normalize(pn - pp) * width;

	// top
	glm::vec3 left_p = np + glm::normalize(pp - np) * width;
	glm::vec3 right_p = pp + glm::normalize(np - pp) * width;


	// top left
	Window::buildings->addPoints(np, left_p, n_up, inner_np);

	// top mid
	Window::buildings->addPoints(left_p, right_p, inner_np, inner_pp);

	// top right
	Window::buildings->addPoints(right_p, pp, inner_pp, p_up);

	// mid left
	Window::buildings->addPoints(n_up, inner_np, n_down, inner_nn);

	// mid mid
	Window::buildings->addPoints(inner_np, inner_pp, inner_nn, inner_pn);

	// mid right
	Window::buildings->addPoints(inner_pp, p_up, inner_pn, p_down);

	// bottom left
	Window::buildings->addPoints(n_down, inner_nn, nn, left_n);

	// bottom mid
	Window::buildings->addPoints(inner_nn, inner_pn, left_n, right_n);

	// bottom right
	Window::buildings->addPoints(inner_pn, p_down, right_n, pn);
}