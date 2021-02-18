#include "Model.h"


void Model::draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);
}

void Model::load_model(string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(process_mesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}
}

glm::vec3& v3_assimp_to_glm(const aiVector3D& v_in) {

	glm::vec3 v_out;
	v_out.x = (float)v_in.x;
	v_out.y = (float)v_in.y;
	v_out.z = (float)v_in.z;

	return v_out;
}

glm::vec2& v2_assimp_to_glm(const aiVector2D& v_in) {

	glm::vec2 v_out;
	v_out.x = (float)v_in.x;
	v_out.y = (float)v_in.y;

	return v_out;
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position = v3_assimp_to_glm(mesh->mVertices[i]);
		vertex.normal = v3_assimp_to_glm(mesh->mNormals[i]);

		vertices.push_back(vertex);

		if (mesh->mMaterialIndex >= 0)
		{
			vertex.tex_coord = glm::vec2(v3_assimp_to_glm(mesh->mTextureCoords[0][i]));
		}
		else
			vertex.tex_coord = glm::vec2(0.f);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < mesh->mNumFaces; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		vector<Texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	return Mesh(vertices, indices, textures);
}

unsigned int texture_from_file(const char* filename, string directory)
{
	int width, height, nrChannels;

	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	string path = directory + filename;

	/*
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_map);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	*/

	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

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
	return texture_id;
}

vector<Texture> Model::load_material_textures(aiMaterial* mat, aiTextureType type, string type_name)
{
	vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		texture.id = texture_from_file(str.C_Str(), directory);
		texture.type = type_name;
		//texture.path = str;
		textures.push_back(texture);
	}
}

