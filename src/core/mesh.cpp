#include "mesh.h"
#include <iostream>

namespace core {

bool mesh::Load(const aiMesh* pMesh) {
    if (!pMesh) {
        std::cerr << "Error: Null mesh pointer provided." << std::endl;
        return false;
    }
    if (!pMesh->HasPositions()) {
        std::cerr << "Warning: Mesh has no vertex positions, skipping" << std::endl;
        return false;
    }
    _vertices.clear();
    _indices.clear();
    _material_index = pMesh->mMaterialIndex;
    _vertices.reserve(pMesh->mNumVertices);
    _indices.reserve(pMesh->mNumFaces * 3);
    for (unsigned int i = 0; i < pMesh->mNumVertices; i++) {
        vertex v;
        v.position = { pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z };
        v.normal = pMesh->HasNormals()
            ? glm::vec3(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z)
            : glm::vec3(0.0f);
        v.uv = (pMesh->mTextureCoords[0] && pMesh->mNumUVComponents[0] >= 2)
            ? glm::vec3(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y, 0.0f)
            : glm::vec3(0.0f);
        _vertices.push_back(v);
    }
    for (unsigned int i = 0; i < pMesh->mNumFaces; i++) {
        const aiFace& face = pMesh->mFaces[i];
        if (face.mNumIndices != 3) continue;
        for (unsigned int j = 0; j < 3; j++)
            _indices.push_back(face.mIndices[j]);
    }
    if (_vertices.empty()) {
        std::cerr << "Error: No valid mesh data found." << std::endl;
        return false;
    }
    ComputeBoundingSphere();
    _index_count = (uint32_t)_indices.size();
    return true;
}

void mesh::Translate(const glm::vec3& v) {
    for (auto& vertex : _vertices)
        vertex.position += v;
}

void mesh::Rotate(const glm::vec3& v) {
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(v.y), glm::vec3(0, 1, 0));
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(v.x), glm::vec3(1, 0, 0));
    glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(v.z), glm::vec3(0, 0, 1));
    glm::mat4 rotationMatrix = rotY * rotX * rotZ;
    for (auto& vertex : _vertices) {
        vertex.position = glm::vec3(rotationMatrix * glm::vec4(vertex.position, 1.0f));
        vertex.normal = glm::vec3(rotationMatrix * glm::vec4(vertex.normal, 0.0f));
        vertex.normal = glm::normalize(vertex.normal);
    }
}

void mesh::Scale(const glm::vec3& v) {
    for (auto& vert : _vertices)
        vert.position *= v;
}

void mesh::Scale(float f) {
    for (auto& vert : _vertices)
        vert.position *= f;
}

void mesh::ComputeBoundingSphere() {
    if (_vertices.empty()) return;
    glm::vec3 center(0);
    for (auto& v : _vertices)
        center += v.position;
    center /= (float)_vertices.size();
    float radius = 0;
    for (auto& v : _vertices) {
        float d = glm::length(v.position - center);
        if (d > radius) radius = d;
    }
    _bounding_sphere = { center, radius };
}

void mesh::print() const {
    std::cout << "Mesh Data:" << std::endl;
    for (size_t i = 0; i < _vertices.size(); i++) {
        const vertex& v = _vertices[i];
        std::cout << "Vertex " << i << ": " << std::endl;
        std::cout << "  Position: (" << v.position.x << ", " << v.position.y << ", " << v.position.z << ")" << std::endl;
        std::cout << "  Normal: (" << v.normal.x << ", " << v.normal.y << ", " << v.normal.z << ")" << std::endl;
        std::cout << "  UV: (" << v.uv.x << ", " << v.uv.y << ")" << std::endl;
    }
    for (size_t i = 0; i < _indices.size(); i += 3)
        std::cout << "Triangle: " << _indices[i] << ", " << _indices[i + 1] << ", " << _indices[i + 2] << std::endl;
}

} // namespace core
