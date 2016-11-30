#include "window.h"
#include "Skybox.h"
#include <iostream>
#include "Humanoid.h"
#include "Bezier.h"

using namespace std;

const char* window_title = "GLFW Starter Project";
Skybox* skybox;
GLint shaderProgram;
GLint skyboxShader;
GLint pointShader;
GLint sphereShader;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"
#define SKYBOX_VERTEX_SHADER_PATH "../skyboxshader.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH "../skyboxshader.frag"
#define POINT_VERTEX_SHADER_PATH "../pointshader.vert"
#define POINT_FRAGMENT_SHADER_PATH "../pointshader.frag"
#define SPHERE_VERTEX_SHADER_PATH "../sphereShader.vert"
#define SPHERE_FRAGMENT_SHADER_PATH "../sphereShader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 5.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

Sphere* Window::sphere;
Group* Window::track;
Points* Window::anchors;
Points* Window::handles;
Lines* Window::lines;

float Window::velocity;
float Window::time;

glm::vec3 Window::highestPoint;
float Window::highestTime;
glm::vec3 spherePos;

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
	velocity = 0;
	time = 0;
	prevPoint = glm::vec3();
	highestPoint = glm::vec3(0.0f);
	highestTime = 0.0f;
	time = 0.0f;

	skybox = new Skybox();
	track = new Group();
	Window::sphere = new Sphere(0);

	// Make the curves, stick them in a circle
	Bezier* first = new Bezier(NULL, glm::vec3(5.0f, 3.0f, 0.0f), 0);
	spherePos = glm::vec3(5.0f, 3.0f, 0.0f);
	Bezier* prev = first;
	track->addChild(first);
	int i = 1;
	for (int angle = 36; angle < 360; angle += 36)
	{
		float x = cos(angle / 180.0f * glm::pi<float>()) * 5.0f;
		float z = sin(angle / 180.0f * glm::pi<float>()) * 5.0f;
		Bezier* curr = new Bezier(prev, glm::vec3(x, 0.0f, z), i);
		curr->p0 = prev->p3;
		curr->update();
		track->addChild(curr);
		i++;

		prev = curr;
	}
	first->setPrev(prev);

	// Make the control points
	Window::anchors = new Points(1);
	Window::handles = new Points(0);
	Window::lines = new Lines();

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	skyboxShader = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);
	pointShader = LoadShaders(POINT_VERTEX_SHADER_PATH, POINT_FRAGMENT_SHADER_PATH);
	sphereShader = LoadShaders(SPHERE_VERTEX_SHADER_PATH, SPHERE_FRAGMENT_SHADER_PATH);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(Window::sphere);
	delete(skybox);
	delete(track);
	delete(anchors);
	delete(handles);
	delete(lines);

	glDeleteProgram(shaderProgram);
	glDeleteProgram(skyboxShader);
	glDeleteProgram(pointShader);
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
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
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
		Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
}

void Window::display_callback(GLFWwindow* window)
{
	// Calculate position of sphere
	if (!paused)
	{
		float h = highestPoint.y - spherePos.y;
		if (h > 0)
			velocity = glm::sqrt(0.000001f * h) + 0.00001f;
		else
			velocity = 0.00001f;

		time = time + velocity;
	}
	while (time > 10)
		time -= 10;
	float t = time - (int)time;
	int index = (int)(time - t);
	spherePos = ((Bezier*)track->children[index])->makePoint(t);
	glm::mat4 C = glm::translate(glm::mat4(1.0f), spherePos);

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render stuff
	skybox->draw(skyboxShader);
	track->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(0.0f));
	lines->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	anchors->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	handles->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	sphere->draw(sphereShader, C, cam_pos);

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

		// Pause
		if (key == GLFW_KEY_P)
		{
			if (paused != 0)
				paused = 0;
			else
				paused = 1;
		}

		// Reset
		if (key == GLFW_KEY_R)
		{
			time = highestTime;
		}
	}
}

void Window::cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec3 currPoint = trackBallMapping(xpos, ypos, width, height);

	if (click)
	{
		click = 0;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		anchors->drawSelect(pointShader);
		handles->drawSelect(pointShader);

		unsigned char result[4];
		glReadPixels((int)xpos, height - (int)ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &result);

		dragging = result[0];
		


		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render stuff
		skybox->draw(skyboxShader);
		track->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(0.0f));
		lines->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		anchors->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		handles->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	}

	if (lmbDown)
	{
		if (dragging != NULL && dragging > 0)
		{
			// Drag a control point
			glm::vec4 move = glm::vec4((xpos - prevX), (prevY - ypos), 0.0f, 0.0f);
			move = glm::normalize(move) / 10.0f;
			move = glm::inverse(Window::P) * move;
			move = glm::inverse(Window::V) * move;

			// Find which point we're actually dragging
			if (dragging <= 80)
			{
				// Anchors
				Bezier* bezier = (Bezier*)track->children[dragging / 8 - 1];
				bezier->moveAnchor(glm::vec3(move));
			}
			else
			{
				// Handles
				int index = (dragging - 88) / 16;
				cout << index << endl;
				Bezier* bezier = (Bezier*)track->children[index];
				if ((dragging - 88) - index * 16 == 8) //even = first
				{
					cout << "second" << endl;
					bezier->move2(glm::vec3(move));
				}
				else
					bezier->move1(glm::vec3(move));
			}
		}
		else if (prevPoint.x != 0 || prevPoint.y != 0 || prevPoint.z != 0)
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

