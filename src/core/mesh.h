#pragma once
#include "inc.h"

namespace core {

struct bounding_sphere {
    glm::vec3 center;
    float radius;
};

class mesh {
    friend class model;
public:
    bool Load(const aiMesh* pMesh);
    const std::vector<vertex>& GetVertices() const { return _vertices; }
    const std::vector<index>& GetIndices() const { return _indices; }
	uint32_t GetIndexCount() const { return _index_count; }
    std::vector<vertex>& GetVertices() { return _vertices; }
    std::vector<index>& GetIndices() { return _indices; }
    constexpr uint32_t GetMaterialIndex() const { return _material_index; }
    
    void Translate(const glm::vec3& v);
    void Rotate(const glm::vec3& v);
    void Scale(const glm::vec3& v);
    void Scale(float f);
    
    const bounding_sphere& GetBoundingSphere() const { return _bounding_sphere; }
    void ComputeBoundingSphere();
    void print(void) const;
private:
    std::vector<vertex> _vertices;
    std::vector<index> _indices;
    uint32_t _material_index = 0;
	uint32_t _index_count = 0;
    bounding_sphere _bounding_sphere;
};

} // namespace core
