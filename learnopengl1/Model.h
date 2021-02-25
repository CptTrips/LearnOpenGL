#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Shader.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h" // image loading library for textures

using std::vector; using std::string;
using std::cout; using std::endl;

class Model
{
public:
	Model(const char* path)
	{
		load_model(path);
	}
	void draw(Shader& shader);

private:
	vector<Texture> loaded_textures;
	vector<Mesh> meshes;
	string directory;

	void load_model(string path, bool flip_uvs=true);
	void process_node(aiNode* node, const aiScene* scene);
	Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type,
		string type_name);
	unsigned int texture_from_file(const char* path);
};

