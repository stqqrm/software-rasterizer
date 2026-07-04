#include "shader.h"
#include "core.h"

namespace core {

fragment_input4 _default_vertex_shader4(const vertex_input4& i, st::float4& clip_pos) {
    glm::mat4 vp;
    core::ReadBuffer(vp, 0, 0);
	st::float4x4 mvp(glm::value_ptr(vp));
    clip_pos = st::mul(mvp, i.position);

    fragment_input4 o;
    o.position = i.position;
    o.normal = st::normalize(i.normal);
    o.uv = i.uv;
    return o;
}

fragment_input4 _skybox_vertex_shader4(const vertex_input4& i, st::float4& clip_pos) {
    st::float4x4 vp;
    core::ReadBuffer(vp, 0, 1);
    clip_pos = st::mul(vp, i.position);

    fragment_input4 o;
    o.position = i.position;
    o.normal = st::normalize(i.normal);
    o.uv = i.position;
    return o;
}

st::int1 _default_fragment_shader4(const fragment_input4& input) {
    material* mat;
    core::ReadBuffer(mat, 2, core::GetBoundMaterialID());

    int diffuse_slot = mat->GetDiffuseSlot();
    if (diffuse_slot == 0) {
        // No diffuse texture was resolved for this material — slot 0 is a
        // sentinel, not a real buffer index (textures are written starting
        // at slot 1), so reading it here would dereference memory that was
        // never written. Fall back to the material's flat diffuse color.
        color c = mat->GetDiffuseColor();
        int32_t packed = (c.a << 24) | (c.r << 16) | (c.g << 8) | c.b;
        return st::int1(packed);
    }

    texture* tex;
    core::ReadBuffer(tex, 3, diffuse_slot);
    return tex->sample4_2d(st::to_float2(input.uv));
}

st::int1 _normals_fragment_shader4(const fragment_input4& input) {
    return st::pack_argb(st::to_01(input.normal));
}

st::int1 _skybox_fragment_shader4(const fragment_input4& i) {
    texture* tex;
    core::ReadBuffer(tex, 1, 0);
    return tex->sample4_cubemap(i.uv);
}

} // namespace core
