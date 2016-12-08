#include "Block.h"
#include "Window.h"
#include <iostream>
#include "OBJObject.h"

using namespace std;

GLuint BlockTexture;

Block::Block(glm::vec3 one, glm::vec3 two, glm::vec3 three, glm::vec3 four, int type)
{
	saladTexture = OBJObject::loadTexture("../objects/salad.ppm");
	this->collidesSphere = 0;
	this->type = type;
	toWorld = glm::mat4(1.0f);

	// Figure out which one is most negative, most positive, etc
	// Average them all and call that the center
	center = one + two + three + four;
	center = center * 0.25f;

	// This probably breaks if the shape is really weird, but for city it should be ok
	assignVertex(one, center);
	assignVertex(two, center);
	assignVertex(three, center);
	assignVertex(four, center);

	makeBlock();

	if (type == 2)
	{
		MatrixTransform* stuff = new MatrixTransform(glm::translate(glm::mat4(1.0f), center),
			glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)));
		stuff->addChild(Window::housie);
		Window::housies->addChild(stuff);
	}

	if (type == 0)
		makeBuildings();

	glGenVertexArrays(1, &bbVAO);
	glGenBuffers(1, &bbVBO);
	glGenBuffers(1, &bbEBO);

	glBindVertexArray(bbVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bbVBO);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices.size() * 3 * 4, bufferVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bbEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bbIndices.size() * 4, bbIndices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &TBO);
	glGenBuffers(1, &NBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices.size() * 3 * 4, bufferVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * 4, normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * 2 * 4, textureCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

					 // We've sent the vertex data over to OpenGL, but there's still something missing.
					 // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * 4, bufferIndices, GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

Block::~Block()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &TBO);
}

void Block::assignVertex(glm::vec3 point, glm::vec3 center)
{
	glm::vec3 relative = point - center;
	if (relative.x < 0)
	{
		if (relative.z < 0)
			nn = point;
		else
			np = point;
	}
	else
	{
		if (relative.z < 0)
			pn = point;
		else
			pp = point;
	}
}

