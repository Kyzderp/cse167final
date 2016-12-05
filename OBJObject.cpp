#include "OBJObject.h"
#include "Skybox.h"
#include "Window.h"
#include <algorithm>

using namespace std;

OBJObject::OBJObject(const char *filepath, const char *tex_filepath, glm::vec3 matAmb,
																glm::vec3 matDiff,
																glm::vec3 matSpec,
																float shiny)
{
	this->default = parse(filepath);
	this->toWorld = default;

	textureMap = loadTexture(tex_filepath);

	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &nVBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &tcVBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*3*4, vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); 

	glBindBuffer(GL_ARRAY_BUFFER, nVBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * 4, normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, tcVBO);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * 2 * 4, texCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*4, indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	this->matAmb = matAmb;
	this->matDiff = matDiff;
	this->matSpec = matSpec;
	this->shiny = shiny;
}

glm::mat4 OBJObject::parse(const char *filepath) 
{
	FILE* fp;     // file pointer
	float x, y, z;
	float xn, yn, zn;
	float xt, yt, zt;
	int c1, c2;
	unsigned int i = 0;

	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec2> temp_texCoords;


	fp = fopen(filepath, "rb");
	if (fp == NULL) { cerr << "error loading file" << endl; }  // just in case the file can't be found or is corrupt


	while ((c1 = fgetc(fp)) != EOF)
	{
		c2 = fgetc(fp);
		if ((c1 == 'v') && (c2 == ' '))
		{
			fscanf(fp, "%f %f %f", &x, &y, &z);
			glm::vec3 v = glm::vec3(x, y, z);
			
			temp_vertices.push_back(v);
		}
		else if ((c1 == 'v') && (c2 == 'n')) 
		{
			fscanf(fp, "%f %f %f", &x, &y, &z);
			temp_normals.push_back(glm::vec3(x, y, z));
		}
		else if ((c1 == 'v') && (c2 == 't'))
		{
			fscanf(fp, "%f %f", &x, &y);
			temp_texCoords.push_back(glm::vec2(x, 1.0f - y)); // had to invert for blender exports?
		}

		else if ((c1 == 'f') && (c2 == ' '))
		{
			fscanf(fp, "%f/%f/%f %f/%f/%f %f/%f/%f", &x, &xt, &xn, &y, &yt, &yn, &z, &zt, &zn);

			// Because we have to use indices?
			this->indices.push_back(i++);
			this->indices.push_back(i++);
			this->indices.push_back(i++);

			this->texCoords.push_back(temp_texCoords[(unsigned int)xt - 1]);
			this->texCoords.push_back(temp_texCoords[(unsigned int)yt - 1]);
			this->texCoords.push_back(temp_texCoords[(unsigned int)zt - 1]);

			this->normals.push_back(temp_normals[(unsigned int)xn - 1]);
			this->normals.push_back(temp_normals[(unsigned int)yn - 1]);
			this->normals.push_back(temp_normals[(unsigned int)zn - 1]);

			this->vertices.push_back(temp_vertices[(unsigned int)x - 1]);
			this->vertices.push_back(temp_vertices[(unsigned int)y - 1]);
			this->vertices.push_back(temp_vertices[(unsigned int)z - 1]);
		}
		else {
			ungetc(c2, fp);
		}
	}
	fclose(fp);

	return glm::mat4(1.0f);
}

OBJObject::~OBJObject()
{
	// Delete previously generated buffers.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &nVBO);
	glDeleteBuffers(1, &tcVBO);
}

void OBJObject::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	glFrontFace(GL_CCW);
	glUseProgram(shaderProgram);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * this->toWorld * C;

	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMap);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	GLint matShineLoc = glGetUniformLocation(shaderProgram, "matShininess");
	glUniform1f(matShineLoc, shiny);

	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void OBJObject::update()
{

}

void OBJObject::spin(float deg)
{
	this->toWorld *= glm::rotate(glm::mat4(1.0f), deg, glm::vec3(0.0f, 1.0f, 0.0f));
}

void OBJObject::rotate(glm::vec3 axis, float deg) {
	this->toWorld = glm::rotate(glm::mat4(1.0f), deg * 0.007f, axis) * this->toWorld;
}

void OBJObject::move(glm::vec3 movementVec)
{
	this->toWorld = glm::translate(glm::mat4(1.0f), movementVec) * this->toWorld;
}

void OBJObject::scale(float factor) {
	this->toWorld = this->toWorld * glm::scale(glm::mat4(1.0f), glm::vec3(factor));
}

void OBJObject::rotateZ(float deg) {
	float rad = deg * glm::pi<float>() / 180.0f;
	this->toWorld = glm::rotate(glm::mat4(1.0f), rad, glm::vec3(0, 0, 1.0f)) * this->toWorld ;
}

void OBJObject::reset() {
	this->toWorld = default;
}

GLuint OBJObject::loadTexture(const GLchar* path)
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