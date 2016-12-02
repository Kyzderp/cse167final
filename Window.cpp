#include "window.h"
#include "Skybox.h"
#include <iostream>
#include "Floor.h"
#include "OBJObject.h"

using namespace std;

const char* window_title = "GLFW Starter Project";
Skybox* skybox;
GLint shaderProgram;
GLint skyboxShader;
GLint sphereShader;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"
#define SKYBOX_VERTEX_SHADER_PATH "../skyboxshader.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH "../skyboxshader.frag"
#define SPHERE_VERTEX_SHADER_PATH "../sphereShader.vert"
#define SPHERE_FRAGMENT_SHADER_PATH "../sphereShader.frag"

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
glm::vec3 spherePos;
glm::vec4 sphereDir;
float speed = 0.05f;

Group* root;

OBJObject *banana;

GLFWwindow* windowInstance;

int rmbDown;
int lmbDown;
int click;
int dragging;
double prevX;
double prevY;
glm::vec3 prevPoint;

int paused;

void Window::initialize_objects()
{
	paused = 0;
	rmbDown = 0;
	lmbDown = 0;
	click = 0;
	prevX = 0;
	prevY = 0;

	root = new Group();
	skybox = new Skybox();

	Window::sphere = new Sphere(0);

	Floor* floor = new Floor();
	root->addChild(floor);

	spherePos = glm::vec3(0.0f, 1.0f, 0.0f);
	sphereDir = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	sphere_cam_pos = glm::vec3(spherePos + (glm::vec3(sphereDir) * -4.0f) + glm::vec3(0.0f, 2.0f, 0.0f));
	sphere_cam_look_at = glm::vec3(spherePos + glm::vec3(0.0f, 2.0f, 0.0f));

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	skyboxShader = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);
	sphereShader = LoadShaders(SPHERE_VERTEX_SHADER_PATH, SPHERE_FRAGMENT_SHADER_PATH);

	banana = new OBJObject("../objects/BananaTriangle.obj",
		"../objects/BananaMark.ppm",
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		32.0f);

	banana->rotate(glm::vec3(0, 0, 1.0f), 180.0f);
	banana->move(glm::vec3(0, 5.0f, 0));
	banana->scale(10.0f);

}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(Window::sphere);
	delete(skybox);
	delete(root);

	glDeleteProgram(shaderProgram);
	glDeleteProgram(skyboxShader);
	glDeleteProgram(sphereShader);
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
}

void Window::display_callback(GLFWwindow* window)
{
	// I'm putting these here because if I put it in key callback it's very slow and weird to control
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		sphereDir = glm::rotate(glm::mat4(1.0f), 5.0f / 60.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * sphereDir;
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		sphereDir = glm::rotate(glm::mat4(1.0f), -5.0f / 60.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * sphereDir;

	// move
	spherePos = spherePos + glm::vec3(sphereDir) * speed;

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render stuff
	skybox->draw(skyboxShader);
	
	if (sphereCamera)
	{
		// from pov of sphere
		sphere_cam_pos = glm::vec3(spherePos + (glm::vec3(sphereDir) * -4.0f) + glm::vec3(0.0f, 2.0f, 0.0f));
		sphere_cam_look_at = glm::vec3(spherePos + glm::vec3(0.0f, 2.0f, 0.0f));
		Window::V = glm::lookAt(sphere_cam_pos, sphere_cam_look_at, sphere_cam_up);
		sphere->draw(sphereShader, glm::translate(glm::mat4(1.0f), spherePos), sphere_cam_pos);
	}
	else
	{
		// default camera
		sphere->draw(sphereShader, glm::translate(glm::mat4(1.0f), spherePos), cam_pos);
	}
	root->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	banana->draw(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
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

