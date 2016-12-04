#ifndef BUMPOBJ_H
#define BUMPOBJ_H

#include "OBJObject.h"

class BumpOBJ : public OBJObject
{
public:
	BumpOBJ(const char* filepath, const char *tex_filepath, const char *normal_filepath,
		glm::vec3 matAmb,
		glm::vec3 matDiff,
		glm::vec3 matSpec,
		float shiny);
	~BumpOBJ();

	std::vector<glm::vec3> tangents;

	GLuint tangentBO;

	GLuint normalMap;

	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);

};

#endif