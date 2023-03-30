#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// for printing out vectors
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

//#include <shader.h>
#include "model.h"
#include <camera.h>

#include <iostream>

#include "ball.h"
//window config
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// camera
/*
vec3(0.000061, -0.999848, -0.017452) Front
vec3(0.000568, 16.851688, 1.219149) Position
vec3(0.999994, -0.000000, 0.003490) Right
vec3(0.003490, 0.017452, -0.999842) Up
- 89
*/
//Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera camera(glm::vec3(0.0, 15.0,1.0), glm::vec3(0.0, 0.0, -1.0), -90.0f, -88.0f); // setting camera to desired position
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Mouse
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

//scroll
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//ball position
glm::vec3 position(0.28f, 0.3f, 5.0f);
float ball_scale = 0.13f;

//Pause button
bool pause = true;

int main(void)
{
	// initialize window stuff
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// end of window stuff
	// tell stb_image.h to flip loaded texture on y-axis
	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	// First, process shader
	Shader ourShader("vertex_shader.glsl", "fragment_shader.glsl");

	// load models
	Model mazeModel(".\\maze\\maze.obj");
	Model ballModel(".\\ball\\hokmaball.obj");
	//Model mazeModel(".\\backpack\\backpack.obj");

	// get some juicy info
	//for (int i = 0; i < ballModel.meshes[0].vertices.size();i++)
	//	std::cout << ballModel.meshes[0].vertices.size() << std::endl;

	//for(int i=0; i<ballModel.meshes[0].indices.size();i++)
	//	std::cout << ballModel.meshes[0].indices[i]<< std::endl;



	//radius calculation for stuff
	// Scale stuff: scale A * radius B = scale B * radius A
	//first get a triangle (multiply to scale!)
	glm::vec3 pos1 = ballModel.meshes[0].vertices[ballModel.meshes[0].indices[0]].Position;
	glm::vec3 pos2 = ballModel.meshes[0].vertices[ballModel.meshes[0].indices[1]].Position;
	glm::vec3 pos3 = ballModel.meshes[0].vertices[ballModel.meshes[0].indices[2]].Position;
	//coordinate of centroid of triangle with some theorm https://www.physicsforums.com/threads/center-mass-of-triangle-in-3-d-space.527709/
	glm::vec3 triangle_center = glm::vec3((pos1.x + pos2.x + pos3.x), (pos1.y + pos2.y + pos3.y), (pos1.z + pos2.z + pos3.z))/3.0f;
	// don't forget to also multiply the scale to position!
	float radius = glm::abs(glm::distance(position, triangle_center));
	// scale function radius B = scale B (should be ball_scale) * radius A (radius) / scale A(1). Also remember that area needs 2 multiplies 
	radius = radius * ball_scale * ball_scale ;


	// Ball controller
	Ball ballControl(position, direction::BALL_UP, radius+0.1f);



	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input button process
		processInput(window);

		//////////// Program starts rendering here
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// clear color AND depth
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//use shader
		ourShader.use();

		// camera view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection); 
		ourShader.setMat4("view", view);

		//render the loaded model
		//maze
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		mazeModel.Draw(ourShader);
		//ball
		model = glm::mat4(1.0f);
		if (!pause)
			model = ballControl.moveforward(model);
		model = glm::scale(model, glm::vec3(ball_scale, ball_scale, ball_scale));	// ball is smol
		ourShader.setMat4("model", model);



		//collision detection
		// different states: 
		// state 1: there is space on the right. Turn right.
		// state 2: There is space only on the left. Turn right 3 times (turn left)
		// state 3: there is no space in the front. There is no space on the left. There is no space on the right. Turn back.
		// default: no space on right but space in front: forward
		// state 1
		if (!mazeModel.clash(ballControl.position, ballControl.center_right_endpoint()) && !mazeModel.clash(ballControl.position, ballControl.bottom_right_endpoint()) && !mazeModel.clash(ballControl.position, ballControl.top_right_endpoint()))
		{
			std::cout << "turn right" << std::endl;
			ballControl.rotate_right();
			ballControl.moveforward(model,radius);
		}
		// state 2
		else if (mazeModel.clash(ballControl.position, ballControl.center_right_endpoint()) && mazeModel.clash(ballControl.position, ballControl.forward_endpoint()) && !mazeModel.clash(ballControl.position, ballControl.center_left_endpoint()) && !mazeModel.clash(ballControl.position, ballControl.top_left_endpoint()))
		{
			std::cout << "turn left" << std::endl;
			ballControl.rotate_right();
			ballControl.rotate_right();
			ballControl.rotate_right();
			ballControl.moveforward(model, radius);
		}
		// state 3
		else if (mazeModel.clash(ballControl.position, ballControl.center_right_endpoint()) && mazeModel.clash(ballControl.position, ballControl.forward_endpoint()))
		{
			std::cout << "turn backward" << std::endl;
			ballControl.rotate_right();
			ballControl.rotate_right();
		}

		//// just check forward
		//if ( mazeModel.clash(ballControl.position, ballControl.forward_endpoint()))
		//{
		//	ballControl.rotate_right();
		//	ballControl.rotate_right();
		//	already_rotate = true;
		//}



		/// Here me doing 
		ballModel.Draw(ourShader);

		//////////// Program ends rendering here
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		pause=false;
	/*
	// This is for testing find right position for camera
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		std::cout << glm::to_string(camera.Front) << std::endl;
		std::cout << glm::to_string(camera.Position) << std::endl;
		std::cout << glm::to_string(camera.Right) << std::endl;
		std::cout << glm::to_string(camera.Up) << std::endl;
		std::cout << camera.Pitch << std::endl;

	}*/
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	//camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}