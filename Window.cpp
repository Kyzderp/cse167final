#include "window.h"
#include "Skybox.h"
#include <iostream>
#include "Floor.h"
#include "OBJObject.h"
#include "BumpOBJ.h"
#include "BounceTransform.h"
#include "GLFWStarterProject/include/irrKlang.h"
#include <time.h>       /* time */

using namespace std;

const char* window_title = "GLFW Starter Project";
Skybox* skybox;
GLint Window::solidShader;
GLint shaderProgram;
GLint skyboxShader;
GLint bumpShader;

GLint viewPosLoc;
GLint viewPosbump;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"
#define SKYBOX_VERTEX_SHADER_PATH "../skyboxshader.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH "../skyboxshader.frag"
#define SPHERE_VERTEX_SHADER_PATH "../bumpShader.vert"
#define SPHERE_FRAGMENT_SHADER_PATH "../bumpShader.frag"
#define SOLID_VERTEX_SHADER_PATH "../solidShader.vert"
#define SOLID_FRAGMENT_SHADER_PATH "../solidShader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 5.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

// Sphere view
glm::vec3 sphere_cam_pos(0.0f, 5.0f, 20.0f);		// e  | Position of camera
glm::vec3 sphere_cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 sphere_cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int sphereCamera = 0; // 0 for default, 1 for sphere

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

Sphere* Window::sphere;
glm::vec3 Window::spherePos;
glm::vec4 Window::sphereDir;
float nick = 1.0f;
float speed = 0.0f; // Current speed
float maxSpeed = 0.3f;
float vertSpeed = 0.0f; // Vertical speed, for gravity calcs. Up is positive.

const float friction = 0.95f; // amount to multiply by
const float gravity = 0.01f;

Group* root;
Group* nanners;
QuadPrism* Window::buildings;
Floor *flor;

MatrixTransform* rotation;
BounceTransform* bounce;

OBJObject *banana;
BumpOBJ *orange;
Group* Window::housie;
Group* Window::housies;
OBJObject* house;
OBJObject* roof;

GLFWwindow* windowInstance;

int rmbDown;
int lmbDown;
int click;
int dragging;
double prevX;
double prevY;
glm::vec3 prevPoint;

int paused;

using namespace irrklang;

void Window::initialize_objects()
{
	paused = 0;
	rmbDown = 0;
	lmbDown = 0;
	click = 0;
	prevX = 0;
	prevY = 0;

	srand((unsigned)time(NULL));

	ISoundEngine* se = createIrrKlangDevice();
	//se->play2D("../audio/breakout.mp3", GL_TRUE);

	root = new Group();
	skybox = new Skybox();

	Window::sphere = new Sphere(0);
	buildings = new QuadPrism();

	house = new OBJObject("../objects/Housie3.obj",
		"../objects/concrete.ppm",
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		32.0f);
	house->cull = 0;

	roof = new OBJObject("../objects/Housie3RoofEdit.obj",
		"../objects/grayroof.ppm",
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		32.0f);
	housie = new Group();
	housie->addChild(house);
	housie->addChild(roof);
	housies = new Group();

	flor = new Floor();
	root->addChild(flor);

	spherePos = flor->roadVertices[flor->roadVertices.size() / 2] + glm::vec3(0.0f, 1.0f, 0.0f);
	//spherePos = glm::vec3(0.0f, 1.0f, 0.0f);
	sphereDir = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	sphere_cam_pos = glm::vec3(spherePos + (glm::vec3(sphereDir) * -4.0f) + glm::vec3(0.0f, 2.0f, 0.0f));
	sphere_cam_look_at = glm::vec3(spherePos + glm::vec3(0.0f, 2.0f, 0.0f));

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	skyboxShader = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);
	bumpShader = LoadShaders(SPHERE_VERTEX_SHADER_PATH, SPHERE_FRAGMENT_SHADER_PATH);
	Window::solidShader = LoadShaders(SOLID_VERTEX_SHADER_PATH, SOLID_FRAGMENT_SHADER_PATH);

	viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	viewPosbump = glGetUniformLocation(bumpShader, "viewPos");

	banana = new OBJObject("../objects/BananaTriangle.obj",
		"../objects/BananaMark.ppm",
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		32.0f);
	banana->rotate(glm::vec3(1.0f, 1.0f, 0.0f), 180.0f);

	nanners = new Group();
	//rotation = new MatrixTransform(glm::mat4(1.0f), glm::mat4(1.0f));
	bounce = new BounceTransform(glm::mat4(1.0f), glm::mat4(1.0f));
	//rotation->addChild(banana);
	//bounce->addChild(rotation);
	nanners->addChild(bounce);

	createBananas();

	glUseProgram(bumpShader);

	orange = new BumpOBJ("../objects/orange_lower_poly.obj",
		"../objects/Orange_Color.ppm",
		"../objects/Orange_Normal.ppm",
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		32.0f);

	//banana->move(glm::vec3(0.0f, 1.0f, 0.0f));
	//banana->scale(5.0f);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(Window::sphere);
	delete(skybox);
	delete(root);
	delete(banana);
	delete(orange);
	delete(buildings);
	delete(house);
	delete(housies);
	delete(housie);

	glDeleteProgram(shaderProgram);
	glDeleteProgram(skyboxShader);
	glDeleteProgram(bumpShader);
	glDeleteProgram(Window::solidShader);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	windowInstance = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!windowInstance)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(windowInstance);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(windowInstance, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(windowInstance, width, height);

	return windowInstance;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		Window::P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		if (sphereCamera)
			Window::V = glm::lookAt(sphere_cam_pos, sphere_cam_look_at, sphere_cam_up);
		else
			Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	nanners->update();
}

