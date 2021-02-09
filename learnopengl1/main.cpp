#include <iostream>
#include <string>
#include <fstream>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h" // image loading library
#include "Shader.h"

float mix_param;
const float mix_increment = 1e-3f;

// Function to call when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Function to process all key inputs
void processInput(GLFWwindow* window, Shader* s)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	glGetUniformfv(s->ID, glGetUniformLocation(s->ID, "mix_param"), &mix_param);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{ 
		mix_param += mix_increment;
		if (mix_param > 1.0f)
			mix_param = 1.0f;
		std::cout << "up" << mix_param << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{ 
		std::cout << "down" << mix_param << std::endl;
		mix_param -= mix_increment;
		if (mix_param < 0.0f)
			mix_param = 0.0f;
		//s->setFloat("mix_param", mix_param);
	}
}

int main(int argc, char** argv)
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Specify the viewport (OpenGL area within the GLFW window)
	glViewport(0, 0, 800, 600);

	// Specify the resize callback function so viewport adapts on window resize

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader ourShader("VertexShader.glsl", "FragmentShader.glsl");

	// Our cube corners
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	/*
	// Rectangle vertices split into triangles
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};*/


	// Bind VAO so we don't have to set up the vertex attributes each time
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	// Create element buffer object to parse indices and vertices into triangles
	/*
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	*/

	// Create a vertex buffer object for sending data to GPU memory
	unsigned int VBO; // Buffer ID
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Tell OpenGL how to interpret the vertex array 
	// for vertex attribute
	// (vertex attribute is at location 0, size 3 * sizeof(type), type=float,
	// not normalised, stride 6 floats, first value at start of buffer)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*
	// now for color attribute
	// (vertex attribute is at location 1, size 3 * sizeof(type), type=float,
	// not normalised, stride 6 floats, first value 3 floats in)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
		(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	*/

	// texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float),
		(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	// Load texture
	int width, height, nrChannels;
	int width1, height1, nrChannels1;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	unsigned char* data1 = stbi_load("awesomeface.png", &width1, &height1, &nrChannels1, 0);

	unsigned int textures[2];
	glGenTextures(2, textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, //mipmap level
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (data1) {
		glTexImage2D(GL_TEXTURE_2D, 0, //mipmap level
			GL_RGB, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	// Background color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 3D Transformations
	glm::mat4 model = glm::mat4(1.0f); // model transform
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, -3.0f);
	glm::mat4 view = glm::mat4(1.0f); // view transform
	view = glm::translate(view, cam_pos);
	
	float fov = 45.0f, aspect_ratio = 4.0f / 3.0f, min_cul = 0.1f, max_cul = 100.0f;
	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, min_cul, max_cul);

	// Assign uniforms
	ourShader.use();
	glUniform1i(glGetUniformLocation(ourShader.ID, "ourTexture1"), 0);
	ourShader.setInt("ourTexture2", 1); // we wrote a class to do the above

	ourShader.setFloat("mix_param", 0.2f);

	ourShader.setMat4("model", &model);
	ourShader.setMat4("view", &view);
	ourShader.setMat4("projection", &projection);

	// Enable z-buffer depth test
	glEnable(GL_DEPTH_TEST);

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Handle input
		processInput(window, &ourShader);

		// Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLoc = glGetUniformLocation(shaderProgram, "ourColor");
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, (float)(glfwGetTime()*0.1), glm::vec3(0.0, 0.0, 1.0));
		ourShader.setMat4("transform", &trans);
		*/

		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.f),
				glm::vec3(0.5f, 0.1f, 0.0f));
			ourShader.setMat4("model", &model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		ourShader.setFloat("mix_param", mix_param);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);

		glBindVertexArray(VAO);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean-up
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteProgram(shaderProgram); // should probably implement destructor in Shader
	glfwTerminate();
	return 0;

}
