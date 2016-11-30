#include "Sphere.h"
#include "Window.h"
#include <iostream>
#include "Skybox.h"

using namespace std;

GLuint skyboxTexture;

Sphere::Sphere(int wireframe)
{
	toWorld = glm::mat4(1.0f);
	this->wireframe = wireframe;

	makeSphere();

	vector<const GLchar*> faces;
	faces.push_back("skybox/mudskipper_rt.ppm");
	faces.push_back("skybox/mudskipper_lf.ppm");
	faces.push_back("skybox/mudskipper_up.ppm");
	faces.push_back("skybox/mudskipper_dn.ppm");
	faces.push_back("skybox/mudskipper_bk.ppm");
	faces.push_back("skybox/mudskipper_ft.ppm");
	skyboxTexture = loadCubemap(faces);

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
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
	glDeleteBuffers(1, &EBO);
}

void Sphere::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 cam_pos)
{ 
	glUseProgram(shaderProgram);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 model = C * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModel = glGetUniformLocation(shaderProgram, "model");
	uView = glGetUniformLocation(shaderProgram, "view");
	GLuint camLoc = glGetUniformLocation(shaderProgram, "cameraPos");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);
	glUniform3f(camLoc, cam_pos.x, cam_pos.y, cam_pos.z);
	// Now draw the Sphere. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	// Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Select GL_MODULATE to mix texture with polygon color for shading:
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// Use bilinear interpolation:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Use clamp to edge to hide skybox edges:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
	float radius = 0.5f;

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
	}
}

GLuint Sphere::loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = loadPPM(faces[i], width, height);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

unsigned char* Sphere::loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = (unsigned int)fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}