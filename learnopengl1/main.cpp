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

constexpr float steradians = 4.f * glm::pi<float>();

const float radius = 10.f;
glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, radius);
glm::vec3 cam_fwd = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

float t = 0.f;
float dt = 0.f;

bool first_mouse = true;
const float mouse_sensitivity = 0.001f;
float mouse_x, mouse_y = 0.;
float phi = glm::acos(cam_fwd.z);
float theta = glm::acos(cam_fwd.y);


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
	theta = glm::clamp(theta, 1e-3f, glm::pi<float>() - 1e-3f);

	//std::cout << '(' << theta << ',' << phi << ')' << std::endl;

	cam_fwd = glm::vec3(-sin(phi) * sin(theta), cos(theta), cos(phi) * sin(theta));
}


struct PointLight
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 position;
	float intensity;
};

struct PlanarLight
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 direction;
	float intensity;
};

struct FlashLight
{
	glm::vec3 color;
	glm::vec3 offset;
	float intensity;
};

void pass_lights_to_shader(Shader &s, PointLight p, PlanarLight pl, FlashLight f)
{
	s.use();

	std::string point_light_str = "point_lights[i]";
	int plstr_i_idx = point_light_str.find('[') + 1;
	point_light_str[plstr_i_idx] = '0';// + i;

	s.setVec3(point_light_str + ".ambient", &p.ambient);
	s.setVec3(point_light_str + ".diffuse", &p.diffuse);
	s.setVec3(point_light_str + ".specular", &p.specular);
	s.setFloat(point_light_str + ".intensity", p.intensity);
	s.setVec3(point_light_str + ".position", &p.position);

	s.setVec3("planar_light.direction", &pl.direction);
	s.setVec3("planar_light.ambient", &pl.ambient);
	s.setVec3("planar_light.diffuse", &pl.diffuse);
	s.setVec3("planar_light.specular", &pl.specular);
	s.setFloat("planar_light.intensity", pl.intensity);

	s.setVec3("flashlight.color", &f.color);
	s.setFloat("flashlight.intensity", f.intensity);
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
	Shader highlight_shader("highlight_v.glsl", "highlight_f.glsl");
	Shader ground_shader("ground_v.glsl", "ground_f.glsl");
	Shader grass_shader("grass_v.glsl", "grass_f.glsl");

	// Load model
	string models_folder = "C:\\Users\\sodai\\Documents\\projects\\TripsLearnOpenGL\\learnopengl1\\models\\";

	string ground_square_path = models_folder + "square\\square.obj";
	Model ground = Model(ground_square_path.c_str());

	string grass_model_path = models_folder + "grass\\grass.obj";
	Model grass = Model(grass_model_path.c_str());

	string guitar_pack_path = models_folder + "backpack\\backpack.obj";
	Model guitar_pack = Model(guitar_pack_path.c_str());

	string window_path = models_folder + "window\\window.obj";
	Model red_window = Model(window_path.c_str());


	// 3D Transformations
	glm::mat4 model = glm::mat4(1.0f); // model transform

	glm::mat4 ground_model = glm::mat4(1.0f);
	ground_model = glm::translate(ground_model, glm::vec3(0., -1.7, 0.));
	ground_model = glm::rotate(ground_model, -glm::radians(90.f), glm::vec3(1., 0., 0.));
	ground_model = glm::scale(ground_model, glm::vec3(100., 100., 100.));
	ground_model = glm::translate(ground_model, glm::vec3(-.5, -.5, 0.));

	vector<glm::vec3> vegetation;
	vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));
	for (int i = 0; i < vegetation.size(); i++)
	{
		vegetation[i] *= 3.f;
		vegetation[i] += glm::vec3(0.f, -1.7f, 0.f);
	}
	glm::mat4 grass_model = glm::mat4(1.);
	grass_model = glm::translate(grass_model, glm::vec3(-0.5, -.5, 0.));

	glm::mat4 window_model = glm::translate(glm::mat4(1.), glm::vec3(0., 0., 5.));

	glm::mat4 view = glm::lookAt(cam_pos, cam_fwd + cam_pos, cam_up); // view transform
	
	float fov = 45.0f, aspect_ratio = 4.0f / 3.0f, min_cul = 0.1f, max_cul = 100.0f;
	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, min_cul, max_cul);
	

	// Lights
	glm::vec3 ground_color = glm::vec3(.3, .12, 0.);

	glm::vec3 ambient_color = 0.25f*glm::vec3(0.1f, 0.2f, .05f);

	PointLight p;
	glm::vec3 light_color = glm::vec3(1.f, .2f, 0.f);;
	p.specular = light_color;
	p.diffuse = light_color / steradians;
	p.ambient = glm::vec3(0.f);
	p.intensity = 100.f;
	p.position = glm::vec3(1.f, -1.3f, 5.f);

	PlanarLight pl;
	glm::vec3 sunlight_color = glm::vec3(1.f, 1.f, 0.98f);
	pl.ambient = 0.15f*glm::vec3(0.5f, .8f, 0.92f);
	pl.diffuse = (1.f/steradians) * sunlight_color;
	pl.specular = sunlight_color;
	pl.intensity = 4.f;
	pl.direction = glm::normalize(glm::vec3(0.5f, 1.f, 0.5f));

	FlashLight f;
	f.color = glm::vec3(1.f);
	f.intensity = 10.f;
	glm::vec4 flashlight_offset_viewspace = glm::vec4(0.f);// -0.1f, 0.f, 0.1f, 1.f);;
	glm::vec3 flashlight_offset;

	// Assign uniforms
	ourShader.use();
	ourShader.setMat4("model", &model);
	ourShader.setMat4("view", &view);
	ourShader.setMat4("projection", &projection);

	highlight_shader.use();
	highlight_shader.setMat4("model", &model);
	highlight_shader.setMat4("view", &view);
	highlight_shader.setMat4("projection", &projection);

	ground_shader.use();
	ground_shader.setMat4("model", &ground_model);
	ground_shader.setMat4("view", &view);
	ground_shader.setMat4("projection", &projection);
	ground_shader.setVec3("ground_color", &ground_color);

	grass_shader.use();
	grass_shader.setMat4("model", &grass_model);
	grass_shader.setMat4("view", &view);
	grass_shader.setMat4("projection", &projection);

	pass_lights_to_shader(ourShader, p, pl, f);
	pass_lights_to_shader(ground_shader, p, pl, f);


	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Background color
	glClearColor(ambient_color.x, ambient_color.y, ambient_color.z, 1.0f);

	// Enable z-buffer depth test
	glEnable(GL_DEPTH_TEST);

	// Enable stencil buffer
	glEnable(GL_STENCIL_TEST);

	// Enable Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Handle input
		processInput(window, &ourShader);

		// Rendering
		float new_t = glfwGetTime();
		dt = new_t - t;
		t = new_t;
		/*
		cout << "frametime " << dt << endl;
		cout << "framerate " << 1./dt << endl;
		*/

		// Co-ordinate systems
		view = glm::lookAt(cam_pos, cam_pos + cam_fwd, cam_up);

		ourShader.use();
		ourShader.setMat4("model", &model);
		ourShader.setMat4("view", &view);
		ourShader.setVec3("view_pos", &cam_pos);

		highlight_shader.use();
		highlight_shader.setMat4("model", &model);
		highlight_shader.setMat4("view", &view);

		ground_shader.use();
		ground_shader.setMat4("view", &view);

		grass_shader.use();
		grass_shader.setMat4("view", &view);

		// Flashlight
		ourShader.use();
		f.offset = glm::vec3(glm::inverse(view) * flashlight_offset_viewspace);
		ourShader.setVec3("flashlight.offset", &f.offset);
		ourShader.setVec3("flashlight.direction", cam_fwd.x, cam_fwd.y, cam_fwd.z);

		glEnable(GL_DEPTH_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		// Draw ground
		glStencilMask(0x00);
		ground_shader.use();
		ground.draw(ground_shader);

		// Draw grass
		glStencilMask(0x00);
		grass_shader.use();
		for (unsigned int i = 0; i < vegetation.size(); i++)
		{
			grass_model = glm::translate(glm::mat4(1.), vegetation[i]);
			grass_shader.setMat4("model", &grass_model);
			grass.draw(grass_shader);
		}

		// Draw window
		grass_shader.use();
		grass_shader.setMat4("model", &window_model);
		red_window.draw(grass_shader);

		// Draw backpack
		glStencilMask(0xFF);
		ourShader.use();
		guitar_pack.draw(ourShader);


		// Highlight
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); // don't write to the stencil buffer. can't we just stencil op with three keeps?
		//glDisable(GL_DEPTH_TEST);
		
		highlight_shader.use();
		guitar_pack.draw(highlight_shader);

		glStencilMask(0xFF); // Actually need this for glClear to work!

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
