#include "BoundedGroup.h"

BoundedGroup::BoundedGroup(glm::vec3 min, glm::vec3 max)
{
	minPoint = min;
	maxPoint = max;
}

void BoundedGroup::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	if (dead) return;

	color = glm::vec3(0, 1.0f, 0);

	if (checkBounds(C)) {
		Window::inCollision = true;

		if (Window::showBB) {
			color = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else {
			Window::se->play2D("../audio/splat.wav", GL_FALSE);
			dead = true;
		}
	}


	for (int i = 0; i < children.size(); i++) {
		children[i]->draw(shaderProgram, C, color);
	}
}

bool BoundedGroup::checkBounds(glm::mat4 C) 
{
    // From: http://www.miguelcasillas.com/?p=30

	glm::vec3 bMax = glm::vec3(C * glm::vec4(maxPoint, 1.0f));
	glm::vec3 bMin = glm::vec3(C * glm::vec4(minPoint, 1.0f));
	glm::vec3 oMax = Window::orangeMax;
	glm::vec3 oMin = Window::orangeMin;

	//printf("bananaMin: %f, %f, %f\n", bMin.x, bMin.y, bMin.z);
	//printf("bananaMax: %f, %f, %f\n", bananaMax.x, bananaMax.y, bananaMax.z);


	//Check if Box1's max is greater than Box2's min and Box1's min is less than Box2's max
  return(oMax.x > bMin.x &&
		oMin.x < bMax.x &&
		oMax.y > bMin.y &&
		oMin.y < bMax.y &&
		-oMax.z > -bMin.z &&
		-oMin.z < -bMax.z);

	/*return(!((oMin.x > bMax.x && oMin.y > bMax.y && -oMin.z > -bMax.z) ||
		(bMin.x > oMax.x && bMin.y > oMax.y && -bMin.z > -oMax.z) ||
		(oMax.x < bMin.x && oMax.y < bMin.y && -oMax.z < -bMin.z) ||
		(bMax.x < oMin.x && bMax.y < oMin.y && -bMax.z < -oMin.z)));*/

	/*return( (
		(-bMin.z < -oMax.z && -bMin.z > -oMin.z)
		  && (bMin.x < oMax.x && bMin.x > oMin.x)     
		  && (bMin.y < oMax.y && bMin.y > oMin.y)
		) ||
		(
		     (bMax.x > oMin.x && bMax.x < oMax.x)
		  && (-bMax.z > -oMin.z && -bMax.z < -oMax.z)
	      && (bMax.y > oMin.y && bMax.y < oMax.y)
		) || 
		(
			(oMin.x > bMin.x && oMin.x < bMax.x) &&
			(oMin.y > bMin.y && oMin.y < bMax.y) &&
			(-oMin.z > -bMin.z && -oMin.z < -bMax.z)
		) ||
		(
			(oMax.x > bMin.x && oMax.x < bMax.x) &&
			(oMax.y > bMin.y && oMax.y < bMax.y) &&
			(-oMax.z > -bMin.z && -oMax.z < -bMax.z)
		)
		);*/


	//If not, it will return false
}
