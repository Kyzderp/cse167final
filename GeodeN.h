#ifndef _GEODE_H_
#define _GEODE_H_

#include "node.h"
#include "../Window.h"

class Geode : public Node {
public:
	Geode();
	~Geode();

	glm::mat4 toWorld = glm::mat4(1.0f);

	GLuint VBO, VAO, EBO, nVBO;
	GLuint uProjection, uModelview;
	GLint matAmbientLoc, matDiffuseLoc, matSpecularLoc, matShineLoc;

	virtual void draw(glm::mat4 C);
	virtual void render() = 0;
};

#endif