void Window::display_callback(GLFWwindow* window)
{
	// I'm putting these here because if I put it in key callback it's very slow and weird to control
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		sphereDir = glm::rotate(glm::mat4(1.0f), 0.05f * nick, glm::vec3(0.0f, 1.0f, 0.0f)) * sphereDir;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		sphereDir = glm::rotate(glm::mat4(1.0f), -0.05f * nick, glm::vec3(0.0f, 1.0f, 0.0f)) * sphereDir;
	}

	// jumping is fun
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && spherePos.y < 1.05 && vertSpeed <= 0)
	{
		vertSpeed = 0.3f;
	}
	vertSpeed -= gravity;

	// give forward burst
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		speed = maxSpeed * nick;
	else
	{
		speed *= friction;
		if (speed < 0.001)
			speed = 0;
	}

	glm::vec3 prevSpherePos = spherePos;
	spherePos = spherePos + glm::vec3(sphereDir) * speed + glm::vec3(0.0f, vertSpeed, 0.0f);
	if (spherePos.y < 1.0)
		spherePos.y = 1.0;
	orange->rotate(glm::normalize(glm::vec3(-sphereDir.z, 0.0f, sphereDir.x)), -speed * 20.0f);

	// Collision with walls
	int collided = 0;
	Block* collisionBlock = flor->blocks[0];
	for (int i = 0; i < flor->blocks.size(); i++)
	{
		if (flor->blocks[i]->doCollisions(1))
		{
			// Pretty safe to assume it will only collide with one block at once, because
			// the roads are wider than the orange
			collided = 1;
			collisionBlock = flor->blocks[i];
			break;
		}
	}
	if (collided) // Give an extra boost to get it out of the range
	{
		spherePos = prevSpherePos;
		spherePos = spherePos + glm::vec3(sphereDir) * speed;
		// Extra iteration because sometimes it still isn't out of the range on a small angle
		while (collisionBlock->doCollisions(0))
		{
			spherePos = spherePos + glm::vec3(sphereDir) * 0.01f;
			cout << "iteration ";
		}
	}

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render skybox
	skybox->draw(skyboxShader);

	// Draw orange
	glUseProgram(bumpShader);
	glUniform3f(viewPosbump, cam_pos.x, cam_pos.y, cam_pos.z);

	glUniform3f(glGetUniformLocation(bumpShader, "dirLight.direction"), -0.2f, -1.0f, -0.7f);
	glUniform3f(glGetUniformLocation(bumpShader, "dirLight.ambient"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(bumpShader, "dirLight.diffuse"), 0.65f, 0.65f, 0.65f);
	glUniform3f(glGetUniformLocation(bumpShader, "dirLight.specular"), 0.75f, 0.75f, 0.75f);

	if (sphereCamera)
	{
		// from pov of sphere
		sphere_cam_pos = glm::vec3(spherePos + (glm::vec3(sphereDir) * -4.0f) + glm::vec3(0.0f, 2.0f, 0.0f));
		sphere_cam_look_at = glm::vec3(spherePos + glm::vec3(0.0f, 2.0f, 0.0f));
		Window::V = glm::lookAt(sphere_cam_pos, sphere_cam_look_at, sphere_cam_up);
		glUniform3f(viewPosLoc, sphere_cam_pos.x, sphere_cam_pos.y, sphere_cam_pos.z);
		glUniform3f(viewPosbump, sphere_cam_pos.x, sphere_cam_pos.y, sphere_cam_pos.z);

		orange->draw(bumpShader, glm::translate(glm::mat4(1.0f), spherePos), sphere_cam_pos);
	}
	else
	{
		// default camera
		glUniform3f(viewPosLoc, cam_pos.x, cam_pos.y, cam_pos.z);

		orange->draw(bumpShader, glm::translate(glm::mat4(1.0f), spherePos), cam_pos);
	}

	// now render objects
	glUseProgram(shaderProgram);

	// Set up light(s)
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.direction"), -0.2f, -1.0f, -0.7f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.ambient"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), 0.65f, 0.65f, 0.65f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.specular"), 0.75f, 0.75f, 0.75f);

	root->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	housies->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(1.0f));

	nanners->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(1.0f));
	//banana->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(1.0f));

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::createBananas()
{
	std::vector<glm::vec3> rVerts = flor->roadVertices;
	std::vector<unsigned int> rIndices = flor->roadIndices;

	int numNanners = rand() % 12 + 10;

	for (int i = 0; i < numNanners; i++) {

		int randIdx = rand() % rIndices.size() + 1; // any random index
		float x;
		float z;

		if (randIdx % 2 == 0) { // even index
			glm::vec3 v1 = rVerts[rIndices[randIdx]];
			glm::vec3 v2 = rVerts[rIndices[randIdx + 1]];

			if ((rand() % 10) > 2) {
				x = (v1.x + v2.x) / 2.0f;
				z = (v1.z + v2.z) / 2.0f;
			}
			else {
				x = v1.x;
				z = v1.z;
			}
		}
		else {
			glm::vec3 v1 = rVerts[rIndices[randIdx]];
			x = v1.x;
			z = v1.z;
		}

		MatrixTransform *mt = new MatrixTransform(glm::translate(glm::mat4(1.0f), glm::vec3(x, 2.0f, z)), glm::mat4(1.0f));
		mt->addChild(banana);
		bounce->addChild(mt);
	}
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// V to toggle view
		if (key == GLFW_KEY_V)
		{
			if (sphereCamera)
			{
				sphereCamera = 0;
				Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
			}
			else
			{
				sphereCamera = 1;
				Window::V = glm::lookAt(sphere_cam_pos, sphere_cam_look_at, sphere_cam_up);
			}
		}
	}
}

