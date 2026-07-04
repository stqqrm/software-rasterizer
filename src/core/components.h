#pragma once
#include "inc.h"

namespace cmp {

struct transform {
	glm::vec3 p{ 0.0f };
	glm::vec3 r{ 0.0f };
	glm::vec3 s{ 1.0f };
};

struct position {
	glm::vec3 p;
};

struct rotation {
	glm::vec3 r;
};

struct velocity {
	glm::vec3 v;
};

struct health {
	int h;
};

struct name {
	std::string n;
};

struct mesh_collider {
	std::vector<core::vertex>   vertices;
	std::vector<uint32_t>       indices;
};

struct script {
	void(*s)(int ent);
};

struct tag_player {};
struct tag_entity {};
struct tag_static {};

} // namespace cmp
