#include <iostream>
#include <string>
#include <fstream>
#include <map>

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

const int window_width = 800;
const int window_height = 600;

constexpr float steradians = 4.f * glm::pi<float>();

glm::vec3 cam_pos = glm::vec3(0.0f, 6.0f, 60.f);
glm::vec3 cam_fwd = glm::vec3(0.0f, -0.1f, -1.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

float t = 0.f;
float dt = 0.f;

bool first_mouse = true;
const float mouse_sensitivity = 0.001f;
float mouse_x, mouse_y = 0.;
float phi = glm::acos(cam_fwd.z);
float theta = glm::acos(cam_fwd.y);

struct TextureData
{
	unsigned char* data;
	int width, height, channels;
};

// Function to call when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Function to process all key inputs
void processInput(GLFWwindow* window)
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

TextureData load_texture(string path)
{
	TextureData td;


	td.data = stbi_load(path.c_str(), &td.width, &td.height, &td.channels, 0);

	return td;
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

void many_quad_scene(GLFWwindow* window)
{
	float quadVertices[] = {
		// positions     // colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};
}

void planet_scene(GLFWwindow* window)
{
	// 3D Transformations
	glm::mat4 view = glm::lookAt(cam_pos, cam_fwd + cam_pos, cam_up); // view transform
	
	float fov = 45.0f, aspect_ratio = 4.0f / 3.0f, min_cul = 0.1f, max_cul = 1000.0f;
	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, min_cul, max_cul);

	// Load models
	string models_folder = "models\\";
	
	string square_path = models_folder + "square\\square.obj";
	string cube_path = models_folder + "cube\\cube.obj";

	string planet_path = models_folder + "planet\\planet.obj";
	Model planet(planet_path.c_str());
	Shader planet_shader("VertexShader.glsl", "FragmentShader.glsl");
	glm::mat4 planet_space = glm::mat4(1.);
	planet_space = glm::scale(planet_space, glm::vec3(4.f));

	string rock_path = models_folder + "rock\\rock.obj";
	Model rock(rock_path.c_str());
	Shader rock_shader("instance_v.glsl", "FragmentShader.glsl");
	unsigned int rock_count = 4e4;
	glm::mat4* rock_spaces;
	rock_spaces = new glm::mat4[rock_count];
	float radius = 100.; //mean radius of the asteroids
	float offset = 25.f; // deviation of asteroids from mean
	for (unsigned int i = 0; i < rock_count; i++)
	{
		glm::mat4 model = glm::mat4(1.f);
		// location around the planet
		float angle = (float)i / (float)rock_count * 360.;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;
		float y = displacement * .4f;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// rescale the rock
		float scale = (rand() % 20) / 100.f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// random orientation
		angle = (rand() % 360);
		model = glm::rotate(model, angle, glm::vec3(.4f, .6f, .8f));

		rock_spaces[i] = model;
	}
	// Pass instance transform array, rock_spaces, to each mesh of the rock model
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, rock_count * sizeof(glm::mat4), &rock_spaces[0], GL_STATIC_DRAW);
	for (unsigned int i = 0; i < rock.meshes.size(); i++)
	{
		unsigned int VAO = rock.meshes[i].VAO;
		glBindVertexArray(VAO);

		std::size_t vec4size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);

		//have to give attrib to each column individually as can only send 4 floats at a time
		for (unsigned int j = 0; j < 4; j++)
		{
			glVertexAttribPointer(3 + j, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)(j*vec4size));
			glEnableVertexAttribArray(3 + j);
			//only move to the next instance transform after done rendering each 
			glVertexAttribDivisor(3 + j, 1);
		}

		glBindVertexArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Framebuffer shader
	Shader pp_shader("postprocess_v.glsl", "postprocess_f.glsl");
	Model frame(square_path.c_str());
	glm::mat4 frame_model = glm::mat4(1.);
	frame_model = glm::scale(frame_model, glm::vec3(2., 2., 1.));
	frame_model = glm::translate(frame_model, glm::vec3(-0.5, -0.5, 0.));
	pp_shader.use();
	pp_shader.setMat4("model", &frame_model);

	// Cubemap
	Shader skybox_shader("cubemap_v.glsl", "cubemap_f.glsl");
	Model skybox(cube_path.c_str());
	glm::mat4 skybox_model = glm::translate(glm::mat4(1.), glm::vec3(-0.5));
	skybox_shader.use();
	skybox_shader.setMat4("model", &skybox_model);
	skybox_shader.setMat4("view", &view);
	skybox_shader.setMat4("projection", &projection);

	// Share view and projection matrices via Uniform buffer
	unsigned int mat4_size = sizeof(glm::mat4);
	vector<Shader> sharing_shaders = {
		rock_shader,
		planet_shader
	};
	for (vector<Shader>::iterator it = sharing_shaders.begin(); it != sharing_shaders.end(); ++it)
	{
		unsigned int uni_block_idx= glGetUniformBlockIndex(it->ID, "shared_matrices");

		glUniformBlockBinding(it->ID, uni_block_idx, 0);
	}

	unsigned int shared_matrices_ubo;
	glGenBuffers(1, &shared_matrices_ubo);

	glBindBuffer(GL_UNIFORM_BUFFER, shared_matrices_ubo);
	glBufferData(GL_UNIFORM_BUFFER, // target
		2*mat4_size, // buffer size
		NULL, // Data
		GL_STATIC_DRAW); // access pattern
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// Link entire buffer to binding point 0
	glBindBufferRange(GL_UNIFORM_BUFFER,
		0, // binding point index
		shared_matrices_ubo, // buffer object
		0, // buffer start address
		2*mat4_size); // buffer end address
	// send matrices
	glBufferSubData(GL_UNIFORM_BUFFER, 0, mat4_size, glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, mat4_size, mat4_size, glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Lights
	glm::vec3 ambient_color = 0.25f*glm::vec3(0.1f, 0.2f, .05f);

	PointLight p;
	glm::vec3 light_color = glm::vec3(1.f, .2f, 0.f);;
	p.specular = light_color;
	p.diffuse = light_color / steradians;
	p.ambient = glm::vec3(0.f);
	p.intensity = 0.f;
	p.position = glm::vec3(1.f, -1.3f, 5.f);

	PlanarLight pl;
	glm::vec3 sunlight_color = glm::vec3(1.f, 1.f, 0.98f);
	pl.ambient = 0.3*glm::vec3(0.5f, .8f, 0.96f);
	pl.diffuse = (1.f/steradians) * sunlight_color;
	pl.specular = sunlight_color;
	pl.intensity = 1.f;
	pl.direction = glm::normalize(glm::vec3(1.f, .1f, 1.f));
	glm::vec3 sun_rot_axis(0., 1., 0.);

	FlashLight f;
	f.color = glm::vec3(1.f);
	f.intensity = 10.f;
	glm::vec4 flashlight_offset_viewspace = glm::vec4(0.f);// -0.1f, 0.f, 0.1f, 1.f);;
	glm::vec3 flashlight_offset;


	pass_lights_to_shader(rock_shader, p, pl, f);
	pass_lights_to_shader(planet_shader, p, pl, f);

	// Framebuffer
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	// Attach renderbuffer for stencil & depth testing (not sampling)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pp_shader.setInt("screen_texture", 0);

	// Cubemap
	vector<string> cubemap_paths =
	{
		"skybox\\left.jpg",
		"skybox\\right.jpg",
		"skybox\\top.jpg",
		"skybox\\bottom.jpg",
		"skybox\\back.jpg",
		"skybox\\front.jpg"
	};
	unsigned int cubemap_tex_id;
	glGenTextures(1, &cubemap_tex_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex_id);

	stbi_set_flip_vertically_on_load(false);
	for (unsigned int i = 0; i < cubemap_paths.size(); i++)
	{
		TextureData td = load_texture(cubemap_paths[i]);
		if (td.data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, td.width, td.height, 0,
				GL_RGB, GL_UNSIGNED_BYTE, td.data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			cout << "Cubemap loaded " << cubemap_paths[i] << endl;
			stbi_image_free(td.data);
		}
		else
		{
			cout << "Cubemap failed to load at path " << cubemap_paths[i] << endl;
			stbi_image_free(td.data);
		}
	}

	

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Background color
	glClearColor(ambient_color.x, ambient_color.y, ambient_color.z, 1.0f);

	// Enable z-buffer depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Enable stencil buffer
	//glEnable(GL_STENCIL_TEST);

	// Enable Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable face culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Handle input
		processInput(window);

		// Rendering
		float new_t = glfwGetTime();
		dt = new_t - t;
		t = new_t;
		//cout << "frametime " << 1000.*dt << "ms" << endl;
		//cout << "framerate " << 1./dt << endl;

		// Co-ordinate systems
		view = glm::lookAt(cam_pos, cam_pos + cam_fwd, cam_up);

		glBindBuffer(GL_UNIFORM_BUFFER, shared_matrices_ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, mat4_size, mat4_size, glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		rock_shader.use();
		rock_shader.setFloat("time", t);
		rock_shader.setVec3("view_pos", &cam_pos);

		planet_shader.use();
		planet_shader.setVec3("view_pos", &cam_pos);

		// Render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glEnable(GL_DEPTH_TEST);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw planet
		planet_shader.use();
		planet_shader.setMat4("model", &planet_space);
		planet.draw(planet_shader);

		// Draw rocks
		/* Non-instanced method
		for (unsigned int i = 0; i < rock_count; i++)
		{
			planet_shader.setMat4("model", &rock_spaces[i]);
			rock.draw(planet_shader);
		}
		*/
		// Instanced method
		rock_shader.use();
		rock.draw(rock_shader, rock_count);

		// Skybox
		skybox_shader.use();
		glm::mat4 skybox_view = glm::lookAt(glm::vec3(0.), cam_fwd, cam_up);
		skybox_shader.setMat4("view", &skybox_view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex_id);
		//skybox.draw(skybox_shader);

		// Post-process framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		pp_shader.use();
		pp_shader.setInt("screen_texture", 0);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		frame.draw(pp_shader);

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteFramebuffers(1, &fbo);

	delete[] rock_spaces;
}

int main(int argc, char** argv)
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window object
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Initialize GLAD (Loads OpenGL routines)
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

	planet_scene(window);

	
	
	// Clean-up
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteProgram(shaderProgram); // should probably implement destructor in Shader
	glfwTerminate();
	return 0;

}
