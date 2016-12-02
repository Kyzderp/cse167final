#ifndef _GeodeN_H_
#define _GeodeN_H_

#include "node.h"
#include "Window.h"

class GeodeN : public Node {
public:
	GeodeN();
	~GeodeN();

	glm::mat4 toWorld = glm::mat4(1.0f);

	GLuint VBO, VAO, EBO, nVBO;
	GLuint uProjection, uModelview;
	GLint matAmbientLoc, matDiffuseLoc, matSpecularLoc, matShineLoc;

	virtual void draw(glm::mat4 C);
	virtual void render() = 0;
};

#endif