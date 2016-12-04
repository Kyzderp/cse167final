#include "Floor.h"
#include "Window.h"
#include <iostream>

using namespace std;

GLuint floorTexture;

Floor::Floor()
{
	// Google API key, but this seems too hard to use AIzaSyBesNtg7B1lJXiXAB1COwRSNawtzO5uVwU

	toWorld = glm::mat4(1.0f);
	size = 50.0f;

	makeFloor();

	floorTexture = loadTexture("../objects/asphalt.ppm");

	// Array object and buffers for road lines (debug purposes?)
	glGenVertexArrays(1, &roadVAO);
	glGenBuffers(1, &roadVBO);
	glGenBuffers(1, &roadEBO);

	glBindVertexArray(roadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, roadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadVertices) * 4 * 4, roadVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(roadIndices) * 4 * 4, roadIndices.data(), GL_STATIC_DRAW);
	cout << "sizeof(roadIndices)" << sizeof(roadIndices) << endl;

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &TBO);
	glGenBuffers(1, &EBO);
	
	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * 3 * 4, vertices.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * 4 * 3, normals.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * 4 * 2, texCoords.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		2, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		2 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.


	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * 4, indices.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

Floor::~Floor()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &TBO);

	glDeleteVertexArrays(1, &roadVAO);
	glDeleteBuffers(1, &roadVBO);
	glDeleteBuffers(1, &roadEBO);
}

void Floor::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{ 
	//cout << "draw Floor!" << endl;
	glFrontFace(GL_CW);

	glUseProgram(shaderProgram);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * C * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	GLuint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
	glUniform3f(colorLoc, color.x, color.y, color.z);
	// Now draw the Floor. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);

	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);

	// Draw roads
	drawRoads(Window::solidShader, C, glm::vec3(1.0f, 0.0f, 0.0f));

	// Draw blocks
	for (int i = 0; i < blocks.size(); i++)
		blocks[i]->draw(Window::solidShader, C, glm::vec3(0.5f, 0.5f, 0.5f));
}

void Floor::drawRoads(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	glUseProgram(shaderProgram);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * C * toWorld;
	uProjection = glGetUniformLocation(Window::solidShader, "projection");
	uModelview = glGetUniformLocation(Window::solidShader, "modelview");
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	GLuint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
	glUniform3f(colorLoc, color.x, color.y, color.z);

	glBindVertexArray(roadVAO);
	glDrawElements(GL_LINES, (GLsizei)roadIndices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Floor::makeFloor()
{
	vertices.push_back(glm::vec3(size, 0.0f, size));
	vertices.push_back(glm::vec3(-size, 0.0f, size));
	vertices.push_back(glm::vec3(-size, 0.0f, -size));
	vertices.push_back(glm::vec3(size, 0.0f, -size));

	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	float scale = 8.0f;
	texCoords.push_back(glm::vec2(1.0f, 1.0f) * scale);
	texCoords.push_back(glm::vec2(-1.0f, 1.0f) * scale);
	texCoords.push_back(glm::vec2(-1.0f, -1.0f) * scale);
	texCoords.push_back(glm::vec2(1.0f, -1.0f) * scale);

	makeRoads();
}

void Floor::makeRoads()
{
	int xn = 4;
	int zn = 4;
	float xdist = size * 2.0f / xn;
	float zdist = size * 2.0f / zn;

	float xvar = xdist / 5.0f;
	float zvar = zdist / 5.0f;

	for (int x = 0; x < xn + 1; x++)
	{
		for (int z = 0; z < zn + 1; z++)
		{
			float xCoord = -size + x * xdist;
			xCoord += xvar * ((float)rand() / (float)RAND_MAX) * 2 - xvar;
			float zCoord = -size + z * zdist;
			zCoord += zvar * ((float)rand() / (float)RAND_MAX) * 2 - zvar;
			roadVertices.push_back(glm::vec3(xCoord, 0.01f, zCoord));
			cout << "vertex: " << xCoord << " " << zCoord << endl;
		}
	}

	// Line indices in one direction
	for (int z = 0; z < zn + 1; z++)
	{
		for (int x = 1; x < xn + 1; x++)
		{
			roadIndices.push_back((x - 1) * (zn + 1) + z);
			roadIndices.push_back(x * (zn + 1) + z);
			cout << (x - 1) * (zn + 1) + z << " " << x * (zn + 1) + z << endl;
		}
	}

	// Other direction
	for (int x = 0; x < xn + 1; x++)
	{
		for (int z = 1; z < zn + 1; z++)
		{
			roadIndices.push_back(z - 1 + x * (zn + 1));
			roadIndices.push_back(z + x * (zn + 1));
			cout << z - 1 + x * (zn + 1) << " " << z + x * (zn + 1) << endl;
		}
	}

	// Now the blocks
	for (int x = 0; x < xn; x++)
	{
		for (int z = 0; z < zn; z++)
		{
			// take down left as "origin"
			int dl = (x * (zn + 1)) + z;
			int dr = dl + 1;
			int ul = dl + zn + 1;
			int ur = ul + 1;

			blocks.push_back(new Block(roadVertices[dl], roadVertices[dr], roadVertices[ul], roadVertices[ur]));
		}
	}

	
}

GLuint Floor::loadTexture(const GLchar* path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_2D, textureID);
	image = Skybox::loadPPM(path, width, height);
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
	);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}
