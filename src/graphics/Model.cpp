#include "graphics/Model.hpp"
#include "graphics/RenderContext.hpp"


namespace fw
{

Model::Model(const string & modelPath)
{
	this->directory = modelPath.substr(0, modelPath.find_last_of('/'));
	loadModel(modelPath);
}

void Model::Render(const RenderContext & context, ShaderProgram & shader, const glm::mat4 & transform)
{
	for (Mesh & mesh : meshes)
	{
		mesh.Render(context, shader, transform);
	}
}

void Model::loadModel(const string & path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals);

	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error(
			"assimp failed to load \"" + path + "\" with error: " + importer.GetErrorString());
	}

	this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// convert the node's meshes to our format
	for(uint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	// process child nodes
	for(uint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices(mesh->mNumVertices);
	vector<GLuint> indices;
	vector<Texture> textures;

	// convert vertices
	for(uint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex &vertex = vertices.at(i);

		// positions/normals
		auto & assimpVertex = mesh->mVertices[i];
		vertex.position.x = assimpVertex.x;
		vertex.position.y = assimpVertex.y;
		vertex.position.z = assimpVertex.z;

		auto & assimpNormal = mesh->mNormals[i];
		vertex.normal.x = assimpNormal.x;
		vertex.normal.y = assimpNormal.y;
		vertex.normal.z = assimpNormal.z;

		if (mesh->HasTangentsAndBitangents())
		{
			auto & assimpTangent = mesh->mTangents[i];
			vertex.tangent.x = assimpTangent.x;
			vertex.tangent.y = assimpTangent.y;
			vertex.tangent.z = assimpTangent.z;

			auto & assimpBitangent = mesh->mBitangents[i];
			vertex.bitangent.x = assimpBitangent.x;
			vertex.bitangent.y = assimpBitangent.y;
			vertex.bitangent.z = assimpBitangent.z;
		}

		// texCoords (only take first one since assimp supports multiple per vertex)
		if(mesh->HasTextureCoords(0))
		{
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
	}

	// convert indices
	for(uint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace & face = mesh->mFaces[i];
		for(uint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	float shininess = 0;

	// convert material
	if(mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = this->loadMaterialTextures(
			material,
			aiTextureType_DIFFUSE
		);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		vector<Texture> specularMaps = this->loadMaterialTextures(
			material,
			aiTextureType_SPECULAR
		);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		vector<Texture> normalMaps = this->loadMaterialTextures(
			material,
			aiTextureType_HEIGHT
		);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		vector<Texture> shininessMaps = this->loadMaterialTextures(
			material,
			aiTextureType_SHININESS
		);
		textures.insert(textures.end(), shininessMaps.begin(), shininessMaps.end());

		material->Get(AI_MATKEY_SHININESS, shininess);
	}

	return Mesh(std::move(vertices), std::move(indices), std::move(textures), shininess);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
	vector<Texture> textures;
	for (uint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString aiStr;
		mat->GetTexture(type, i, &aiStr);
		string str = string(aiStr.C_Str());
		bool alreadyLoaded = false;

		for (uint j = 0; j < loadedTextures.size(); j++)
		{
			if (loadedTextures[j].path.compare(str) == 0)
			{
				textures.push_back(this->loadedTextures[j]);
				alreadyLoaded = true;
				break;
			}
		}

		if (!alreadyLoaded)
		{
			string fullPath = this->directory + "/" + str;
			Texture texture;
			texture.id = Texture::LoadFromFile(fullPath);
			texture.type = Texture::GetType(type);
			texture.path = str;
			textures.push_back(texture);
			this->loadedTextures.push_back(texture);
		}
	}

	return textures;
}

}