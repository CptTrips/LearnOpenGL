#include <iostream>
#include <string>
#include <fstream>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include "stb_image.h" // image loading library
#include "Shader.h"

// Function to call when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Function to process all key inputs
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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

	// Our rectangle corners
	float vertices[] = {
		// vertices			// colors			// texture coords
		0.5f, 0.5f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,	1.0f, 1.0f, 0.0f,   0.0f, 1.0f
	};
	// Rectangle vertices split into triangles
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	// Vertices for two triangles
	/*
	float vertices[] = {
		// positions		colors
		-0.0f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f
	};

	float texCoords[] = {
		0.0f, 0.0f, // a texture co-ordinate ([0,1]x[0,1]) for each vertex
		1.0f, 0.0f,
		0.5f, 1.0f
	};
	*/

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
	if (data1) {
		glTexImage2D(GL_TEXTURE_2D, 0, //mipmap level
			GL_RGB, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	// Bind VAO so we don't have to set up the vertex attributes each time
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	// Create element buffer object to parse indices and vertices into triangles
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Create a vertex buffer object for sending data to GPU memory
	unsigned int VBO; // Buffer ID
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Tell OpenGL how to interpret the vertex array 
	// for vertex attribute
	// (vertex attribute is at location 0, size 3 * sizeof(type), type=float,
	// not normalised, stride 6 floats, first value at start of buffer)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// now for color attribute
	// (vertex attribute is at location 1, size 3 * sizeof(type), type=float,
	// not normalised, stride 6 floats, first value 3 floats in)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
		(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float),
		(void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	// Background color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Assign texture pointer uniforms
	ourShader.use();
	glUniform1i(glGetUniformLocation(ourShader.ID, "ourTexture1"), 0);
	ourShader.setInt("ourTexture2", 1); // we wrote a class to do the above

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Handle input
		processInput(window);

		// Rendering
		glClear(GL_COLOR_BUFFER_BIT);

		/*
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLoc = glGetUniformLocation(shaderProgram, "ourColor");
		*/

		// Uniform offset
		unsigned int uniform_loc = glGetUniformLocation(ourShader.ID, "offset");
		glUniform3f(uniform_loc, 0.0f, 0.0f, 0.0f);
		ourShader.use();
		//glUniform4f(vertexColorLoc, 0.0f, greenValue, 0.0f, 1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
