#include "../BumpOBJ.h"
#include "../Skybox.h"
#include "../Window.h"

BumpOBJ::BumpOBJ(const char *filepath, const char *tex_filepath, const char *normal_filepath, glm::vec3 matAmb,
	glm::vec3 matDiff,
	glm::vec3 matSpec,
	float shiny) 
	: OBJObject(filepath, tex_filepath, matAmb, matDiff, matSpec, shiny)
{
	// load normal texture map
	normalMap = loadTexture(normal_filepath);


	// now just run the extra code to generate tangent vectors
	for (unsigned int i = 0; i < indices.size(); i += 3) {
		glm::vec3 v0 = vertices[indices[i]];
		glm::vec3 v1 = vertices[indices[i + 1]];
		glm::vec3 v2 = vertices[indices[i + 2]];

		glm::vec3 Edge1 = v1 - v0;
		glm::vec3 Edge2 = v2 - v0;

		float DeltaU1 = texCoords[indices[i+1]].x - texCoords[indices[i]].x;
		float DeltaV1 = texCoords[indices[i+1]].y - texCoords[indices[i]].y;
		float DeltaU2 = texCoords[indices[i+2]].x - texCoords[indices[i]].x;
		float DeltaV2 = texCoords[indices[i+2]].y - texCoords[indices[i]].y;

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		glm::vec3 Tangent, Bitangent;

		Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
		Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
		Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

		Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
		Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
		Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);

		tangents.push_back(glm::normalize(Tangent));
		tangents.push_back(glm::normalize(Tangent)); 
		tangents.push_back(glm::normalize(Tangent));

		biTangents.push_back(glm::normalize(Bitangent));
		biTangents.push_back(glm::normalize(Bitangent));
		biTangents.push_back(glm::normalize(Bitangent));
	}

	// Now bind the tangent and bitangent buffers
	glGenBuffers(1, &tangentBO);
	glGenBuffers(1, &biTangentBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tangentBO);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * 3 * 4, tangents.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, biTangentBO);
	glBufferData(GL_ARRAY_BUFFER, biTangents.size() * 3 * 4, biTangents.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

BumpOBJ::~BumpOBJ()
{
	glDeleteBuffers(1, &tangentBO);
	glDeleteBuffers(1, &biTangentBO);
}

void BumpOBJ::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	glFrontFace(GL_CCW);
	glUseProgram(shaderProgram);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * this->toWorld;

	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMap);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, normalMap);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	GLint matShineLoc = glGetUniformLocation(shaderProgram, "matShininess");
	glUniform1f(matShineLoc, shiny);

	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
