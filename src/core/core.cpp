#include "core.h"

namespace core {

int		                            core::_bound_material_id = 0;
color*							    core::_img_buffer = nullptr;
uint32_t                            core::_img_width = 0;
uint32_t                            core::_img_height = 0;

uint16_t*						    core::_depth_buffer = nullptr;
uint32_t                            core::_depth_width = 0;
uint32_t                            core::_depth_height = 0;

uint16_t*						    core::_shadow_buffer = nullptr;
uint32_t							core::_shadow_width = 0;
uint32_t							core::_shadow_height = 0;

camera*							    core::_camera = nullptr;
ipipeline*					        core::_bound_pipeline = nullptr;
std::vector<triangle_cmd>	        core::_draw_queue;
uint64_t                            core::_draw_queue_next = 0;
uint32_t							core::_write_mask = 0;
std::vector<std::vector<
	std::pair<size_t, void*>>>      core::_buffer;
std::vector<pipeline_desc>	        core::_desc;
handle							    core::_pipeline_handle;


bool inside_near(const glm::vec4& v) {
	return v.z >= -v.w;
}

void lerp_vertex(
	const fragment_input& a, const glm::vec4& clip_a,
	const fragment_input& b, const glm::vec4& clip_b,
	float t,
	fragment_input& out_vert, glm::vec4& out_clip) {
	out_clip = clip_a + (clip_b - clip_a) * t;
	out_vert.uv = a.uv + (b.uv - a.uv) * t;
	out_vert.normal = a.normal + (b.normal - a.normal) * t;
}

int clip_triangle_near(
	const fragment_input in_vert[3],
	const glm::vec4 in_clip[3],
	fragment_input out_vert[4],
	glm::vec4 out_clip[4])
{
	int out_count = 0;

	for (int i = 0; i < 3; i++) {
		const fragment_input& current_vert = in_vert[i];
		const glm::vec4& current_clip = in_clip[i];
		const fragment_input& prev_vert = in_vert[(i + 2) % 3];
		const glm::vec4& prev_clip = in_clip[(i + 2) % 3];

		bool curr_in = inside_near(current_clip);
		bool prev_in = inside_near(prev_clip);

		if (curr_in != prev_in) {
			float t = (prev_clip.z + prev_clip.w) /
				((prev_clip.z + prev_clip.w) - (current_clip.z + current_clip.w));
			lerp_vertex(prev_vert, prev_clip, current_vert, current_clip, t,
				out_vert[out_count], out_clip[out_count]);
			out_count++;
		}

		if (curr_in) {
			out_vert[out_count] = current_vert;
			out_clip[out_count] = current_clip;
			out_count++;
		}
	}

	return out_count;
}

inline st::int1 inside_near4(const st::float4& v) {
	return st::int1(v.z >= -v.w);
}

inline void lerp_vertex4(
	const fragment_input4& a, const st::float4& clip_a,
	const fragment_input4& b, const st::float4& clip_b,
	st::float1 t,
	fragment_input4& out_vert, st::float4& out_clip)
{
	out_clip = clip_a + (clip_b - clip_a) * t;
	out_vert.uv = a.uv + (b.uv - a.uv) * t;
	out_vert.normal = a.normal + (b.normal - a.normal) * t;
	out_vert.position = a.position + (b.position - a.position) * t;
}

st::int1 clip_triangle_near4(
	const fragment_input4 in_vert[3],
	const st::float4      in_clip[3],
	fragment_input4       out_vert[4],
	st::float4            out_clip[4])
{
	st::int1 in_mask[3];
	for (int i = 0; i < 3; i++)
		in_mask[i] = inside_near4(in_clip[i]);

	st::int1 isect_mask[3], keep_mask[3];
	for (int i = 0; i < 3; i++) {
		int p = (i + 2) % 3;
		isect_mask[i] = in_mask[i] ^ in_mask[p];
		keep_mask[i] = in_mask[i];
	}

	const int N = 6;
	st::int1 fired[N] = {
		isect_mask[0] & st::int1(1), keep_mask[0] & st::int1(1),
		isect_mask[1] & st::int1(1), keep_mask[1] & st::int1(1),
		isect_mask[2] & st::int1(1), keep_mask[2] & st::int1(1),
	};

	st::int1 slot[N];
	slot[0] = st::int1(0);
	for (int e = 1; e < N; e++)
		slot[e] = slot[e - 1] + fired[e - 1];

	st::int1 out_count = slot[N - 1] + fired[N - 1];

	fragment_input4 isect_vert[3];
	st::float4      isect_clip[3];
	for (int i = 0; i < 3; i++) {
		int p = (i + 2) % 3;
		st::float1 pzw = in_clip[p].z + in_clip[p].w;
		st::float1 czw = in_clip[i].z + in_clip[i].w;
		st::float1 t = pzw / (pzw - czw);
		lerp_vertex4(in_vert[p], in_clip[p], in_vert[i], in_clip[i], t,
			isect_vert[i], isect_clip[i]);
	}

	const st::float4* src_clip[N] = {
		&isect_clip[0], &in_clip[0],
		&isect_clip[1], &in_clip[1],
		&isect_clip[2], &in_clip[2],
	};
	const fragment_input4* src_vert[N] = {
		&isect_vert[0], &in_vert[0],
		&isect_vert[1], &in_vert[1],
		&isect_vert[2], &in_vert[2],
	};

	for (int s = 0; s < 4; s++) {
		st::int1        si(s);
		st::float4      c = {};
		fragment_input4 v = {};
		for (int e = 0; e < N; e++) {
			st::int1 w = (slot[e] == si) & (fired[e] != 0);
			c.x = st::select(w, src_clip[e]->x, c.x);
			c.y = st::select(w, src_clip[e]->y, c.y);
			c.z = st::select(w, src_clip[e]->z, c.z);
			c.w = st::select(w, src_clip[e]->w, c.w);
			v.uv.x = st::select(w, src_vert[e]->uv.x, v.uv.x);
			v.uv.y = st::select(w, src_vert[e]->uv.y, v.uv.y);
			v.uv.z = st::select(w, src_vert[e]->uv.z, v.uv.z);
			v.normal.x = st::select(w, src_vert[e]->normal.x, v.normal.x);
			v.normal.y = st::select(w, src_vert[e]->normal.y, v.normal.y);
			v.normal.z = st::select(w, src_vert[e]->normal.z, v.normal.z);
			v.position.x = st::select(w, src_vert[e]->position.x, v.position.x);
			v.position.y = st::select(w, src_vert[e]->position.y, v.position.y);
			v.position.z = st::select(w, src_vert[e]->position.z, v.position.z);
		}
		out_clip[s] = c;
		out_vert[s] = v;
	}

	return out_count;
}


void core::Initialize() {
	_draw_queue.resize(GFX_TRIANGLE_QUEUE_SIZE);
	_draw_queue_next = 0;
	_img_width = BUFFER_WIDTH;
	_img_height = BUFFER_HEIGHT;
}

void core::UnInitialize() {
	if (_img_buffer) delete[] _img_buffer;
	if (_depth_buffer) delete[] _depth_buffer;
	for (auto& v : _buffer) {
		for (auto& [size, ptr] : v) {
			delete ptr;
		}
	}
}

handle core::CreatePipeline(pipeline_desc pd) {
	handle h = reinterpret_cast<handle>(_desc.size());
	_desc.push_back(pd);
	return h;
}

void core::CreateImageBuffer(uint32_t width, uint32_t height) {
	_img_width = width;
	_img_height = height;
	if (_img_buffer) delete[] _img_buffer;
	_img_buffer = new color[width * height];
}

void core::CreateDepthBuffer(uint32_t width, uint32_t height) {
	_depth_width = width;
	_depth_height = height;
	_depth_buffer = new uint16_t[width * height];
}

void core::BindMaterialID(int mat_id) {
	_bound_material_id = mat_id;
}

void core::Clear(const color& c) {
	uint32_t color_val = *(uint32_t*)&c;
	__m128i color4 = _mm_set1_epi32(color_val);
	int total = _img_width * _img_height;
	int i = 0;
	for (; i <= total - 4; i += 4)
		_mm_storeu_si128((__m128i*) & _img_buffer[i], color4);
	for (; i < total; i++)
		_img_buffer[i] = c;
}

void core::ClearDepth(float depth) {
	uint16_t d = (uint16_t)((depth * 0.5f + 0.5f) * 65535.0f);
	int total = _depth_width * _depth_height;
	__m128i depth8 = _mm_set1_epi16((short)d);
	int i = 0;
	for (; i <= total - 8; i += 8)
		_mm_storeu_si128((__m128i*) & _depth_buffer[i], depth8);
	for (; i < total; i++)
		_depth_buffer[i] = d;
}

void core::Submit(const mesh& m) {
	if (!_camera->IsInFrustum(m.GetBoundingSphere().center, m.GetBoundingSphere().radius))
		return;

	auto& vertices = m.GetVertices();
	auto& indices = m.GetIndices();
	_bound_material_id = m.GetMaterialIndex();

	for (size_t i = 0; i < indices.size(); i += 3) {
		_draw_queue[_draw_queue_next++] = {
			vertices[indices[i]],
			vertices[indices[i + 1]],
			vertices[indices[i + 2]],
			_bound_material_id
		};
	}
}

void core::Submit(const vertex& v0, const vertex& v1, const vertex& v2) {
	_draw_queue[_draw_queue_next++] = { v0, v1, v2, _bound_material_id };
}

void core::DrawRect(int x, int y, int w, int h, color col) {
	for (int py = y; py < y + h; py++) {
		if (py < 0 || py >= (int)_img_height) continue;
		for (int px = x; px < x + w; px++) {
			if (px < 0 || px >= (int)_img_width) continue;
			_img_buffer[py * _img_width + px] = col;
		}
	}
}

void core::DrawGlyph(const texture& font_tex, const glyph& g, int x0, int y0, int sw, int sh, color col) {
	for (int py = 0; py < sh; py++)
	{
		for (int px = 0; px < sw; px++)
		{
			int sx = x0 + px;
			int sy = y0 + py;

			if (sx < 0 || sx >= (int)_img_width)  continue;
			if (sy < 0 || sy >= (int)_img_height) continue;

			float u = g.u0 + ((float)px / sw) * (g.u1 - g.u0);
			float v = g.v0 + ((float)py / sh) * (g.v1 - g.v0);

			int tx = std::clamp((int)(u * font_tex._width), 0, font_tex._width - 1);
			int ty = std::clamp((int)(v * font_tex._height), 0, font_tex._height - 1);

			uint8_t glyphAlpha = font_tex._data[ty * font_tex._width + tx];

			if (glyphAlpha <= 32)
				continue;

			uint8_t finalAlpha = (uint8_t)((glyphAlpha * col.a) / 255);

			color& dst = _img_buffer[sy * _img_width + sx];

			float srcA = finalAlpha / 255.0f;
			float dstA = dst.a / 255.0f;
			float outA = srcA + dstA * (1.0f - srcA);

			if (outA == 0.0f)
				continue;

			dst.r = (uint8_t)((col.r * srcA + dst.r * dstA * (1.0f - srcA)) / outA);
			dst.g = (uint8_t)((col.g * srcA + dst.g * dstA * (1.0f - srcA)) / outA);
			dst.b = (uint8_t)((col.b * srcA + dst.b * dstA * (1.0f - srcA)) / outA);
			dst.a = (uint8_t)(outA * 255.0f);
		}
	}
}

void core::DrawText(const std::string& text, const font& f, const texture& font_tex, int screenX, int screenY, color col, float scale, color outlineColor, int outlineThickness)
{
	int cursor = screenX;

	int baseline = screenY + (int)(f.GetAscent() * scale);
	int lineHeight = (int)(f.GetLineHeight() * scale);

	for (char c : text)
	{
		if (c == '\n')
		{
			cursor = screenX;
			baseline += lineHeight;
			continue;
		}

		if (c < 32 || c > 126)
			continue;

		const glyph& g = f.GetGlyph(c);

		int x0 = cursor + (int)(g.bearingX * scale);
		int y0 = baseline - (int)(g.bearingY * scale);
		int sw = (int)(g.width * scale);
		int sh = (int)(g.height * scale);

		if (outlineThickness > 0)
		{
			for (int oy = -outlineThickness; oy <= outlineThickness; oy++)
			{
				for (int ox = -outlineThickness; ox <= outlineThickness; ox++)
				{
					if (ox == 0 && oy == 0)
						continue;

					if (ox * ox + oy * oy > outlineThickness * outlineThickness)
						continue;

					DrawGlyph(font_tex, g, x0 + ox, y0 + oy, sw, sh, outlineColor);
				}
			}
		}

		DrawGlyph(font_tex, g, x0, y0, sw, sh, col);

		cursor += (int)(g.advance * scale);
	}
}

void core::Draw() {
	_bound_pipeline->Draw();
}

float core::GetDelta() {
	using clock = std::chrono::high_resolution_clock;
	using seconds = std::chrono::duration<float>;
	static auto lastTime = clock::now();
	static bool firstCall = true;

	auto currentTime = clock::now();

	if (firstCall) {
		firstCall = false;
		lastTime = currentTime;
		return 0.0f;
	}

	seconds delta = currentTime - lastTime;
	lastTime = currentTime;
	return delta.count();
}

} // namespace core