void Window::cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec3 currPoint = trackBallMapping(xpos, ypos, width, height);

	if (lmbDown && !sphereCamera)
	{
		if (prevPoint.x != 0 || prevPoint.y != 0 || prevPoint.z != 0)
		{
			float angle;
			// Perform horizontal (y-axis) rotation
			angle = (float) (prevX - xpos) / 100.0f;
			cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cam_pos, 1.0f));
			cam_up = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cam_up, 1.0f));
			//Now rotate vertically based on current orientation
			angle = (float) (ypos - prevY) / 100.0f;
			glm::vec3 axis = glm::cross((cam_pos - cam_look_at), cam_up);
			cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, axis) * glm::vec4(cam_pos, 1.0f));
			cam_up = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, axis) * glm::vec4(cam_up, 1.0f));
			// Now update the camera
			Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		}
	}
	prevPoint = currPoint;

	prevX = xpos;
	prevY = ypos;
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		rmbDown = 1;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		rmbDown = 0;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		lmbDown = 1;
		click = 1;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		lmbDown = 0;
		dragging = NULL;
	}
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// scroll up = positive

	if (yoffset > 0)
		cam_pos *= 0.9;
	else
		cam_pos *= 1.1;
	Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}
// To the right is positive X
// Up is positive Y
// Out of screen is positive Z


glm::vec3 Window::trackBallMapping(double xpos, double ypos, double width, double height)
{
	glm::vec3 v = glm::vec3();
	float d;
	v.x = (float)((2 * xpos - width) / width);
	v.y = (float)((height - 2 * ypos) / height);
	v.z = 0.0f;
	d = glm::length(v);
	d = (d < 1.0) ? (float)d : 1.0f;
	v.z = sqrtf(1.001f - d*d);
	return glm::normalize(v);
}