void Block::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color)
{
	//cout << "draw block!" << endl;
	if (type == 1 || type == 2)
		color = glm::vec3(0.2f, 0.8f, 0.3f);
	else
		return;

	glUseProgram(shaderProgram);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * C * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, saladTexture);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Block::drawBB(GLuint shaderProgram, glm::mat4 C)
{
	//cout << "draw block!" << endl;
	if (type == 1 || type == 2)
		return;

	glm::vec3 color = glm::vec3(1.0f);
	if (this->collidesSphere)
	{
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		cout << "collides sphere" << endl;
	}

	glUseProgram(shaderProgram);
	glFrontFace(GL_CCW);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * C * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	GLuint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
	glUniform3f(colorLoc, color.x, color.y, color.z);
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(bbVAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_LINES, bbIndices.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Block::findSquare()
{

}

void Block::makeBlock()
{
	float width = 4.0f;
	// First need to make room for the road, so get the point 1 away from actual vertex
	nn = nn + glm::normalize((np - nn) + (pn - nn)) * width;
	pn = pn + glm::normalize((nn - pn) + (pp - pn)) * width;
	np = np + glm::normalize((nn - np) + (pp - np)) * width;
	pp = pp + glm::normalize((np - pp) + (pn - pp)) * width;

	// Bounding box for housie
	if (type == 3)
	{
		float housieX = 5.0f;
		float housieZ = 7.0f;
		nn = glm::vec3(center.x - housieX, 0.0f, center.z - housieZ);
		pn = glm::vec3(center.x + housieX, 0.0f, center.z - housieZ);
		np = glm::vec3(center.x - housieX, 0.0f, center.z + housieZ);
		pp = glm::vec3(center.x + housieX, 0.0f, center.z + housieZ);
	}

	// Now goes in buffer
	float blockHeight = 0.2f;
	if (type == 0)
		blockHeight = 12.0f;
	if (type == 3)
		blockHeight = 15.0f;
	bufferVertices.push_back(np);
	bufferVertices.push_back(pp);
	bufferVertices.push_back(pp + glm::vec3(0.0f, blockHeight, 0.0f));
	bufferVertices.push_back(np + glm::vec3(0.0f, blockHeight, 0.0f));

	bufferVertices.push_back(nn);
	bufferVertices.push_back(pn);
	bufferVertices.push_back(pn + glm::vec3(0.0f, blockHeight, 0.0f));
	bufferVertices.push_back(nn + glm::vec3(0.0f, blockHeight, 0.0f));

	// Bounding box indices
	unsigned int blah[24] =
	{
		// side
		0, 1, 1, 2, 2, 3, 3, 0,
		// side
		4, 5, 5, 6, 6, 7, 7, 4,
		// connect
		0, 4, 1, 5, 2, 6, 3, 7
	};
	bbIndices.insert(bbIndices.end(), blah, blah + 24);


	// normals
	glm::vec3 nCenter = (np + pp + nn + pn) * 0.25f;
	nCenter.y += blockHeight / 2;
	normals.push_back(np - nCenter);
	normals.push_back(pp - nCenter);
	normals.push_back(pp + glm::vec3(0.0f, blockHeight, 0.0f) - nCenter);
	normals.push_back(np + glm::vec3(0.0f, blockHeight, 0.0f) - nCenter);

	normals.push_back(nn - nCenter);
	normals.push_back(pn - nCenter);
	normals.push_back(pn + glm::vec3(0.0f, blockHeight, 0.0f) - nCenter);
	normals.push_back(nn + glm::vec3(0.0f, blockHeight, 0.0f) - nCenter);

	// tex coords
	textureCoords.push_back(glm::vec2(0.0, 1.0));
	textureCoords.push_back(glm::vec2(1.0, 1.0));
	textureCoords.push_back(glm::vec2(1.0, 1.0));
	textureCoords.push_back(glm::vec2(0.0, 1.0));

	textureCoords.push_back(glm::vec2(0.0, 0.0));
	textureCoords.push_back(glm::vec2(1.0, 0.0));
	textureCoords.push_back(glm::vec2(1.0, 0.0));
	textureCoords.push_back(glm::vec2(0.0, 0.0));
}

void Block::makeBuildings()
{
	float width = glm::min(
		glm::min(glm::length(np - nn), glm::length(np - pp)), 
		glm::min(glm::length(pn - pp), glm::length(pn - nn))
	);
	width = width * 0.4f;

	glm::vec3 inner_nn = nn + glm::normalize((np - nn) + (pn - nn)) * width;
	glm::vec3 inner_pn = pn + glm::normalize((nn - pn) + (pp - pn)) * width;
	glm::vec3 inner_np = np + glm::normalize((nn - np) + (pp - np)) * width;
	glm::vec3 inner_pp = pp + glm::normalize((np - pp) + (pn - pp)) * width;

	// This is really bad

	// bottom
	glm::vec3 left_n = nn + glm::normalize(pn - nn) * width;
	glm::vec3 right_n = pn + glm::normalize(nn - pn) * width;

	// left
	glm::vec3 n_down = nn + glm::normalize(np - nn) * width;
	glm::vec3 n_up = np + glm::normalize(nn - np) * width;

	// right
	glm::vec3 p_down = pn + glm::normalize(pp - pn) * width;
	glm::vec3 p_up = pp + glm::normalize(pn - pp) * width;

	// top
	glm::vec3 left_p = np + glm::normalize(pp - np) * width;
	glm::vec3 right_p = pp + glm::normalize(np - pp) * width;


	// top left
	Window::buildings->addPoints(np, left_p, n_up, inner_np);

	// top mid
	Window::buildings->addPoints(left_p, right_p, inner_np, inner_pp);

	// top right
	Window::buildings->addPoints(right_p, pp, inner_pp, p_up);

	// mid left
	Window::buildings->addPoints(n_up, inner_np, n_down, inner_nn);

	// mid mid
	Window::buildings->addPoints(inner_np, inner_pp, inner_nn, inner_pn);

	// mid right
	Window::buildings->addPoints(inner_pp, p_up, inner_pn, p_down);

	// bottom left
	Window::buildings->addPoints(n_down, inner_nn, nn, left_n);

	// bottom mid
	Window::buildings->addPoints(inner_nn, inner_pn, left_n, right_n);

	// bottom right
	Window::buildings->addPoints(inner_pn, p_down, right_n, pn);
}

int Block::doCollisions(int reflect)
{
	if (type == 1 || type == 2) // Do not do block collisions for park and housie
		return 0;

	glm::vec2 totalReflection = glm::vec2(0.0f);
	int collided = 0;
	if (collision2D(glm::vec2(pn.x, pn.z), glm::vec2(pp.x, pp.z)))
	{
		if (reflect)
		{
			//cout << "collision with right side" << endl;
			totalReflection = doReflection(pp - pn);
		}
		collided++;
	}
	else if (collision2D(glm::vec2(pp.x, pp.z), glm::vec2(np.x, np.z)))
	{
		if (reflect)
		{
			//cout << "collision with top side" << endl;
			totalReflection = doReflection(np - pp);
		}
		collided++;
	}
	else if (collision2D(glm::vec2(nn.x, nn.z), glm::vec2(pn.x, pn.z)))
	{
		if (reflect)
		{
			//cout << "collision with bottom side" << endl;
			totalReflection = doReflection(pn - nn);
		}
		collided++;
	}
	else if (collision2D(glm::vec2(np.x, np.z), glm::vec2(nn.x, nn.z)))
	{
		if (reflect)
		{
			//cout << "collision with left side" << endl;
			totalReflection = doReflection(nn - np);
		}
		collided++;
	}

	if (reflect)
	{
		this->collidesSphere = collided;
		if (collided)
		{
			Window::sphereDir = glm::normalize(glm::vec4(totalReflection.x, 0.0f, totalReflection.y, 0.0f));
		}
	}
	return collided;
}

glm::vec2 Block::doReflection(glm::vec3 sideVector)
{
	glm::vec2 dir = glm::normalize(glm::vec2(Window::sphereDir.x, Window::sphereDir.z));
	glm::vec2 normal = glm::normalize(glm::vec2(-sideVector.z, sideVector.x));
	return glm::reflect(dir, normal);
}

int Block::collision2D(glm::vec2 start, glm::vec2 end)
{
	// Negative determinant = to the right
	glm::vec2 d = end - start;
	//glm::vec2 relSpherePos = glm::vec2(Window::spherePos.x, Window::spherePos.z) - start;
	glm::vec2 flatSphereDir = glm::vec2(Window::sphereDir.x, Window::sphereDir.z);

	//float determinant = d.x * relSpherePos.y - d.y * relSpherePos.x;
	float determinant = d.x * flatSphereDir.y - d.y * flatSphereDir.x;
	//if (determinant > 0.0f)
	if (determinant < 0.0f)
		return 0;

	float r = 1.0f; // this is the sphere radius. hardcoded so whatever.
	// http://stackoverflow.com/questions/1073336/circle-line-segment-collision-detection-algorithm

	glm::vec2 f = start - glm::vec2(Window::spherePos.x, Window::spherePos.z);


	float a = glm::dot(d, d);
	float b = 2 * glm::dot(f, d);
	float c = glm::dot(f, f) - r * r;

	float discriminant = b*b - 4 * a*c;
	if (discriminant < 0)
	{
		// no intersection
		return 0;
	}
	else
	{
		// ray didn't totally miss sphere,
		// so there is a solution to
		// the equation.

		discriminant = sqrt(discriminant);

		// either solution may be on or off the ray so need to test both
		// t1 is always the smaller value, because BOTH discriminant and
		// a are nonnegative.
		float t1 = (-b - discriminant) / (2 * a);
		float t2 = (-b + discriminant) / (2 * a);

		// 3x HIT cases:
		//          -o->             --|-->  |            |  --|->
		// Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit), 

		// 3x MISS cases:
		//       ->  o                     o ->              | -> |
		// FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)

		if (t1 >= 0 && t1 <= 1)
		{
			// t1 is the intersection, and it's closer than t2
			// (since t1 uses -b - discriminant)
			// Impale, Poke
			return 1;
		}

		// here t1 didn't intersect so we are either started
		// inside the sphere or completely past it
		if (t2 >= 0 && t2 <= 1)
		{
			// ExitWound
			return 1;
		}

		// no intn: FallShort, Past, CompletelyInside
		return 0;
	}
}