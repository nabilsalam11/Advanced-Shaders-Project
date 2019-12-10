// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// Include GLFW File
#include <GLFW/glfw3.h>
// Include Soil header file
#include <SOIL/SOIL.h>
// Includes all GLM files
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

// Includes Shader Header
#include "shader.h"
// Includes Camera Header
#include "camera.h"
// Includes Texture Header
#include "texture.h"
// Includes Model Header
#include "model.h"

// Function for a key press for Normal Mapping
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// Function for mouse to move around the model
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// Function for mouse to scroll forward or backwards in model
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Function for model to move
void do_movement();

// Integers for window width and height value
const int WINDOW_WIDTH = 1240, WINDOW_HEIGHT = 1000;
// Window width and height value are divided by 2
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // Key Pressed confirmed
GLfloat deltaTime = 0.0f; // Time passes through each key pressed
GLfloat lastFrame = 0.0f; // Time Frame Ends when program closes
Camera camera(glm::vec3(-4.0f, 2.8f, 14.0f));
glm::vec3 lampPos(0.0, 0.f, 8.0);
bool bNormalMapping = true;
Model objModel; // Rabbit Model
Model objModel2;  //Cyborg Model
Model objModel3;  //SkyBall Model
Model objModel4;  //Helmet Model
Model objModel5;  //Desert Model
 

