#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <iostream>
#include "core.h"

namespace core {

bool texture::Load(const fs::path& path) {
    int width, height, numChannels;
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &numChannels, 0);
    if (!data) {
        const char* err = stbi_failure_reason();
        std::cerr << "stb_image error: " << err << " " << path.string() << std::endl;
        return false;
    }
    _width = width;
    _height = height;
    _numChannels = numChannels;
    _data.assign(data, data + (width * height * numChannels));
    stbi_image_free(data);
    return true;
}

bool texture::Load(const font& f) {
    _width = f.GetAtlasWidth();
    _height = f.GetAtlasHeight();
    _numChannels = 1;
    const auto& atlas = f.GetAtlas();
    _data.clear();
    _data.reserve(atlas.size());
    _data.insert(_data.end(), atlas.begin(), atlas.end());
    return true;
}

st::int1 texture::sample4_2d(const st::float2& uv) const {
    st::float1 fx = st::frac(uv.x) * (float)_width;
    st::float1 fy = st::frac(uv.y) * (float)_height;
    alignas(16) float ux[st::simd_width];
    alignas(16) float uy[st::simd_width];
    st::load(fx, ux);
    st::load(fy, uy);
    alignas(16) int result[st::simd_width] = {};
    int write_mask = core::GetWriteMask();
    for (int i = 0; i < st::simd_width; i++) {
        if (!(write_mask & (1 << i))) continue;
        int x = (int)ux[i];
        int y = (int)uy[i];
        x = x < 0 ? 0 : (x >= _width ? _width - 1 : x);
        y = y < 0 ? 0 : (y >= _height ? _height - 1 : y);
        const uint8_t* p = _data.data() + (y * _width + x) * _numChannels;
        result[i] = _numChannels >= 4
            ? (p[3] << 24) | (p[0] << 16) | (p[1] << 8) | p[2]
            : (0xFF << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
    }
    return st::to_int1(result);
}

st::int1 texture::sample4_3d(const st::float3& uv) const {
    alignas(16) float ux[st::simd_width];
    alignas(16) float uy[st::simd_width];
    alignas(16) float uz[st::simd_width];
    st::load(uv.x, ux);
    st::load(uv.y, uy);
    st::load(uv.z, uz);
    alignas(16) int result[st::simd_width] = { 0 };
    int write_mask = core::GetWriteMask();
    for (int i = 0; i < st::simd_width; i++) {
        if (!(write_mask & (1 << i))) continue;
        float x = ux[i];
        float y = uy[i];
        float z = uz[i];
        float len = sqrtf(x * x + y * y + z * z);
        if (len > 0.0001f) { x /= len; y /= len; z /= len; }
        float fu = (atan2f(x, z) / (2.0f * 3.14159265f)) + 0.5f;
        float fv = 0.5f - (asinf(y) / 3.14159265f);
        int px = (int)(fu * (float)(_width - 1) + 0.5f);
        int py = (int)(fv * (float)(_height - 1) + 0.5f);
        px = px < 0 ? 0 : (px >= (int)_width ? (int)_width - 1 : px);
        py = py < 0 ? 0 : (py >= (int)_height ? (int)_height - 1 : py);
        size_t idx = (size_t)(py * _width + px) * _numChannels;
        const uint8_t* p = &_data[idx];
        if (_numChannels >= 4)
            result[i] = (p[3] << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
        else if (_numChannels == 3)
            result[i] = (0xFF << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
        else if (_numChannels == 2)
            result[i] = (p[1] << 24) | (p[0] << 16) | (p[0] << 8) | p[0];
        else
            result[i] = (0xFF << 24) | (p[0] << 16) | (p[0] << 8) | p[0];
    }
    return st::to_int1(result);
}

st::int1 texture::sample4_cubemap(const st::float3& dir) const {
    static constexpr float inv4 = 1.0f / 4.0f;
    static constexpr float inv3 = 1.0f / 3.0f;
    st::float3 a = st::abs(dir);
    st::float1 ax_dom = (a.x >= a.y) & (a.x >= a.z);
    st::float1 ay_dom = (a.y >= a.x) & (a.y >= a.z) & ~ax_dom;
    st::float1 az_dom = ~ax_dom & ~ay_dom;
    st::float1 inv_ax = st::float1(1.0f) / a.x;
    st::float1 inv_ay = st::float1(1.0f) / a.y;
    st::float1 inv_az = st::float1(1.0f) / a.z;
    st::float1 inv_ma = st::select(ax_dom, inv_ax, st::select(ay_dom, inv_ay, inv_az));
    st::float1 pos_x = (dir.x > 0.0f);
    st::float1 pos_y = (dir.y > 0.0f);
    st::float1 pos_z = (dir.z > 0.0f);
    st::float1 u_px = -dir.z * inv_ax, v_px = -dir.y * inv_ax;
    st::float1 u_nx = dir.z * inv_ax, v_nx = -dir.y * inv_ax;
    st::float1 u_py = dir.x * inv_ay, v_py = dir.z * inv_ay;
    st::float1 u_ny = dir.x * inv_ay, v_ny = -dir.z * inv_ay;
    st::float1 u_pz = dir.x * inv_az, v_pz = -dir.y * inv_az;
    st::float1 u_nz = -dir.x * inv_az, v_nz = -dir.y * inv_az;
    st::float1 u_x = st::select(pos_x, u_px, u_nx);
    st::float1 v_x = st::select(pos_x, v_px, v_nx);
    st::float1 u_y = st::select(pos_y, u_py, u_ny);
    st::float1 v_y = st::select(pos_y, v_py, v_ny);
    st::float1 u_z = st::select(pos_z, u_pz, u_nz);
    st::float1 v_z = st::select(pos_z, v_pz, v_nz);
    st::float1 u = st::select(ax_dom, u_x, st::select(ay_dom, u_y, u_z));
    st::float1 v = st::select(ax_dom, v_x, st::select(ay_dom, v_y, v_z));
    st::float1 fx_x = st::select(pos_x, 2.0f, 0.0f);
    st::float1 fy_x = st::float1(1.0f);
    st::float1 fx_y = st::float1(1.0f);
    st::float1 fy_y = st::select(pos_y, 0.0f, 2.0f);
    st::float1 fx_z = st::select(pos_z, 1.0f, 3.0f);
    st::float1 fy_z = st::float1(1.0f);
    st::float1 fx = st::select(ax_dom, fx_x, st::select(ay_dom, fx_y, fx_z));
    st::float1 fy = st::select(ax_dom, fy_x, st::select(ay_dom, fy_y, fy_z));
    u = (fx + 0.5f * (u + 1.0f)) * inv4;
    v = (fy + 0.5f * (v + 1.0f)) * inv3;
    u = u - st::floor(u);
    v = v - st::floor(v);
    st::int1 tx = st::floor(u * st::float1((float)_width));
    st::int1 ty = st::floor(v * st::float1((float)_height));
    tx = st::max(st::int1(0), st::min(tx, _width - 1));
    ty = st::max(st::int1(0), st::min(ty, _height - 1));
    alignas(16) int tx_arr[st::simd_width] = { 0 };
    alignas(16) int ty_arr[st::simd_width] = { 0 };
    alignas(16) int result[st::simd_width] = { 0 };
    st::load(tx, tx_arr);
    st::load(ty, ty_arr);
    int write_mask = core::GetWriteMask();
    for (int i = 0; i < st::simd_width; i++) {
        if (!(write_mask & (1 << i))) continue;
        const uint8_t* p = _data.data() + (ty_arr[i] * _width + tx_arr[i]) * _numChannels;
        if (_numChannels >= 4)
            result[i] = (p[3] << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
        else
            result[i] = (255 << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
    }
    return st::to_int1(result);
}

st::int1 texture::sample4_quad(const st::float3& uv) const {
    
}

} // namespace core
