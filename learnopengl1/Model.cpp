#include "Model.h"


void Model::draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);
}

void Model::load_model(string path)
{
	cout << "loading model " << path << endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('\\'));

	process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
	//cout << "node found" << endl;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//cout << "processing mesh " << i << " of " << node->mNumMeshes << endl;
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(process_mesh(mesh, scene));
	}

	//cout << "node has " << node->mNumChildren << " children" << endl;
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		//cout << "child " << i << " of " << node->mNumChildren << endl;
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

glm::vec2& v2_assimp_to_glm(const aiVector3D& v_in) {

	glm::vec2 v_out;
	v_out.x = v_in.x;
	v_out.y = v_in.y;

	return v_out;
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	// Vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position = v3_assimp_to_glm(mesh->mVertices[i]);
		vertex.normal = v3_assimp_to_glm(mesh->mNormals[i]);

		if (mesh->mTextureCoords[0])
		{
			vertex.tex_coord = v2_assimp_to_glm(mesh->mTextureCoords[0][i]);
		}
		else
			vertex.tex_coord = glm::vec2(0.f);

		vertices.push_back(vertex);

	}

	// Faces/Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//cout << "mesh has " << mesh->mNumVertices << " vertices "
	//	<< mesh->mNumFaces << " faces" << endl;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		vector<Texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

		//cout << "mesh material " << material << endl;
	}

	return Mesh(vertices, indices, textures);
}

unsigned int Model::texture_from_file(const char* path)
{
	int width, height, nrChannels;

	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	cout << "loading texture " << path << endl;

	string path_str = string(path);
	string extention = path_str.substr(path_str.find_last_of('.'), string::npos);

	if (extention == ".jpg")
		stbi_set_flip_vertically_on_load(true);
	else
		stbi_set_flip_vertically_on_load(false);

	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data) {
		// GL_RGB for jpg, GL_RGBA for png
		GLenum channel_type = GL_RGB;
		if (nrChannels == 3)
			channel_type = GL_RGB;
		else if (nrChannels == 4)
			channel_type = GL_RGBA;
		else if (nrChannels == 1)
			channel_type = GL_RED;

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, //mipmap level
			channel_type, width, height, 0, channel_type, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
		string path = directory + '\\' + str.C_Str();

		bool skip = false;
		for (unsigned int j = 0; j < loaded_textures.size(); j++)
		{
			// if texture has alreay been loaded
			if (std::strcmp(loaded_textures[j].path.c_str(), path.c_str()) == 0)
			{
				//cout << "texture exists " << path << endl;
				textures.push_back(loaded_textures[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			//cout << "new texture " << path << endl;
			Texture texture;
			texture.id = texture_from_file(path.c_str());
			texture.type = type_name;
			texture.path = path;
			textures.push_back(texture);
			loaded_textures.push_back(texture);
		}
	}

	return textures;
}

