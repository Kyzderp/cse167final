#include "Sphere.h"
#include "Window.h"
#include <iostream>
#include "Floor.h"

using namespace std;

GLuint sphereTexture;

Sphere::Sphere(int wireframe)
{
	toWorld = glm::mat4(1.0f);
	this->wireframe = wireframe;

	makeSphere();

	sphereTexture = Floor::loadTexture("../objects/globe.ppm");

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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4 * 3, vertices.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4 * 3, indices.data(), GL_STATIC_DRAW);

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

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

Sphere::~Sphere()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &TBO);
	glDeleteBuffers(1, &EBO);
}

void Sphere::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 cam_pos)
{ 
	glUseProgram(shaderProgram);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);

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
	glUniform3f(colorLoc, color.x, color.y, color.z);

	// Now draw the Sphere. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sphereTexture);

	glDrawElements(GL_TRIANGLES, (GLsizei)vertices.size() * 3 * 3, GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Sphere::update()
{
	
}

void Sphere::makeSphere()
{
	int slices = 20;
	int stacks = 10;
	float pi = glm::pi<float>();
	float radius = 1.0f;

	int angleInc = 180 / stacks;
	// theta is angle from Z axis
	for (int theta = 0; theta <= 180; theta += angleInc) 
	{
		// Figure out what the r and z are
		// cos(theta) = z/radius
		float z = cos(theta / 180.0f * pi) * radius;
		// sin(theta) = r/radius
		float r = sin(theta / 180.0f * pi) * radius;

		makeCircle(slices, r, z);
	}

	if (wireframe)
	{
		// The above draws the stacks. Now we need to connect the stacks.
		vector<glm::vec3> sliceVerts;
		for (int i = 0; i < slices * 2; i++)
		{
			// For this slice, draw lines in between all the stacks
			glm::vec3 prev = vertices[i];
			for (int j = 1; j < stacks + 1; j++)
			{
				int index = j * slices * 2 + i;
				glm::vec3 curr = vertices[index];
				sliceVerts.push_back(prev);
				sliceVerts.push_back(curr);

				prev = curr;
			}
		}

		// Stick them in
		for (int i = 0; i < sliceVerts.size(); i++)
		{
			vertices.push_back(sliceVerts[i]);
		}
	}
	else
	{
		// If not wireframe, then have to populate the indices
		// start at the bottom stack, go in a circle connecting the next stack
		for (int j = 0; j < stacks; j++)
		{
			for (int i = 0; i < slices * 2; i++)
			{
				int index = j * slices * 2 + i;
				int adjacent = index + 1;

				int nextLayer = index + slices * 2;
				int nextAdj = nextLayer + 1;

				if (i == 19)
				{
					adjacent = index - i;
					nextAdj = index - i + slices;
				}

				indices.push_back(index); 
				indices.push_back(adjacent);
				indices.push_back(nextLayer);

				indices.push_back(nextLayer);
				indices.push_back(adjacent);
				indices.push_back(nextAdj);
			}
		}
	}
}

void Sphere::makeCircle(int slices, float radius, float z)
{
	float pi = glm::pi<float>();

	for (int i = 0; i < slices; i++)
	{
		float angle = 2 * pi * i / slices;
		float next_angle = 2 * pi * (i + 1) / slices;

		float x1 = cos(angle) * radius;
		float y1 = sin(angle) * radius;
		float x2 = cos(next_angle) * radius;
		float y2 = sin(next_angle) * radius;
		glm::vec3 one = glm::vec3(x1, y1, z);
		vertices.push_back(one);
		glm::vec3 two = glm::vec3(x2, y2, z);
		vertices.push_back(two);

		normals.push_back(one);
		normals.push_back(two);

		texCoords.push_back(glm::vec2(one.x / 2 + 0.5, one.y / 2 + 0.5));
		texCoords.push_back(glm::vec2(two.x / 2 + 0.5, two.y / 2 + 0.5));
	}
}
