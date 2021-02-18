#include "Mesh.h"
/*
using std::vector;
using std::string;
*/

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setup_mesh();
}

void Mesh::setup_mesh()
{
	// Tell OpenGL to make space for new buffers and arrays and get a handle
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Use the newly created buffers as the current buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// Pass data to the buffers
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Specify how Vertex data should be sliced for position, normal, etc.
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

	// debind the vertex array to avoid re-associating a different VBO/EBO
	glBindVertexArray(0);
}

void Mesh::draw(Shader& shader)
{
	unsigned int diffuse_tex_count = 0;
	unsigned int specular_tex_count = 0;

	/* I think the way I want this to work is that a texture implicitly has a 
	diffuse specular and emissive component. The shader accepts an array of 
	textures (size of which is the greatest OpenGL allows). So I will 
	glGenTexture(3, [name])*/
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		// Shouldn't we find the index of the texture in the loaded_textures? Do we have access to that here?
		glActiveTexture(GL_TEXTURE0 + i);

		string number;
		string name = textures[i].type;
		if (name == "diffuse") number = std::to_string(diffuse_tex_count++);
		else if (name == "specular") number = std::to_string(specular_tex_count++);

		string shader_var = "materials[" + number + "]." + name;
		shader.setInt(shader_var.c_str(), i);

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);

	// Draw call
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
