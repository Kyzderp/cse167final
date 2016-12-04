#include "QuadPrism.h"
#include "Window.h"
#include <iostream>

using namespace std;

QuadPrism::QuadPrism()
{
	toWorld = glm::mat4(1.0f);
	numBuildings = 0;
}

QuadPrism::~QuadPrism()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void QuadPrism::assignVertex(glm::vec3 point, glm::vec3 center)
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

void QuadPrism::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	//cout << "draw QuadPrism!" << endl;

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

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	
	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	//glDrawElements(GL_TRIANGLES, 36 * numBuildings, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < numBuildings; i++)
	{
		glUniform3f(colorLoc, colors[i], colors[i], colors[i]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)(36 * i * 4));
	}


	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void QuadPrism::addPoints(glm::vec3 one, glm::vec3 two, glm::vec3 three, glm::vec3 four)
{
	// Figure out which one is most negative, most positive, etc
	// Average them all and call that the center
	glm::vec3 center = one + two + three + four;
	center = center * 0.25f;
	// This probably breaks if the shape is really weird, but for city it should be ok
	assignVertex(one, center);
	assignVertex(two, center);
	assignVertex(three, center);
	assignVertex(four, center);

	np = one;
	pp = two;
	nn = three;
	pn = four;


	makeQuadPrism();
}

void QuadPrism::makeQuadPrism()
{
	float width = 0.0f;
	// First need to make room for the road, so get the point 1 away from actual vertex
	nn = nn + glm::normalize((np - nn) + (pn - nn)) * width;
	pn = pn + glm::normalize((nn - pn) + (pp - pn)) * width;
	np = np + glm::normalize((nn - np) + (pp - np)) * width;
	pp = pp + glm::normalize((np - pp) + (pn - pp)) * width;

	float height = 5.0f + rand() % 8;
	// Now goes in buffer
	bufferVertices.push_back(np);
	bufferVertices.push_back(pp);
	bufferVertices.push_back(pp + glm::vec3(0.0f, height, 0.0f));
	bufferVertices.push_back(np + glm::vec3(0.0f, height, 0.0f));

	bufferVertices.push_back(nn);
	bufferVertices.push_back(pn);
	bufferVertices.push_back(pn + glm::vec3(0.0f, height, 0.0f));
	bufferVertices.push_back(nn + glm::vec3(0.0f, height, 0.0f));

	// Keep repeating D:
	unsigned int blah[36] = { 0, 1, 2, 2, 3, 0,
		// Top face
	1, 5, 6, 6, 2, 1,
		// Back face
	7, 6, 5, 5, 4, 7,
		// Bottom face
	4, 0, 3, 3, 7, 4,
		// Left face
	4, 5, 1, 1, 0, 4,
		// Right face
	3, 2, 6, 6, 7, 3 };

	for (int i = 0; i < 36; i++)
		blah[i] = blah[i] + 8 * numBuildings;

	bufferIndices.insert(bufferIndices.end(), blah, blah + 36);

	colors.push_back(((float)rand() / (float)RAND_MAX) * 0.35 + 0.5);

	numBuildings++;
}

void QuadPrism::makeBuffers()
{
	cout << "We have " << numBuildings << " buildings!" << endl;
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
	glBufferData(GL_ARRAY_BUFFER, bufferVertices.size() * 3 * 4, bufferVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferIndices.size() * 4, bufferIndices.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}