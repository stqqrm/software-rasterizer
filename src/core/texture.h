#pragma once
#include "inc.h"
#include "font.h"

namespace core {

enum class cubemap_layout {
	layout_4x3,
	layout_3x4,
	layout_cross,
};

class texture {
public:
	texture() = default;
	~texture() = default;
	texture(const texture&) = default;
	bool Load(const fs::path& path);
	bool Load(const font& path);
	st::int1 sample4_2d(const st::float2& uv) const;
	st::int1 sample4_3d(const st::float3& uv) const;
	st::int1 sample4_cubemap(const st::float3& uv) const;
    st::int1 sample4_quad(const st::float3& uv) const;
public:
	int _width		= 0;
	int _height		= 0;
	int _depth		= 1;
	int _numChannels= 0;
	std::vector<unsigned char> _data;
};

} // namespace core
