#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Geode.h"

class Sphere : public Geode {
public:
	Sphere(glm::vec3 c, float r, float slice, float stack, float width);

	glm::vec3 color;
	glm::vec3 colorSpec;
	float shiny = 32.0f;

	int SEGMENTS = 16;
	float RADIUS;
	float SLICES = 10.0f; // 135
	float STACKS = 5.0f;

	std::vector<glm::vec3> points;

	//virtual void draw(glm::mat4 C);
	virtual void render();
	virtual void update();
};

#endif