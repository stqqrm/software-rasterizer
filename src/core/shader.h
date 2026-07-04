#pragma once
#include "inc.h"

namespace core {

struct vertex_input4 {
	st::float3 position, normal, uv;
};

struct fragment_input4 {
	st::float3 position, normal, uv;
};

typedef fragment_input4(*vertex_shader_t)(const vertex_input4&, st::float4&);
typedef st::int1(*fragment_shader_t)(const fragment_input4&);


fragment_input4 _default_vertex_shader4(const vertex_input4& input, st::float4& clip_pos);
fragment_input4 _skybox_vertex_shader4(const vertex_input4& input, st::float4& clip_pos);

st::int1 _default_fragment_shader4(const fragment_input4& input);
st::int1 _normals_fragment_shader4(const fragment_input4& input);
st::int1 _skybox_fragment_shader4(const fragment_input4& input);

} // namespace core
