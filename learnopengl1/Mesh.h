#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

using std::string;
using std::vector;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tex_coord;
};

struct Texture {
	unsigned int id;
	string type; // "diffuse", "specular", "emissive"
};

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

	void draw(Shader& shader);

private:
	unsigned int VAO, VBO, EBO;

	void setup_mesh();
};

