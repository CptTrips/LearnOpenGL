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

	return Mesh(vertices, indices, textures);
}


