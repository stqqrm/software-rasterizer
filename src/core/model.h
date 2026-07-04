#pragma once
#include "inc.h"
#include "mesh.h"
#include "material.h"

namespace core {

class model {
public:
	bool Load(const fs::path& file_path);
	const std::vector<mesh>& GetMeshes() const { return _meshes; }
	std::vector<mesh>& GetMeshes() { return _meshes; }
	const std::vector<material>& GetMaterials() const { return _materials; }
	std::vector<material>& GetMaterials() { return _materials; }
	const std::vector<texture>& GetTextures() const { return _textures; }
	std::vector<texture>& GetTextures() { return _textures; }
	int GetNumberOfTriangles() {
		int result = 0;
		for (const auto& m : _meshes)
			result += static_cast<int>(m.GetIndexCount() / 3);
		return result;
	}
	void Translate(const glm::vec3& translation) {
		for (auto& m : _meshes) {
			m.Translate(translation);
			m.ComputeBoundingSphere();
		}
	}
	void Rotate(const glm::vec3& v) {
		for (auto& m : _meshes) {
			m.Rotate(v);
			m.ComputeBoundingSphere();
		}
	}
private:
	std::vector<mesh> _meshes;
	std::vector<material> _materials;
	std::vector<texture> _textures;
};

} // namespace core
