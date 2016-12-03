// Nicholas Deagon A11583792

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

	glGenTextures(1, &textureMap);
	int width, height;
	unsigned char* image;

	image = Skybox::loadPPM(tex_filepath, width, height);
	glBindTexture(GL_TEXTURE_2D, textureMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);


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
	float r, g, b;
	int c1, c2;
	unsigned int i = 0;

	float maxLength = 0.0f;
	float maxX = -1000.0f;
	float minX = 1000.0f;
	float maxY = -1000.0f;
	float minY = 1000.0f;
	float maxZ = -1000.0f;
	float minZ = 1000.0f;

	fp = fopen(filepath, "rb");
	if (fp == NULL) { cerr << "error loading file" << endl; }  // just in case the file can't be found or is corrupt


	while ((c1 = fgetc(fp)) != EOF)
	{
		c2 = fgetc(fp);
		if ((c1 == 'v') && (c2 == ' '))
		{
			fscanf(fp, "%f %f %f", &x, &y, &z);
			glm::vec3 v = glm::vec3(x, y, z);

			if (x > maxX) maxX = x;
			if (x < minX) minX = x;
			if (y > maxY) maxY = y;
			if (y < minY) minY = y;
			if (z > maxZ) maxZ = z;
			if (z < minZ) minZ = z;
			
			this->temp_vertices.push_back(v);
		}
		else if ((c1 == 'v') && (c2 == 'n')) 
		{
			fscanf(fp, "%f %f %f", &x, &y, &z);
			this->normals.push_back(glm::vec3(x, y, z));
		}
		else if ((c1 == 'v') && (c2 == 't'))
		{
			fscanf(fp, "%f %f", &x, &y);
			this->temp_texCoords.push_back(glm::vec2(x, y));
		}

		else if ((c1 == 'f') && (c2 == ' '))
		{
			fscanf(fp, "%f/%f/%f %f/%f/%f %f/%f/%f", &x, &xt, &xn, &y, &yt, &yn, &z, &zt, &zn);

			//this->indices.push_back((unsigned int)x-1);
			//this->indices.push_back((unsigned int)y-1);
			//this->indices.push_back((unsigned int)z-1);

			this->indices.push_back(i++);
			this->indices.push_back(i++);
			this->indices.push_back(i++);

			this->texCoords.push_back(temp_texCoords[(unsigned int)xt - 1]);
			this->texCoords.push_back(temp_texCoords[(unsigned int)yt - 1]);
			this->texCoords.push_back(temp_texCoords[(unsigned int)zt - 1]);

			printf("texCoords: %f/%f/%f\n", xt, yt, zt);

			this->vertices.push_back(temp_vertices[(unsigned int)x - 1]);
			this->vertices.push_back(temp_vertices[(unsigned int)y - 1]);
			this->vertices.push_back(temp_vertices[(unsigned int)z - 1]);


		}
		else {
			ungetc(c2, fp);
		}
	}
	fclose(fp);

	float avgX = (maxX + minX) / 2.0f;
	float avgY = (maxY + minY) / 2.0f;
	float avgZ = (maxZ + minZ) / 2.0f;


	maxLength = max( max( maxX - minX, maxY - minY ), maxZ - minZ );
	
	//for (int i = 0; i < vertices.size(); i++) {
	//	vertices[i].x = (vertices[i].x - avgX) / maxLength;
	//	vertices[i].y = (vertices[i].y - avgY) / maxLength;
	//	vertices[i].z = (vertices[i].z - avgZ) / maxLength;
	//}

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

void OBJObject::draw(GLuint shaderProgram)
{
	glFrontFace(GL_CCW);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * this->toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"

	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMap);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);


	GLint matAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
	GLint matDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
	GLint matShineLoc = glGetUniformLocation(shaderProgram, "material.shininess");
	glUniform3f(matAmbientLoc, matAmb.x, matAmb.y, matAmb.z);
	glUniform3f(matDiffuseLoc, matDiff.x, matDiff.y, matDiff.z);
	glUniform3f(matSpecularLoc, matSpec.x, matSpec.y, matSpec.z);
	glUniform1f(matShineLoc, shiny);

	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void OBJObject::update()
{
	//spin(0.01f);
}

void OBJObject::spin(float deg)
{
	//this->angle += deg;
	//if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	// This creates the matrix to rotate the cube
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
	float rad = deg * glm::pi<float>() / 180.0;
	this->toWorld = glm::rotate(glm::mat4(1.0f), rad, glm::vec3(0, 0, 1.0f)) * this->toWorld ;
}

void OBJObject::reset() {
	this->toWorld = default;
}