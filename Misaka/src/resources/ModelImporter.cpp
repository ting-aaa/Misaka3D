#include "ModelImporter.h"

#include <iostream>
#include <vector>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IResource.h"
#include "TextureLoader.h"
#include "AssetManager.h"
#include "../renderer/Geometry.h"
#include "../renderer/Texture.h"
#include "../material/MaterialBase.h"
#include "../material/CommonMaterial.h"

namespace Misaka {

static glm::mat4 AssimpToGLM(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

static void ProcessNode(aiNode* node, const aiScene* scene, ModelPrefab::NodeData& outNode) {
    outNode.name = node->mName.C_Str();
    outNode.localTransform = AssimpToGLM(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        unsigned int meshIndex = node->mMeshes[i];
        outNode.meshIndices.push_back(meshIndex);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        ModelPrefab::NodeData childNode;
        ProcessNode(node->mChildren[i], scene, childNode);
        outNode.children.push_back(childNode);
    }
}

std::shared_ptr<ModelPrefab> ModelImporter::Load(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices
    );
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    auto prefab = std::make_shared<ModelPrefab>();
    auto assetMgr = AssetManager::Ins();

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        std::string meshName = mesh->mName.C_Str();
        if (meshName.empty()) {
            meshName = "mesh_" + std::to_string(i);
        }
        std::string geoID = path + meshName;
        
        auto geometry = assetMgr->GetOrLoad<Geometry>(geoID, [&]() {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;

            for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
                // 位置
                vertices.push_back(mesh->mVertices[v].x);
                vertices.push_back(mesh->mVertices[v].y);
                vertices.push_back(mesh->mVertices[v].z);
                // 法线
                if (mesh->mNormals) {
                    vertices.push_back(mesh->mNormals[v].x);
                    vertices.push_back(mesh->mNormals[v].y);
                    vertices.push_back(mesh->mNormals[v].z);
                } else {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }
                // 纹理坐标
                if (mesh->mTextureCoords[0]) {
                    vertices.push_back(mesh->mTextureCoords[0][v].x);
                    vertices.push_back(mesh->mTextureCoords[0][v].y);
                } else {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }
                // 切线
                if (mesh->HasTangentsAndBitangents()) {
                    vertices.push_back(mesh->mTangents[v].x);
                    vertices.push_back(mesh->mTangents[v].y);
                    vertices.push_back(mesh->mTangents[v].z);
                } else {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }
            }

            for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
                aiFace face = mesh->mFaces[f];
                for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            return std::make_shared<Geometry>(vertices, indices);
        });

        // 处理材质
        aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
        aiString matName;
        aiMat->Get(AI_MATKEY_NAME, matName);
        std::string strMatName = matName.C_Str();
        std::string matID = path + "mat_" + strMatName;

        auto material = assetMgr->GetOrLoad<CommonMaterial>(matID, [&]() {
            auto shader = assetMgr->Find<Shader>("assets/shader/common");
            auto mat = std::make_shared<CommonMaterial>(shader);
            
            // 获取纹理
            auto loadTexture = [&](aiTextureType type) -> unsigned int {
                if (aiMat->GetTextureCount(type) > 0) {
                    aiString texPath;
                    aiMat->GetTexture(type, 0, &texPath);
                    std::string resolvedPath = PathResolver::Resolve(texPath.C_Str(), path);
                    std::string texUID = resolvedPath;
                    
                    auto tex = assetMgr->GetOrLoad<Texture>(texUID, [&]() {
                        return TextureLoader::Load(resolvedPath);
                    });
                    
                    if (tex) return tex->id;
                }
                return 0;
            };

            mat->albedoMap = loadTexture(aiTextureType_DIFFUSE);
            mat->normalMap = loadTexture(aiTextureType_NORMALS);
            mat->roughnessMap = loadTexture(aiTextureType_SHININESS);
            mat->metallicMap = loadTexture(aiTextureType_METALNESS);

            // 获取基础颜色
            aiColor4D diffuse;
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) {
                mat->color = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
            }

            return mat;
        });

        ModelPrefab::MeshEntry entry;
        entry.name = meshName;
        entry.geometry = geometry;
        entry.material = material;
        prefab->meshes.push_back(entry);
    }

    // 处理节点层级
    ProcessNode(scene->mRootNode, scene, prefab->rootNode);

    return prefab;
}

} // namespace Misaka
