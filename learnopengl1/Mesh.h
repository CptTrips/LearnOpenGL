#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include "Shader.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coord;
};

struct Texture {
	unsigned int id;
	string type; // "diffuse", "specular", "emissive"
	string path; // for checking if we've already loaded
};

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

	void draw(Shader& shader, int instances=1);

private:
	unsigned int VAO, VBO, EBO;

	void setup_mesh();
};

