#ifndef _BOUNCETRANSFORM_H_
#define _BOUNCETRANSFORM_H_

#include "MatrixTransform.h"

class BounceTransform : public MatrixTransform
{
public:
	BounceTransform(glm::mat4 transformMat, glm::mat4 scaleMat);
	~BounceTransform();

	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	void update();
};

#endif

