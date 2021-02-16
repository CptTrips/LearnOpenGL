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


const float radius = 10.f;
glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, -radius);
glm::vec3 cam_fwd = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

float t = 0.f;
float dt = 0.f;

bool first_mouse = true;
const float mouse_sensitivity = 0.001f;
float mouse_x, mouse_y = 0.;
float phi = 0;
float theta = glm::radians(90.);


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

	float cam_speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		cam_speed = 8.f;
	}
	else {
		cam_speed = 4.f;
	}
	float cam_ds = cam_speed * dt;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cam_pos += cam_ds * cam_fwd;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cam_pos += glm::normalize(glm::cross(cam_up, cam_fwd)) * cam_ds;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cam_pos -= glm::normalize(glm::cross(cam_up, cam_fwd)) * cam_ds;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cam_pos -= cam_ds * cam_fwd;
	}
 }

void mouse_callback(GLFWwindow* window, double x, double y)
{
	if (first_mouse)
	{
		mouse_x = x;
		mouse_y = y;
		first_mouse = false;
	}

	float dphi = (x - mouse_x) * mouse_sensitivity;
	float dtheta = (y - mouse_y) * mouse_sensitivity;

	mouse_x = x;
	mouse_y = y;

	phi += dphi;
	phi = glm::mod(phi, 2.f * glm::pi<float>());
	
	theta += dtheta;
	glm::clamp(theta, 0.f, glm::pi<float>());

	//std::cout << '(' << theta << ',' << phi << ')' << std::endl;

	cam_fwd = glm::vec3(-sin(phi) * sin(theta), cos(theta), cos(phi) * sin(theta));
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

	// Capture mouse and process movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	Shader ourShader("VertexShader.glsl", "FragmentShader.glsl");
	Shader light_source_shader("light_source_v.glsl", "light_source_f.glsl");

	// Our cube corners
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	// Bind VAO so we don't have to set up the vertex attributes each time
	// Vertex VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO); // assigns to 2nd param 1st param's worth of indices of Vertex Array Objects
	glBindVertexArray(VAO); // Set's param to the current VAO

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

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	// light VAO
	unsigned int light_vao;
	glGenVertexArrays(1, &light_vao);
	glBindVertexArray(light_vao);

	// Light VBO (same data, VBO)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// Load texture
	int width, height, nrChannels;

	unsigned int diffuse_map;
	glGenTextures(1, &diffuse_map);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_map);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("container2.png", &width, &height, &nrChannels, 0);

	if (data) {
		// GL_RGB for jpg, GL_RGBA for png
		glTexImage2D(GL_TEXTURE_2D, 0, //mipmap level
			GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	unsigned int specular_map;
	glGenTextures(1, &specular_map);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular_map);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//stbi_set_flip_vertically_on_load(true);
	data = stbi_load("container2_specular.png", &width, &height, &nrChannels, 0);

	if (data) {
		// GL_RGB for jpg, GL_RGBA for png
		glTexImage2D(GL_TEXTURE_2D, 0, //mipmap level
			GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	unsigned int emissive_map;
	glGenTextures(1, &emissive_map);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, emissive_map);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//stbi_set_flip_vertically_on_load(true);
	data = stbi_load("matrix.jpg", &width, &height, &nrChannels, 0);

	if (data) {
		// GL_RGB for jpg, GL_RGBA for png
		glTexImage2D(GL_TEXTURE_2D, 0, //mipmap level
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 3D Transformations
	glm::mat4 model = glm::mat4(1.0f); // model transform
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 view = glm::lookAt(cam_pos, cam_fwd + cam_pos, cam_up); // view transform
	
	float fov = 45.0f, aspect_ratio = 4.0f / 3.0f, min_cul = 0.1f, max_cul = 100.0f;
	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, min_cul, max_cul);

	glm::vec3 light_pos_0 = glm::vec3(1.2f, 1.0f, 2.0f);
	glm::vec3 light_pos = light_pos_0;

	glm::vec3 light_color_0 = 0.1f*glm::vec3(1.f, 1.f, 1.f) * (1.f / sqrt(3.f));
	glm::vec3 light_color;
	glm::vec3 color_axis_0 = glm::vec3(-1.f, -1.f, 1.f) * (1.f / sqrt(3.f));
	glm::vec3 color_axis_1 = glm::cross(light_color_0, color_axis_0);

	// Assign uniforms
	ourShader.use();

	ourShader.setMat4("model", &model);
	ourShader.setMat4("view", &view);
	ourShader.setMat4("projection", &projection);

	ourShader.setFloat("material.shininess", 32.0f);
	ourShader.setInt("material.diffuse", 0);
	ourShader.setInt("material.specular", 1);
	ourShader.setInt("material.emissive", 2);


	ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
	ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
	ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	light_source_shader.use();
	light_source_shader.setMat4("projection", &projection);
	light_source_shader.setVec3("light_color", &light_color_0);

	// Background color
	glClearColor(0.1f*light_color_0.x, 0.1f*light_color_0.y, 0.1f*light_color_0.z, 1.0f);

	// Enable z-buffer depth test
	glEnable(GL_DEPTH_TEST);

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Handle input
		processInput(window, &ourShader);

		// Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float new_t = glfwGetTime();
		dt = new_t - t;
		t = new_t;
		
		view = glm::lookAt(cam_pos, cam_pos + cam_fwd, cam_up);

		light_pos = light_pos_0 + (float)sin(3. * t) * glm::vec3(0., 0., 1.);

		light_color = light_color_0 * sqrt(3.f);//+ sin(2.f *t) * color_axis_0 + cos(2.f * t) * color_axis_1;

		glClearColor(0.1f*light_color.x, 0.1f*light_color.y, 0.1f*light_color.z, 1.0f);

		// illuminated cube
		ourShader.use();
		model = glm::mat4(1.0f);
		model = glm::rotate(model, t*glm::radians(50.f),
			glm::vec3(0.5f, 0.1f, 0.0f));
		ourShader.setMat4("model", &model);

		ourShader.setMat4("view", &view);

		ourShader.setVec3("light.position", light_pos.x, light_pos.y, light_pos.z);
		ourShader.setVec3("view_pos", &cam_pos);

		glm::vec3 light_ambient = 0.2f * light_color;
		glm::vec3 light_diffuse = 0.5f * light_color;
		glm::vec3 light_specular = 1.f * light_color;

		ourShader.setVec3("light.ambient", &light_ambient);
		ourShader.setVec3("light.diffuse", &light_diffuse);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// light source cube
		light_source_shader.use();
		model = glm::translate(glm::mat4(1.0f), light_pos);
		model = glm::scale(model, glm::vec3(0.2f));
		light_source_shader.setMat4("model", &model);

		light_source_shader.setMat4("view", &view);

		light_source_shader.setVec3("light_color", &light_color);

		glBindVertexArray(light_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		*/

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

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