int main(int argc, char** argv)
{

	if (!glfwInit())	// Initialize GLFW
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// Opening GKFW Windows files for OpenGL version 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Setting up GLFW Window Varaibles for Width and Height
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of normal mapping(Press N to change mapping)", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Gives Context to GLFW Window
	glfwMakeContextCurrent(window);

	// GLFW Key Set Callback on window
	glfwSetKeyCallback(window, key_callback);
	// GLFW Sets Mouse Cursor Movement
	glfwSetCursorPosCallback(window, mouse_move_callback);
	// GLFW Sets Mouse Scolling Movement
	glfwSetScrollCallback(window, mouse_scroll_callback);
	// GLFW Sets Input function towards Window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Applying GLEW files
	glewExperimental = GL_TRUE; // Ensuring that GLEW files work
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	// Giving Window Height and Width
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	//  Text File to load all Models
	std::ifstream modelPath("modelPath.txt");
	if (!modelPath)
	{
		std::cerr << "Error::could not read model path file." << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	std::string modelFilePath;   //Loading Rabbit Model File
	std::getline(modelPath, modelFilePath);
	if (!objModel.loadModel(modelFilePath))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	std::getline(modelPath, modelFilePath); //Loading Cyborg Model File
	if (!objModel2.loadModel(modelFilePath))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}


	std::getline(modelPath, modelFilePath); //Loading Mars Ball Model File
	if (!objModel3.loadModel(modelFilePath))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	std::getline(modelPath, modelFilePath);  //Loading Helmet Model File
	if (!objModel4.loadModel(modelFilePath))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	std::getline(modelPath, modelFilePath); //Loading Desert Model File
	if (!objModel5.loadModel(modelFilePath))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}


	// Loading textures to create Diffuse and Normal Map
	GLuint diffuseMap = TextureHelper::load2DTexture("../../resources/textures/mars.jpg");  //Textures for Mars Ball
	GLuint normalMap = TextureHelper::load2DTexture("../../resources/textures/mars.jpg");

	GLuint diffuseMap2 = TextureHelper::load2DTexture("../../resources/textures/metal.png"); //Textures for Metal
	GLuint normalMap2 = TextureHelper::load2DTexture("../../resources/textures/metal2.png");

	GLuint diffuseMap3 = TextureHelper::load2DTexture("../../resources/textures/sand.jpg"); //Textures for Sand
	GLuint normalMap3 = TextureHelper::load2DTexture("../../resources/textures/sand.jpg");

	// Including the fragment and vertex shaders
	Shader shader("scene.vertex", "scene.frag");

	glEnable(GL_DEPTH_TEST);
	// When the GLFW Window Closes
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // GLFW Polls Function to record Time
		do_movement(); // Movement stays the same for Rabbit

		// Sets Colors for the Rabbit
		glClearColor(0.70f, 0.90f, 0.40f, 1.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // Setting uo Window Height and Width for the program
		glm::mat4 view = camera.getViewMatrix(); // Setting up Camera Matrix

		// Initialize Shader

		shader.use();
		// Giving the light ambient, diffuse, specular and position properties
		GLint lightAmbientLoc = glGetUniformLocation(shader.programId, "light.ambient");
		GLint lightDiffuseLoc = glGetUniformLocation(shader.programId, "light.diffuse");
		GLint lightSpecularLoc = glGetUniformLocation(shader.programId, "light.specular");
		GLint lightPosLoc = glGetUniformLocation(shader.programId, "light.position");
		glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z);
		// Setting up the view position
		GLint viewPosLoc = glGetUniformLocation(shader.programId, "viewPos");
		glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);
		// Setting up the Light Position
		lightPosLoc = glGetUniformLocation(shader.programId, "lightPos");
		glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z);
		// Setting up the Projection
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		////////////////////////Rabbit Model///////////////////////////////////////////
		glm::vec3 m_ModelPos(-3.0, 0.f, 8.0);
		glm::mat4 model;
		model = glm::translate(model, m_ModelPos);
		// Builds the Rabbit Model
		//model = glm::rotate(model, (GLfloat)glfwGetTime() * -2, glm::normalize(glm::vec3(1.0, 10.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(shader.programId, "normalMapping"), bNormalMapping);
		// Draws Normal Mapping Shader
		objModel.draw(shader);

		std::cout << "using normal mapping " << (bNormalMapping ? "true" : "false") << std::endl;

		/////////////////////////////////Cyborg Model//////////////////////////////////////////
		glm::vec3 m_ModelPosition(0.0, 0.f, 4.0);
		glm::mat4 model2;
		model2 = glm::translate(model2, m_ModelPosition);
		// Builds the Cyborg Model
		//model2 = glm::rotate(model2, (GLfloat)glfwGetTime() * -2, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model2));
		glUniform1i(glGetUniformLocation(shader.programId, "normalMapping"), bNormalMapping);
		// Draws Normal Mapping Shader
		objModel2.draw(shader);

		//////////////////////////////////Mars Ball Model////////////////////////////////////
		glm::vec3 m_ModelPosition2(5.0, 4.f, 0.0);
		glm::mat4 model3;
		model3 = glm::translate(model3, m_ModelPosition2);
		// Builds the Moon Model
		model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * -2, glm::normalize(glm::vec3(10.0, 1.0, 10.0)));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model3));
		glUniform1i(glGetUniformLocation(shader.programId, "normalMapping"), bNormalMapping);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glUniform1i(glGetUniformLocation(shader.programId, "diffuseMap"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glUniform1i(glGetUniformLocation(shader.programId, "normalMap"), 1);

		//Draws Normal Mapping Shader
		objModel3.draw(shader);


		//////////////////////////////////Helmet Model////////////////////////////////////
		glm::vec3 m_ModelPosition3(2.0, 0.f, 7.0);
		glm::mat4 model4;
		model4 = glm::translate(model4, m_ModelPosition3);
		// Builds the Helmet Model
		//model4 = glm::rotate(model4, (GLfloat)glfwGetTime() * -2, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model4));
		glUniform1i(glGetUniformLocation(shader.programId, "normalMapping"), bNormalMapping);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap2);
		glUniform1i(glGetUniformLocation(shader.programId, "diffuseMap"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap2);
		glUniform1i(glGetUniformLocation(shader.programId, "normalMap"), 1);

		//Draws Normal Mapping Shader
		objModel4.draw(shader);

		//////////////////////////////////Desert Model////////////////////////////////////
		glm::vec3 m_ModelPosition4(0.0, -1.f, 0.0);
		glm::mat4 model5;
		model5 = glm::translate(model4, m_ModelPosition4);
		// Builds the Desert Model
		//model4 = glm::rotate(model4, (GLfloat)glfwGetTime() * -2, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model5));
		glUniform1i(glGetUniformLocation(shader.programId, "normalMapping"), bNormalMapping);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap3);
		glUniform1i(glGetUniformLocation(shader.programId, "diffuseMap"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap3);
		glUniform1i(glGetUniformLocation(shader.programId, "normalMap"), 1);

		//Draws Normal Mapping Shader
		objModel5.draw(shader);

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // Normal Mapping swapped when pressed N
	}
	// End Program
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keyPressedStatus[key] = true;
		else if (action == GLFW_RELEASE)
			keyPressedStatus[key] = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // Window will close down when ESC button is pressed
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		bNormalMapping = !bNormalMapping;
	}
}
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove) // When the mouse moves
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMove(xoffset, yoffset);
}
// Scrolling Mouse function
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.handleMouseScroll(yoffset);
}
// Moving the camera
void do_movement()
{

	if (keyPressedStatus[GLFW_KEY_W])
		camera.handleKeyPress(FORWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_S])
		camera.handleKeyPress(BACKWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_A])
		camera.handleKeyPress(LEFT, deltaTime);
	if (keyPressedStatus[GLFW_KEY_D])
		camera.handleKeyPress(RIGHT, deltaTime);
}

