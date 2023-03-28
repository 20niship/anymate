#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <movutl/instance/instance.hpp>
#include <movutl/io/object_loader.hpp>

namespace mu::io {
using namespace mu::db;
using namespace mu::render;
#if 0

static MeshCol* processMesh(aiMesh* mesh, const aiScene* ) {
  auto m = instance::create_mesh_col();
  // walk through each of the mesh's vertices
  for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
    MeshCol::Vertex vertex;
    vertex.pos[0] = mesh->mVertices[i].x;
    vertex.pos[1] = mesh->mVertices[i].y;
    vertex.pos[2] = mesh->mVertices[i].z;
    if(mesh->mTextureCoords[0]) {
      vertex.uv[0] = mesh->mTextureCoords[0][i].x;
      vertex.uv[1] = mesh->mTextureCoords[0][i].y;
    } else {
      vertex.uv[0] = vertex.uv[1] = 0;
    }
    m->vertex.push_back(std::move(vertex));
  }
  return m;
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
static void processNode(Body* b, aiNode* node, const aiScene* scene) {
  // process each mesh located at the current node
  for(unsigned int i = 0; i < node->mNumMeshes; i++) {
    // the node object only contains indices to index the actual objects in the scene.
    // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    b->meshs.push_back(processMesh(mesh, scene));
  }

  DISP(node->mNumChildren);
  // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for(unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(b, node->mChildren[i], scene);
  }
}


db::Body *impl_load_ply(const char* file_name) {
  DISP(file_name);
  db::Body *b = instance::create_body();

  Assimp::Importer importer;
  std::cout << "1" << std::endl;
  const aiScene* scene = importer.ReadFile(file_name, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  std::cout << "2" << std::endl;
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    LOGE << "ERROR::ASSIMP:: " << importer.GetErrorString();
    return nullptr;
  }
  /* b->directory = std::filesystem::path(file_name).parent_path().string(); */
  /* DISP(b->directory); */

  // process ASSIMP's root node recursively
  processNode(b, scene->mRootNode, scene);
  return b;
}
#endif

} // namespace mu::io
