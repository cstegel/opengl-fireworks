#pragma once

#include "Common.hpp"
#include "cs488/ShaderProgram.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Texture.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>


namespace fw
{

class RenderContext;

// made with help from: http://www.learnopengl.com/#!Model-Loading/Model
class Model
{
public:
	Model() = default;
	Model(const string & modelPath);
	void Render(const RenderContext & context, ShaderProgram & shader, const glm::mat4 & transform);

private:

	void loadModel(const string & path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(
		aiMaterial* mat,
		aiTextureType type);

	string directory;
	vector<Mesh> meshes;
	vector<Texture> loadedTextures;
};

}