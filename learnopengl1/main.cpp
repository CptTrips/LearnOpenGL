#include <iostream>
#include <string>
#include <fstream>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#include "stb_image.h" // image loading library

#include "Shader.h"
#include "Model.h"
#include "Mesh.h"


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
		//cout << glm::to_string(cam_pos) << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cam_pos += glm::normalize(glm::cross(cam_up, cam_fwd)) * cam_ds;
		//cout << glm::to_string(cam_pos) << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cam_pos -= glm::normalize(glm::cross(cam_up, cam_fwd)) * cam_ds;
		//cout << glm::to_string(cam_pos) << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cam_pos -= cam_ds * cam_fwd;
		//cout << glm::to_string(cam_pos) << endl;
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

	// Load shaders
	Shader ourShader("VertexShader.glsl", "FragmentShader.glsl");
	Shader light_source_shader("light_source_v.glsl", "light_source_f.glsl");

	// Load model
	string guitar_pack_path = "C:\\Users\\sodai\\Documents\\projects\\TripsLearnOpenGL\\learnopengl1\\models\\backpack.obj";
	Model guitar_pack = Model(guitar_pack_path.c_str());

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 3D Transformations
	glm::mat4 model = glm::mat4(1.0f); // model transform

	glm::mat4 view = glm::lookAt(cam_pos, cam_fwd + cam_pos, cam_up); // view transform
	
	float fov = 45.0f, aspect_ratio = 4.0f / 3.0f, min_cul = 0.1f, max_cul = 100.0f;
	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, min_cul, max_cul);
	
	glm::vec3 light_positions[] = {
		glm::vec3(1.f, -0.3f, 10.f),
		100.f*glm::vec3(2.3f, -3.3f, -4.0f),
		100.f*glm::vec3(-4.0f, 2.0f, -12.0f),
		100.f*glm::vec3(0.0f, 0.0f, -3.0f)
	};

	glm::vec3 light_color_0 = glm::vec3(1.f, 1.f, 1.f) * (1.f / sqrt(3.f));
	glm::vec3 light_color = glm::vec3(1.f, .2f, 0.f);;
	// For a light which changes color at constant lightness
	glm::vec3 color_axis_0 = glm::vec3(-1.f, -1.f, 1.f) * (1.f / sqrt(3.f));
	glm::vec3 color_axis_1 = glm::cross(light_color_0, color_axis_0);

	constexpr float steradians = 4.f * glm::pi<float>();

	glm::vec3 sunlight_color = glm::vec3(1.f, 1.f, 0.98f);
	glm::vec3 sunlight_ambient = 0.15f*glm::vec3(0.5f, .8f, 0.92f);
	glm::vec3 sunlight_diffuse = (1.f/steradians) * sunlight_color;
	glm::vec3 sunlight_specular = sunlight_color;
	glm::vec3 sunlight_dir = glm::normalize(glm::vec3(0.5f, 1.f, 0.5f));

	glm::vec4 flashlight_offset_viewspace = glm::vec4(0.f);// -0.1f, 0.f, 0.1f, 1.f);;
	glm::vec3 flashlight_offset;

	// Assign uniforms
	ourShader.use();

	ourShader.setMat4("model", &model);
	ourShader.setMat4("view", &view);
	ourShader.setMat4("projection", &projection);

	int point_light_count = 4;
	std::string point_light_str = "point_lights[i]";
	int plstr_i_idx = point_light_str.find('[') + 1;
	for (int i = 0; i < point_light_count; i++) {

		point_light_str[plstr_i_idx] = '0' + i;

		glm::vec3 light_color_diffuse = 1.f / steradians * light_color;

		ourShader.setVec3(point_light_str + ".ambient", 0.f, 0.f, 0.f);
		ourShader.setVec3(point_light_str + ".diffuse", &light_color_diffuse);
		ourShader.setVec3(point_light_str + ".specular", &light_color);
		ourShader.setFloat(point_light_str + ".intensity", 40.f);
		ourShader.setVec3(point_light_str + ".position", &light_positions[i]);
	}

	ourShader.setVec3("planar_light.direction", &sunlight_dir);
	ourShader.setVec3("planar_light.ambient", &sunlight_ambient);
	ourShader.setVec3("planar_light.diffuse", &sunlight_diffuse);
	ourShader.setVec3("planar_light.specular", &sunlight_specular);
	ourShader.setFloat("planar_light.intensity", 10.f);

	ourShader.setVec3("flashlight.color", 1.0f, 1.0f, 1.0f);
	ourShader.setFloat("flashlight.intensity", 2.f);

	light_source_shader.use();
	light_source_shader.setMat4("projection", &projection);
	light_source_shader.setVec3("light_color", &light_color_0);

	// Background color
	glClearColor(0.1f*light_color_0.x, 0.1f*light_color_0.y, 0.1f*light_color_0.z, 1.0f);

	// Enable z-buffer depth test
	glEnable(GL_DEPTH_TEST);

	// Enable stencil buffer
	glEnable(GL_STENCIL_TEST);

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Handle input
		processInput(window, &ourShader);

		// Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		float new_t = glfwGetTime();
		dt = new_t - t;
		t = new_t;

		cout << "frametime " << dt << endl;

		// Changing light color
		//light_color = light_color_0 + sin(2.f *t) * color_axis_0 + cos(2.f * t) * color_axis_1;

		/*
		glm::vec3 light_ambient = 0.2f * light_color;
		glm::vec3 light_diffuse = 0.5f * light_color;
		glm::vec3 light_specular = 1.f * light_color;

		ourShader.setVec3("point_light.ambient", &light_ambient);
		ourShader.setVec3("point_light.diffuse", &light_diffuse);
		*/
		//light_pos = light_pos_0 + (float)sin(3. * t) * glm::vec3(0., 0., 1.);

		glClearColor(0.01f, 0.02f, 0.f, 1.0f);

		// Co-ordinate systems
		view = glm::lookAt(cam_pos, cam_pos + cam_fwd, cam_up);
		ourShader.use();
		model = glm::mat4(1.0f);
		ourShader.setMat4("model", &model);
		ourShader.setMat4("view", &view);
		ourShader.setVec3("view_pos", &cam_pos);

		/*
		for (int i = 0; i < point_light_count; i++) {

			point_light_str[plstr_i_idx] = '0' + i;

			// light source
			ourShader.use();
			ourShader.setVec3(point_light_str + ".position", &light_positions[i]);
		}
		*/

		// Flashlight
		ourShader.use();
		flashlight_offset = glm::vec3(glm::inverse(view) * flashlight_offset_viewspace);
		ourShader.setVec3("flashlight.offset", &flashlight_offset);
		ourShader.setVec3("flashlight.direction", cam_fwd.x, cam_fwd.y, cam_fwd.z);

		guitar_pack.draw(ourShader);

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
