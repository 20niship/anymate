#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <movutl/instance/instance.hpp>
#include <movutl/io/object_loader.hpp>


namespace mu::io {

using namespace mu::db;
using namespace mu::render;

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Image struct.
static core::Vec<Image*> loadMaterialTextures(db::Body* b, aiMaterial* mat, aiTextureType type, const char* typeName) {
  core::Vec<Image*> tmp_texes;
  for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    bool skip = false;
    for(unsigned int j = 0; j < b->textures.size(); j++) {
      if(std::strcmp(b->textures[j]->path(), str.C_Str()) == 0) {
        b->textures.push_back(b->textures[j]);
        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
        break;
      }
    }
    if(!skip) { // if texture hasn't been loaded already, load it
      const std::string path = b->directory + "/" + std::string(str.C_Str());
      auto tex               = instance::create_texture(path.c_str());
      tex->texture_name(typeName);
      tex->path(str.C_Str());
      tmp_texes.push_back(tex);
      b->textures.push_back(tex); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
    }
  }
  return tmp_texes;
}


static Mesh* processMesh(db::Body* b, aiMesh* mesh, const aiScene* scene) {
  auto m = instance::create_mesh_3d();
  m->primitive_type(_MeshBase::PrimitiveType::TRIANGLES);

  // walk through each of the mesh's vertices
  for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Mesh::Vertex vertex;
    vertex.pos[0] = mesh->mVertices[i].x;
    vertex.pos[1] = mesh->mVertices[i].y;
    vertex.pos[2] = mesh->mVertices[i].z;
    if(mesh->HasNormals()) {
      vertex.norm[0] = mesh->mNormals[i].x;
      vertex.norm[1] = mesh->mNormals[i].y;
      vertex.norm[2] = mesh->mNormals[i].z;
    }
    if(mesh->mTextureCoords[0]) {
      vertex.uv[0] = mesh->mTextureCoords[0][i].x;
      vertex.uv[1] = mesh->mTextureCoords[0][i].y;

      vertex.tangent[0] = mesh->mTangents[i].x;
      vertex.tangent[1] = mesh->mTangents[i].y;
      vertex.tangent[2] = mesh->mTangents[i].z;
      vertex.bitangent[0] = mesh->mBitangents[i].x;
      vertex.bitangent[0] = mesh->mBitangents[i].y;
      vertex.bitangent[0] = mesh->mBitangents[i].z;
    } else {
      vertex.uv[0] = vertex.uv[1] = 0;
    }
    m->vertex.push_back(std::move(vertex));
  }
  // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
  for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    // retrieve all indices of the face and store them in the indices vector
    for(unsigned int j = 0; j < face.mNumIndices; j++) m->indices.push_back(face.mIndices[j]);
  }
  // process materials
  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
  // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
  // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
  // Same applies to other texture as the following list summarizes:
  // diffuse: texture_diffuseN
  // specular: texture_specularN
  // normal: texture_normalN

  auto diffuseMaps = loadMaterialTextures(b, material, aiTextureType_DIFFUSE, "texture_diffuse");
  if(diffuseMaps.size() > 0)
    for(auto&& d : diffuseMaps) b->textures.push_back(d);
  auto specularMaps = loadMaterialTextures(b, material, aiTextureType_SPECULAR, "texture_specular");
  if(specularMaps.size() > 0)
    for(auto&& d : specularMaps) b->textures.push_back(d);
  auto normalMaps = loadMaterialTextures(b, material, aiTextureType_HEIGHT, "texture_normal");
  if(normalMaps.size() > 0)
    for(auto&& d : normalMaps) b->textures.push_back(d);
  auto heightMaps = loadMaterialTextures(b, material, aiTextureType_AMBIENT, "texture_height");
  if(heightMaps.size() > 0)
    for(auto&& d : heightMaps) b->textures.push_back(d);
  return m;
}


// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
static void processNode(Body* b, aiNode* node, const aiScene* scene) {
  // process each mesh located at the current node
  for(unsigned int i = 0; i < node->mNumMeshes; i++) {
    // the node object only contains indices to index the actual objects in the scene.
    // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    b->meshs.push_back(processMesh(b, mesh, scene));
  }
  // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for(unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(b, node->mChildren[i], scene);
  }
}

db::Body *impl_load_obj(const char* file_name) {
  db::Body *b = instance::create_body();
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(file_name, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    LOGE << "ERROR::ASSIMP:: " << importer.GetErrorString();
    return nullptr;
  }

  // process ASSIMP's root node recursively
  processNode(b, scene->mRootNode, scene);
  return b;
}

} // namespace mu::io
