#include "model.h"
#include <iostream>
#include <algorithm>
#include "asset_manager.h"

namespace core {

bool model::Load(const fs::path& file_path) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(
        file_path.string(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs
    );
    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }
    for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
        mesh m;
        if (m.Load(pScene->mMeshes[i]))
            _meshes.push_back(m);
        else
            throw std::runtime_error("Assimp error: Failed to load mesh.");
    }
    auto load_tex = [&](const aiMaterial* pMaterial, aiTextureType type) -> int {
        aiString path;
        if (pMaterial->GetTexture(type, 0, &path) == AI_SUCCESS) {
            // FBX files exported on Windows store relative texture paths with
            // backslash separators. On POSIX, std::filesystem::path only
            // treats '/' as a separator, so a raw backslash path is parsed
            // as one literal (non-existent) filename component. Normalize
            // before resolving.
            std::string rel = path.C_Str();
            std::replace(rel.begin(), rel.end(), '\\', '/');
            fs::path full_path = file_path.parent_path() / rel;
            if (!fs::exists(full_path)) {
                std::cerr << "Texture not found: " << full_path << std::endl;
                return 0;
            }
            texture tex;
            if (tex.Load(full_path.string())) {
                printf("Loaded texture: %s\n", full_path.string().c_str());
                _textures.push_back(std::move(tex));
                return _textures.size();
            }
            std::cerr << "Failed to load texture: " << full_path << std::endl;
        }
        return 0;
    };
    printf("Materials: %d, Textures: %d\n", pScene->mNumMaterials, pScene->mNumTextures);
    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial* pMat = pScene->mMaterials[i];
        material mat;
        mat._diffuse_slot = load_tex(pMat, aiTextureType_DIFFUSE);
        mat._specular_slot = load_tex(pMat, aiTextureType_SPECULAR);
        mat._normal_slot = load_tex(pMat, aiTextureType_NORMALS);
        aiColor4D diffuse;
        if (aiGetMaterialColor(pMat, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS) {
            mat._diffuse_color = {
                (uint8_t)(diffuse.r * 255),
                (uint8_t)(diffuse.g * 255),
                (uint8_t)(diffuse.b * 255),
                (uint8_t)(diffuse.a * 255)
            };
        }
        _materials.push_back(mat);
    }
    return true;
}

} // namespace core
