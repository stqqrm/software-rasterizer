#pragma once
#include "inc.h"
#include "material.h"
#include "texture.h"
#include "camera.h"
#include "shader.h"
#include "mesh.h"

// 16:9

// #define BUFFER_WIDTH 2560
// #define BUFFER_HEIGHT 1440

#define BUFFER_WIDTH 1920
#define BUFFER_HEIGHT 1080

//#define BUFFER_WIDTH 1280
//#define BUFFER_HEIGHT 720

// #define BUFFER_WIDTH 640
// #define BUFFER_HEIGHT 360

// #define BUFFER_WIDTH 320
// #define BUFFER_HEIGHT 180

// #define BUFFER_WIDTH 160
// #define BUFFER_HEIGHT 90

// 4:3

// #define BUFFER_WIDTH 1280
// #define BUFFER_HEIGHT 960

// #define BUFFER_WIDTH 640
// #define BUFFER_HEIGHT 480

// #define BUFFER_WIDTH 320
// #define BUFFER_HEIGHT 240

// #define BUFFER_WIDTH 160
// #define BUFFER_HEIGHT 120

// #define BUFFER_WIDTH 80
// #define BUFFER_HEIGHT 60

// #define BUFFER_WIDTH 960
// #define BUFFER_HEIGHT 540


namespace core {

bool inside_near(const glm::vec4& v);

void lerp_vertex(
	const fragment_input& a, const glm::vec4& clip_a,
	const fragment_input& b, const glm::vec4& clip_b,
	float t,
	fragment_input& out_vert, glm::vec4& out_clip);

int clip_triangle_near(
	const fragment_input in_vert[3],
	const glm::vec4 in_clip[3],
	fragment_input out_vert[4],
	glm::vec4 out_clip[4]);


inline void masked_write_vert4(
	fragment_input4 out_vert[4], st::float4 out_clip[4],
	const st::int1& slot_mask_0,   // mask for lanes whose out_count == 0
	const st::int1& slot_mask_1,
	const st::int1& slot_mask_2,
	const st::int1& slot_mask_3,
	const fragment_input4& src_vert, const st::float4& src_clip)
{
	// For each output slot k, blend src into out[k] where slot_mask_k is set.
	auto blend_f1 = [](const st::float1& dst, const st::float1& src, const st::int1& mask) -> st::float1 {
		return st::select(mask, src, dst);
		};
	auto blend_clip = [&](st::float4& dst, const st::float4& src, const st::int1& mask) {
		dst.x = blend_f1(dst.x, src.x, mask);
		dst.y = blend_f1(dst.y, src.y, mask);
		dst.z = blend_f1(dst.z, src.z, mask);
		dst.w = blend_f1(dst.w, src.w, mask);
		};
	auto blend_vert = [&](fragment_input4& dst, const fragment_input4& src, const st::int1& mask) {
		dst.uv.x = blend_f1(dst.uv.x, src.uv.x, mask);
		dst.uv.y = blend_f1(dst.uv.y, src.uv.y, mask);
		dst.normal.x = blend_f1(dst.normal.x, src.normal.x, mask);
		dst.normal.y = blend_f1(dst.normal.y, src.normal.y, mask);
		dst.normal.z = blend_f1(dst.normal.z, src.normal.z, mask);
		};

	blend_clip(out_clip[0], src_clip, slot_mask_0);  blend_vert(out_vert[0], src_vert, slot_mask_0);
	blend_clip(out_clip[1], src_clip, slot_mask_1);  blend_vert(out_vert[1], src_vert, slot_mask_1);
	blend_clip(out_clip[2], src_clip, slot_mask_2);  blend_vert(out_vert[2], src_vert, slot_mask_2);
	blend_clip(out_clip[3], src_clip, slot_mask_3);  blend_vert(out_vert[3], src_vert, slot_mask_3);
}


st::int1 inside_near4(const st::float4& v);

void lerp_vertex4(
	const fragment_input4& a, const st::float4& clip_a,
	const fragment_input4& b, const st::float4& clip_b,
	st::float1 t,
	fragment_input4& out_vert, st::float4& out_clip);

st::int1 clip_triangle_near4(
	const fragment_input4 in_vert[3],
	const st::float4     in_clip[3],
	fragment_input4      out_vert[4],
	st::float4           out_clip[4]);

// triangles allowed
// #define GFX_TRIANGLE_QUEUE_SIZE 134217728
#define GFX_TRIANGLE_QUEUE_SIZE 262144

struct rasterizer_input4 {
	st::float4 clip_pos;
	st::float3 normal;
	st::float3 uv;
	st::int1 material_id;
};

struct triangle_cmd {
	vertex v0, v1, v2;
	int material_id;
};

typedef enum depth_mode : int {
	DEPTH_DISABLE		= (1 << 0),
	DEPTH_READ_ONLY		= (1 << 1),
	DEPTH_WRITE_ONLY	= (1 << 2),
	DEPTH_READ_WRITE	= (1 << 3),
};

typedef enum blend_mode : int {
	BLEND_NONE			= (1 << 4),
	BLEND_ALPHA			= (1 << 5),
	BLEND_ADDITIVE		= (1 << 6),
	BLEND_MULTIPLY		= (1 << 7),
};

typedef  enum cull_mode : int {
	CULL_NONE			= (1 << 8),
	CULL_BACK			= (1 << 9),
	CULL_FRONT			= (1 << 10),
};

typedef enum fill_mode : int {
	FILL_WIREFRAME		= (1 << 11),
	FILL_SOLID			= (1 << 12),
};

typedef enum winding_order : int {
	WINDING_CW			= (1 << 13),
	WINDING_CCW			= (1 << 14),
};

class core;

struct ipipeline {
	virtual void Draw() = 0;
	//virtual void Draw4() = 0;
	virtual ~ipipeline() = default;
};

// extern core& g_gfx;

struct pipeline_desc {
	vertex_shader_t		vs;
	fragment_shader_t	fs;
	depth_mode			dm;
	blend_mode			bm;
	cull_mode			cm;
	fill_mode			fm;
	winding_order		wo;
};

typedef void* handle;

class core {
private:
	template <vertex_shader_t VS, fragment_shader_t FS, depth_mode DM, blend_mode BM, cull_mode CM, fill_mode FM, winding_order WO>
	friend struct pipeline;
public:
	core() = delete;

	static void Initialize();
	static void UnInitialize();

	template<typename P>
	static void BindPipeline();
	
	static handle CreatePipeline(pipeline_desc pd);

	static void CreateImageBuffer(uint32_t width, uint32_t height);
	static void CreateDepthBuffer(uint32_t width, uint32_t height);

	template <typename T>
	static void WriteBuffer(const T& in, int col, int slot);
	template <typename T>
	static void ReadBuffer(T& out, int col, int slot);

	static void BindMaterialID(int mat_id);
	static int  GetBoundMaterialID() { return _bound_material_id; }

	static void Clear(const color& c);
	static void ClearDepth(float depth);
	static void DrawRect(int x, int y, int w, int h, color col);
	static void DrawGlyph(const texture& font_tex, const glyph& g, int x0, int y0, int sw, int sh, color col);
	static void DrawText(const std::string& text, const font& f, const texture& font_tex, int screenX, int screenY, color col, float scale, color outlineColor, int outlineThickness);
	static void Submit(const mesh& m);
	static void Submit(const vertex& v0, const vertex& v1, const vertex& v2);
	static void Draw();

	static float GetDelta();

	static void SetCamera(camera* cam) { _camera = cam; }
	static color* GetImageBuffer() { return _img_buffer; }
	static color** GetImageBufferPtr() { return &_img_buffer; }
	static uint16_t* GetDepthBuffer() { return _depth_buffer; }
	static camera* GetCamera() { return _camera; }
	static uint32_t GetWriteMask() { return _write_mask; }

protected:
	static int                          _bound_material_id;
	
	static color*						_img_buffer;
	static uint32_t                     _img_width;
	static uint32_t                     _img_height;
	
	static uint16_t*					_depth_buffer;
	static uint32_t                     _depth_width;
	static uint32_t                     _depth_height;

	static uint16_t*					_shadow_buffer;
	static uint32_t						_shadow_width;
	static uint32_t						_shadow_height;

	static camera*						_camera;
	static ipipeline*					_bound_pipeline;
	static std::vector<triangle_cmd>    _draw_queue;
	static uint64_t                     _draw_queue_next;
	static uint32_t						_write_mask;
	static std::vector<std::vector<
			std::pair<size_t ,void*>>> _buffer;

	static std::vector<pipeline_desc>	_desc;
	static handle						_pipeline_handle;
};

template<typename T>
inline void core::WriteBuffer(const T& in, int col, int slot) {
	if (_buffer.size() <= static_cast<size_t>(slot)) {
		_buffer.resize(slot + 1);
	}

	if (_buffer[slot].size() <= static_cast<size_t>(col)) {
		_buffer[slot].resize(col + 1, { 0, nullptr });
	}

	auto& cell = _buffer[slot][col];

	// Reallocate if needed
	if (cell.second == nullptr || cell.first < sizeof(T)) {
		if (cell.second != nullptr) {
			free(cell.second);
		}

		cell.second = malloc(sizeof(T));
		cell.first = sizeof(T);
	}

	memcpy(cell.second, &in, sizeof(T));
}

template<typename P>
void core::BindPipeline() {
	static P instance;
	_bound_pipeline = &instance;
}

template<typename T>
inline void core::ReadBuffer(T& out, int col, int slot) {
	auto& cell = _buffer[slot][col];
	memcpy(&out, cell.second, sizeof(T));
}

template <vertex_shader_t VS, fragment_shader_t FS, depth_mode DM, blend_mode BM, cull_mode CM, fill_mode FM, winding_order WO>
struct pipeline : ipipeline {
	void Draw() override;
	//void Draw4() override;
private:
	void _Rasterize(
		fragment_input& fi0, glm::vec4& c0,
		fragment_input& fi1, glm::vec4& c1,
		fragment_input& fi2, glm::vec4& c2);
	//void _RasterizeDepth4(
	//	fragment_input4& fi0, st::float4& c0,
	//	fragment_input4& fi1, st::float4& c1,
	//	fragment_input4& fi2, st::float4& c2);
	//void _Rasterize4(
	//	fragment_input4& fi0, st::float4& c0,
	//	fragment_input4& fi1, st::float4& c1,
	//	fragment_input4& fi2, st::float4& c2);
};

/*
template <vertex_shader_t VS, fragment_shader_t FS, depth_mode DM, blend_mode BM, cull_mode CM, fill_mode FM, winding_order WO>
void pipeline<VS, FS, DM, BM, CM, FM, WO>::Draw4() {
	// First, batch the draw queue into groups of 4 triangles
	struct triangle_job4 {
		triangle_cmd* cmd[4];
		int count;
	};

	std::vector<triangle_job4> jobs;
	jobs.reserve((core::_draw_queue_next + 3) / 4);

	uint64_t i = 0;
	for (; i < core::_draw_queue_next; ) {
		triangle_job4 job{};
		job.count = 0;
		for (int lane = 0; lane < 4 && i < core::_draw_queue_next; ++lane, ++i) {
			job.cmd[lane] = &core::_draw_queue[i];
			job.count++;
		}
		// Pad with the last valid command
		for (int lane = job.count; lane < 4; ++lane)
			job.cmd[lane] = job.cmd[job.count - 1];
		jobs.push_back(job);
	}
	core::_draw_queue_next = 0;

	// Helper to pack vertices from 4 triangles
	auto pack = [](
		const vertex* v0,
		const vertex* v1,
		const vertex* v2,
		const vertex* v3) -> vertex_input4 {
			vertex_input4 in;
			st::store(in.position.x, glm::value_ptr(v3->position));
			st::store(in.position.y, glm::value_ptr(v3->position));
			st::store(in.position.z, glm::value_ptr(v3->position));
			st::store(in.normal.x, glm::value_ptr(v3->normal));
			st::store(in.normal.y, glm::value_ptr(v3->normal));
			st::store(in.normal.z, glm::value_ptr(v3->normal));
			st::store(in.uv.x, glm::value_ptr(v3->uv));
			st::store(in.uv.y, glm::value_ptr(v3->uv));
			st::store(in.uv.z, glm::value_ptr(v3->uv));
			return in;
		};

	// Process each batch of 4 triangles
	for (auto& job : jobs) {
		// Pack vertices from all 4 triangles in the batch
		// Each vi[n] contains the nth vertex from all 4 triangles
		auto vi0 = pack(&job.cmd[0]->v0, &job.cmd[1]->v0, &job.cmd[2]->v0, &job.cmd[3]->v0);
		auto vi1 = pack(&job.cmd[0]->v1, &job.cmd[1]->v1, &job.cmd[2]->v1, &job.cmd[3]->v1);
		auto vi2 = pack(&job.cmd[0]->v2, &job.cmd[1]->v2, &job.cmd[2]->v2, &job.cmd[3]->v2);

		// Clip space positions (each cp[n] contains 4 values, one per triangle)
		st::float4 cp0, cp1, cp2;

		// Vertex shader processes all 4 triangles simultaneously
		auto vs0 = VS(vi0, cp0);
		auto vs1 = VS(vi1, cp1);
		auto vs2 = VS(vi2, cp2);

		// Set material for first triangle (others will be handled per-lane)
		core::_bound_material_id = job.cmd[0]->material_id;
		core::_write_mask = 0xF;  // All lanes active

		// Now we have 4 triangles processed in parallel
		// Each SIMD lane contains data for one complete triangle

		// Rearrange for winding order
		fragment_input4 in[3];
		st::float4 cp[3] = { cp0, cp1, cp2 };

		if constexpr (WO == WINDING_CW) {
			in[0] = vs0;
			in[1] = vs1;
			in[2] = vs2;
		}
		else { // WINDING_CCW
			in[0] = vs2;
			in[1] = vs1;
			in[2] = vs0;
		}

		// Perform near-plane clipping on all 4 triangles simultaneously
		fragment_input4 clipped_in[4];
		st::float4 clipped_cp[4];
		st::int1 count = clip_triangle_near4(in, cp, clipped_in, clipped_cp);

		// Rasterize the clipped triangles (still in SIMD)
		// Each lane may have a different number of resulting triangles (3 or 4)

		// First triangle (all lanes have at least 3 vertices after clipping)
		_Rasterize4(
			clipped_in[2], clipped_cp[2],
			clipped_in[1], clipped_cp[1],
			clipped_in[0], clipped_cp[0]);
		
		//// Some lanes may have a fourth vertex (count == 4 for those lanes)
		st::any(count == 4, [&](int lane) {
			_Rasterize4(
				clipped_in[3], clipped_cp[3],
				clipped_in[2], clipped_cp[2],
				clipped_in[0], clipped_cp[0]);
		});
	}

	core::_write_mask = 0xF;

}

template<vertex_shader_t VS, fragment_shader_t FS, depth_mode DM, blend_mode BM, cull_mode CM, fill_mode FM, winding_order WO>
inline void pipeline<VS, FS, DM, BM, CM, FM, WO>::_RasterizeDepth4(
	fragment_input4& fi0, st::float4& c0,
	fragment_input4& fi1, st::float4& c1,
	fragment_input4& fi2, st::float4& c2)
{

}

template<vertex_shader_t VS, fragment_shader_t FS, depth_mode DM, blend_mode BM, cull_mode CM, fill_mode FM, winding_order WO>
inline void pipeline<VS, FS, DM, BM, CM, FM, WO>::_Rasterize4(fragment_input4& fi0, st::float4& c0, fragment_input4& fi1, st::float4& c1, fragment_input4& fi2, st::float4& c2)
{

}
*/
/*
template <vertex_shader_t VS, fragment_shader_t FS, depth_mode DM, blend_mode BM, cull_mode CM, fill_mode FM, winding_order WO>
void pipeline<VS, FS, DM, BM, CM, FM, WO>::Draw4() {
	// First, batch the draw queue into groups of 4 triangles
	struct triangle_job4 {
		triangle_cmd* cmd[4];
		int count;
	};

	std::vector<triangle_job4> jobs;
	jobs.reserve((core::_draw_queue_next + 3) / 4);

	uint64_t i = 0;
	for (; i < core::_draw_queue_next; ) {
		triangle_job4 job{};
		job.count = 0;
		for (int lane = 0; lane < 4 && i < core::_draw_queue_next; ++lane, ++i) {
			job.cmd[lane] = &core::_draw_queue[i];
			job.count++;
		}
		// Pad with the last valid command
		for (int lane = job.count; lane < 4; ++lane)
			job.cmd[lane] = job.cmd[job.count - 1];
		jobs.push_back(job);
	}
	core::_draw_queue_next = 0;

	// Helper to pack vertices from 4 triangles
	auto pack = [](
		const vertex* v0,
		const vertex* v1,
		const vertex* v2,
		const vertex* v3) -> vertex_input4 {
			vertex_input4 in;
			st::store(in.position.x, v3->position.x, v2->position.x, v1->position.x, v0->position.x);
			st::store(in.position.y, v3->position.y, v2->position.y, v1->position.y, v0->position.y);
			st::store(in.position.z, v3->position.z, v2->position.z, v1->position.z, v0->position.z);
			st::store(in.normal.x, v3->normal.x, v2->normal.x, v1->normal.x, v0->normal.x);
			st::store(in.normal.y, v3->normal.y, v2->normal.y, v1->normal.y, v0->normal.y);
			st::store(in.normal.z, v3->normal.z, v2->normal.z, v1->normal.z, v0->normal.z);
			st::store(in.uv.x, v3->uv.x, v2->uv.x, v1->uv.x, v0->uv.x);
			st::store(in.uv.y, v3->uv.y, v2->uv.y, v1->uv.y, v0->uv.y);
			st::store(in.uv.z, v3->uv.z, v2->uv.z, v1->uv.z, v0->uv.z);
			return in;
		};

	// Process each batch of 4 triangles
	for (auto& job : jobs) {
		// Pack vertices from all 4 triangles in the batch
		// Each vi[n] contains the nth vertex from all 4 triangles
		auto vi0 = pack(&job.cmd[0]->v0, &job.cmd[1]->v0, &job.cmd[2]->v0, &job.cmd[3]->v0);
		auto vi1 = pack(&job.cmd[0]->v1, &job.cmd[1]->v1, &job.cmd[2]->v1, &job.cmd[3]->v1);
		auto vi2 = pack(&job.cmd[0]->v2, &job.cmd[1]->v2, &job.cmd[2]->v2, &job.cmd[3]->v2);

		// Clip space positions (each cp[n] contains 4 values, one per triangle)
		st::float4 cp0, cp1, cp2;

		// Vertex shader processes all 4 triangles simultaneously
		auto vs0 = VS(vi0, cp0);
		auto vs1 = VS(vi1, cp1);
		auto vs2 = VS(vi2, cp2);

		// Set material for first triangle (others will be handled per-lane)
		core::_bound_material_id = job.cmd[0]->material_id;
		core::_write_mask = 0xF;  // All lanes active

		// Now we have 4 triangles processed in parallel
		// Each SIMD lane contains data for one complete triangle

		// Rearrange for winding order
		fragment_input4 in[3];
		st::float4 cp[3] = { cp0, cp1, cp2 };

		if constexpr (WO == WINDING_CW) {
			in[0] = vs0;
			in[1] = vs1;
			in[2] = vs2;
		}
		else { // WINDING_CCW
			in[0] = vs2;
			in[1] = vs1;
			in[2] = vs0;
		}

		// Perform near-plane clipping on all 4 triangles simultaneously
		fragment_input4 clipped_in[4];
		// st::float4 clipped_cp[4];
		// st::int1 count = clip_triangle_near4(in, cp, clipped_in, clipped_cp);

		// Rasterize the clipped triangles (still in SIMD)
		// Each lane may have a different number of resulting triangles (3 or 4)

		// First triangle (all lanes have at least 3 vertices after clipping)
		_Rasterize4(
			clipped_in[2], clipped_cp[2],
			clipped_in[1], clipped_cp[1],
			clipped_in[0], clipped_cp[0]);

		// Some lanes may have a fourth vertex (count == 4 for those lanes)
		if (st::any(count == 4)) {
			_Rasterize4(
				clipped_in[3], clipped_cp[3],
				clipped_in[2], clipped_cp[2],
				clipped_in[0], clipped_cp[0]);
		}
	}

	core::_write_mask = 0xF;

}

*/
template <vertex_shader_t VS, fragment_shader_t FS, depth_mode DM, blend_mode BM, cull_mode CM, fill_mode FM, winding_order WO>
void pipeline<VS, FS, DM, BM, CM, FM, WO>::Draw() {
	struct triangle_job4 {
		triangle_cmd* cmd[4];
		int count;
	};

	std::vector<triangle_job4> jobs;
	jobs.reserve((core::_draw_queue_next + 3) / 4);

	uint64_t i = 0;
	for (; i < core::_draw_queue_next; ) {
		triangle_job4 job{};
		job.count = 0;
		for (int lane = 0; lane < 4 && i < core::_draw_queue_next; ++lane, ++i) {
			job.cmd[lane] = &core::_draw_queue[i];
			job.count++;
		}
		// Pad with the last valid command instead of job.cmd[0] which could be null
		for (int lane = job.count; lane < 4; ++lane)
			job.cmd[lane] = job.cmd[job.count - 1];
		jobs.push_back(job);
	}
	core::_draw_queue_next = 0;

	auto pack4 = [](
		const vertex* v0,
		const vertex* v1,
		const vertex* v2,
		const vertex* v3) -> vertex_input4
		{
			vertex_input4 in;
			in.position.x = _mm_set_ps(v3->position.x, v2->position.x, v1->position.x, v0->position.x);
			in.position.y = _mm_set_ps(v3->position.y, v2->position.y, v1->position.y, v0->position.y);
			in.position.z = _mm_set_ps(v3->position.z, v2->position.z, v1->position.z, v0->position.z);
			in.normal.x = _mm_set_ps(v3->normal.x, v2->normal.x, v1->normal.x, v0->normal.x);
			in.normal.y = _mm_set_ps(v3->normal.y, v2->normal.y, v1->normal.y, v0->normal.y);
			in.normal.z = _mm_set_ps(v3->normal.z, v2->normal.z, v1->normal.z, v0->normal.z);
			in.uv.x = _mm_set_ps(v3->uv.x, v2->uv.x, v1->uv.x, v0->uv.x);
			in.uv.y = _mm_set_ps(v3->uv.y, v2->uv.y, v1->uv.y, v0->uv.y);
			in.uv.z = _mm_set_ps(v3->uv.z, v2->uv.z, v1->uv.z, v0->uv.z);
			return in;
		};

	auto unpack = [](const fragment_input4& vs, int lane) -> fragment_input {
		alignas(16) float nx[4], ny[4], nz[4];
		alignas(16) float ux[4], uy[4], uz[4];
		_mm_store_ps(nx, static_cast<__m128>(vs.normal.x));
		_mm_store_ps(ny, static_cast<__m128>(vs.normal.y));
		_mm_store_ps(nz, static_cast<__m128>(vs.normal.z));
		_mm_store_ps(ux, static_cast<__m128>(vs.uv.x));
		_mm_store_ps(uy, static_cast<__m128>(vs.uv.y));
		_mm_store_ps(uz, static_cast<__m128>(vs.uv.z));
		fragment_input fi;
		fi.normal = { nx[lane], ny[lane], nz[lane] };
		fi.uv = { ux[lane], uy[lane], uz[lane] };
		return fi;
		};

	for (auto& job : jobs) {
		// Vertex input
		auto vi0 = pack4(&job.cmd[0]->v0, &job.cmd[1]->v0, &job.cmd[2]->v0, &job.cmd[3]->v0);
		auto vi1 = pack4(&job.cmd[0]->v1, &job.cmd[1]->v1, &job.cmd[2]->v1, &job.cmd[3]->v1);
		auto vi2 = pack4(&job.cmd[0]->v2, &job.cmd[1]->v2, &job.cmd[2]->v2, &job.cmd[3]->v2);

		// Clip space positions
		st::float4 cp0;
		st::float4 cp1;
		st::float4 cp2;

		auto vs0 = VS(vi0, cp0);
		auto vs1 = VS(vi1, cp1);
		auto vs2 = VS(vi2, cp2);

		for (int lane = 0; lane < job.count; ++lane) {
			core::_bound_material_id = job.cmd[lane]->material_id;
			core::_write_mask = (1 << lane);

			// Unpack clip space positions for this lane
			glm::vec4 cp[3];
			cp[0] = {
				((float*)&cp0.x)[lane],
				((float*)&cp0.y)[lane],
				((float*)&cp0.z)[lane],
				((float*)&cp0.w)[lane]
			};
			cp[1] = {
				((float*)&cp1.x)[lane],
				((float*)&cp1.y)[lane],
				((float*)&cp1.z)[lane],
				((float*)&cp1.w)[lane]
			};
			cp[2] = {
				((float*)&cp2.x)[lane],
				((float*)&cp2.y)[lane],
				((float*)&cp2.z)[lane],
				((float*)&cp2.w)[lane]
			};

			// Unpack fragment inputs with correct winding order
			fragment_input in[3];
			fragment_input clipped_in[4];
			glm::vec4 clipped_cp[4];

			if constexpr (WO == WINDING_CW) {
				in[0] = unpack(vs0, lane);
				in[1] = unpack(vs1, lane);
				in[2] = unpack(vs2, lane);

				int count = clip_triangle_near(in, cp, clipped_in, clipped_cp);
				if (count < 3) continue;

				_Rasterize(
					clipped_in[2], clipped_cp[2],
					clipped_in[1], clipped_cp[1],
					clipped_in[0], clipped_cp[0]);

				if (count == 4) {
					_Rasterize(
						clipped_in[3], clipped_cp[3],
						clipped_in[2], clipped_cp[2],
						clipped_in[0], clipped_cp[0]);
				}
			}
			else if constexpr (WO == WINDING_CCW) {
				in[0] = unpack(vs2, lane);
				in[1] = unpack(vs1, lane);
				in[2] = unpack(vs0, lane);

				int count = clip_triangle_near(in, cp, clipped_in, clipped_cp);
				if (count < 3) continue;

				_Rasterize(
					clipped_in[0], clipped_cp[0],
					clipped_in[1], clipped_cp[1],
					clipped_in[2], clipped_cp[2]);

				if (count == 4) {
					_Rasterize(
						clipped_in[0], clipped_cp[0],
						clipped_in[2], clipped_cp[2],
						clipped_in[3], clipped_cp[3]);
				}
			}
		}
	}

	core::_write_mask = 0xF;
}


template <vertex_shader_t VS, fragment_shader_t FS, depth_mode DM, blend_mode BM, cull_mode CM, fill_mode FM, winding_order WO>
void pipeline<VS, FS, DM, BM, CM, FM, WO>::_Rasterize(
	fragment_input& fi0, glm::vec4& c0,
	fragment_input& fi1, glm::vec4& c1,
	fragment_input& fi2, glm::vec4& c2) {
	// -------------------------------------------------------------------------
	// Clip-space -> NDC -> screen-space
	// -------------------------------------------------------------------------
	const float inv_w0 = 1.0f / c0.w;
	const float inv_w1 = 1.0f / c1.w;
	const float inv_w2 = 1.0f / c2.w;
	const glm::vec3 ndc0 = { c0.x * inv_w0, c0.y * inv_w0, c0.z * inv_w0 };
	const glm::vec3 ndc1 = { c1.x * inv_w1, c1.y * inv_w1, c1.z * inv_w1 };
	const glm::vec3 ndc2 = { c2.x * inv_w2, c2.y * inv_w2, c2.z * inv_w2 };

	const float W = (float)core::_img_width;
	const float H = (float)core::_img_height;
	const glm::vec2 s0 = { (ndc0.x * 0.5f + 0.5f) * W, (1.0f - (ndc0.y * 0.5f + 0.5f)) * H };
	const glm::vec2 s1 = { (ndc1.x * 0.5f + 0.5f) * W, (1.0f - (ndc1.y * 0.5f + 0.5f)) * H };
	const glm::vec2 s2 = { (ndc2.x * 0.5f + 0.5f) * W, (1.0f - (ndc2.y * 0.5f + 0.5f)) * H };

	// -------------------------------------------------------------------------
	// Bounding box + backface cull
	// -------------------------------------------------------------------------
	const int minX = (int)fmaxf(0.f, (float)(int)(fminf(s0.x, fminf(s1.x, s2.x))));
	const int maxX = (int)fminf(W - 1.f, (float)(int)(fmaxf(s0.x, fmaxf(s1.x, s2.x)) + 1));
	const int minY = (int)fmaxf(0.f, (float)(int)(fminf(s0.y, fminf(s1.y, s2.y))));
	const int maxY = (int)fminf(H - 1.f, (float)(int)(fmaxf(s0.y, fmaxf(s1.y, s2.y)) + 1));

	const float area = edge(s0, s1, s2);
	if (area >= 0.0f) return;
	const float invArea = 1.0f / area;

	// -------------------------------------------------------------------------
	// Edge function gradients (used for span clipping per row)
	// -------------------------------------------------------------------------
	const float dw0_dx = s2.y - s1.y, dw0_dy = -(s2.x - s1.x);
	const float dw1_dx = s0.y - s2.y, dw1_dy = -(s0.x - s2.x);
	const float dw2_dx = s1.y - s0.y, dw2_dy = -(s1.x - s0.x);

	constexpr bool depth_read = (DM == DEPTH_READ_ONLY || DM == DEPTH_READ_WRITE);
	constexpr bool depth_write = (DM == DEPTH_WRITE_ONLY || DM == DEPTH_READ_WRITE);
	constexpr bool is_skybox = (DM == DEPTH_READ_ONLY);

	// -------------------------------------------------------------------------
	// Pre-divide attributes by w (perspective-correct interpolation numerators)
	// -------------------------------------------------------------------------
	const float u0 = fi0.uv.x * inv_w0, u1 = fi1.uv.x * inv_w1, u2 = fi2.uv.x * inv_w2;
	const float v0 = fi0.uv.y * inv_w0, v1 = fi1.uv.y * inv_w1, v2 = fi2.uv.y * inv_w2;
	const float t0 = fi0.uv.z * inv_w0, t1 = fi1.uv.z * inv_w1, t2 = fi2.uv.z * inv_w2;

	float nx0 = 0, nx1 = 0, nx2 = 0;
	float ny0 = 0, ny1 = 0, ny2 = 0;
	float nz0 = 0, nz1 = 0, nz2 = 0;
	if constexpr (depth_read && !is_skybox) {
		nx0 = fi0.normal.x * inv_w0; nx1 = fi1.normal.x * inv_w1; nx2 = fi2.normal.x * inv_w2;
		ny0 = fi0.normal.y * inv_w0; ny1 = fi1.normal.y * inv_w1; ny2 = fi2.normal.y * inv_w2;
		nz0 = fi0.normal.z * inv_w0; nz1 = fi1.normal.z * inv_w1; nz2 = fi2.normal.z * inv_w2;
	}
	float z0 = 0, z1 = 0, z2 = 0;
	if constexpr (depth_read || depth_write) {
		z0 = ndc0.z * 0.5f + 0.5f;
		z1 = ndc1.z * 0.5f + 0.5f;
		z2 = ndc2.z * 0.5f + 0.5f;
	}

	// -------------------------------------------------------------------------
	// Screen-space attribute gradients  (attr = A·w0 + B·w1 + C·w2) / area
	// -------------------------------------------------------------------------
	auto grad = [&](float a0, float a1, float a2) -> std::pair<float, float> {
		return {
			(dw0_dx * a0 + dw1_dx * a1 + dw2_dx * a2) * invArea,  // d/dx
			(dw0_dy * a0 + dw1_dy * a1 + dw2_dy * a2) * invArea   // d/dy
		};
		};

	const auto [dudx, dudy] = grad(u0, u1, u2);
	const auto [dvdx, dvdy] = grad(v0, v1, v2);
	const auto [dtdx, dtdy] = grad(t0, t1, t2);
	const auto [diwdx, diwdy] = grad(inv_w0, inv_w1, inv_w2);

	float dzdx = 0, dzdy = 0;
	float dnxdx = 0, dnxdy = 0;
	float dnydx = 0, dnydy = 0;
	float dnzdx = 0, dnzdy = 0;
	if constexpr (depth_read || depth_write)  std::tie(dzdx, dzdy) = grad(z0, z1, z2);
	if constexpr (depth_read && !is_skybox) {
		std::tie(dnxdx, dnxdy) = grad(nx0, nx1, nx2);
		std::tie(dnydx, dnydy) = grad(ny0, ny1, ny2);
		std::tie(dnzdx, dnzdy) = grad(nz0, nz1, nz2);
	}

	// -------------------------------------------------------------------------
	// Seed row-start attribute values at pixel centre (minX+0.5, minY+0.5)
	// -------------------------------------------------------------------------
	const glm::vec2 startP = { minX + 0.5f, minY + 0.5f };
	float w0_row = edge(s1, s2, startP);
	float w1_row = edge(s2, s0, startP);
	float w2_row = edge(s0, s1, startP);

	const float b0 = w0_row * invArea;
	const float b1 = w1_row * invArea;
	const float b2 = w2_row * invArea;

	float u_row = b0 * u0 + b1 * u1 + b2 * u2;
	float v_row = b0 * v0 + b1 * v1 + b2 * v2;
	float t_row = b0 * t0 + b1 * t1 + b2 * t2;
	float iw_row = b0 * inv_w0 + b1 * inv_w1 + b2 * inv_w2;
	float z_row = 0, nx_row = 0, ny_row = 0, nz_row = 0;
	if constexpr (depth_read || depth_write)   z_row = b0 * z0 + b1 * z1 + b2 * z2;
	if constexpr (depth_read && !is_skybox) {
		nx_row = b0 * nx0 + b1 * nx1 + b2 * nx2;
		ny_row = b0 * ny0 + b1 * ny1 + b2 * ny2;
		nz_row = b0 * nz0 + b1 * nz1 + b2 * nz2;
	}

	// Lane offsets for 4-wide interpolation: lane i = base + i * gradient
	const st::float1 lanes = st::float1(0.f, 1.f, 2.f, 3.f);

	// -------------------------------------------------------------------------
	// Write one shaded lane into the framebuffer, applying the active blend mode
	// -------------------------------------------------------------------------
	auto write_pixel = [&](int idx, int lane, st::int1 result) {
		const uint32_t p = (uint32_t)result[lane];
		const uint8_t  pr = (p >> 16) & 0xFF;
		const uint8_t  pg = (p >> 8) & 0xFF;
		const uint8_t  pb = (p) & 0xFF;
		const uint8_t  pa = (p >> 24) & 0xFF;
		color& dst = core::_img_buffer[idx];
		if constexpr (BM == BLEND_NONE) {
			if (pa >= 128) dst = color(pr, pg, pb);
		}
		else if constexpr (BM == BLEND_ALPHA) {
			const float a = pa * (1.f / 255.f);
			dst.r = (uint8_t)(pr * a + dst.r * (1.f - a));
			dst.g = (uint8_t)(pg * a + dst.g * (1.f - a));
			dst.b = (uint8_t)(pb * a + dst.b * (1.f - a));
		}
		else if constexpr (BM == BLEND_ADDITIVE) {
			dst.r = (uint8_t)MIN(255, (int)dst.r + pr);
			dst.g = (uint8_t)MIN(255, (int)dst.g + pg);
			dst.b = (uint8_t)MIN(255, (int)dst.b + pb);
		}
		else if constexpr (BM == BLEND_MULTIPLY) {
			dst.r = (uint8_t)(dst.r * pr / 255);
			dst.g = (uint8_t)(dst.g * pg / 255);
			dst.b = (uint8_t)(dst.b * pb / 255);
		}
		};

	// =========================================================================
	// Main scanline loop
	// =========================================================================
	for (int y = minY; y <= maxY; y++) {
		const int rowIndex = y * (int)W;

		// -- Span clipping: find the x range that lies inside the triangle ----
		int span_start = minX, span_end = maxX;
		auto clip_span = [&](float w_row, float dw_dx) {
			if (fabsf(dw_dx) < 1e-6f) { if (w_row > 0) span_start = span_end + 1; return; }
			const float x_cross = minX - w_row / dw_dx;
			if (dw_dx > 0) span_end = (int)fminf((float)span_end, floorf(x_cross));
			else            span_start = (int)fmaxf((float)span_start, ceilf(x_cross));
			};
		clip_span(w0_row, dw0_dx);
		clip_span(w1_row, dw1_dx);
		clip_span(w2_row, dw2_dx);
		span_start = MAX(span_start, minX);
		span_end = MIN(span_end, maxX);

		if (span_start > span_end) goto next_row;
		{
			// Step all interpolants forward to span_start
			const int steps = span_start - minX;
			float u = u_row + dudx * steps;
			float v = v_row + dvdx * steps;
			float t = t_row + dtdx * steps;
			float iw = iw_row + diwdx * steps;
			float zf = 0, nx = 0, ny = 0, nz = 0;
			if constexpr (depth_read || depth_write) zf = z_row + dzdx * steps;
			if constexpr (depth_read && !is_skybox) {
				nx = nx_row + dnxdx * steps;
				ny = ny_row + dnydx * steps;
				nz = nz_row + dnzdx * steps;
			}

			int x = span_start;

			// -----------------------------------------------------------------
			// 4-wide SIMD loop
			// -----------------------------------------------------------------
			for (; x <= span_end - 3; x += 4) {
				// Broadcast base + spread across 4 lanes
				const st::float1 iw4 = st::float1(iw) + lanes * diwdx;

				// Newton-Raphson reciprocal: rcp = approx; rcp = rcp*(2 - iw*rcp)
				const st::float1 approx = _mm_rcp_ps(static_cast<__m128>(iw4));
				const st::float1 rcp = approx * (st::float1(2.f) - iw4 * approx);

				// Perspective-correct UVs
				fragment_input4 frag4;
				frag4.uv.x = (st::float1(u) + lanes * dudx) * rcp;
				frag4.uv.y = (st::float1(v) + lanes * dvdx) * rcp;
				frag4.uv.z = (st::float1(t) + lanes * dtdx) * rcp;
				frag4.normal.x = frag4.normal.y = frag4.normal.z = st::float1(0.f);
				frag4.position.x = frag4.position.y = frag4.position.z = st::float1(0.f);

				// -- Depth test -----------------------------------------------
				if constexpr (is_skybox) {
					int active = 0;
					for (int i = 0; i < 4; i++)
						if (core::_depth_buffer[rowIndex + x + i] == 0xFFFF)
							active |= (1 << i);
					core::_write_mask = active;
					if (!active) { u += dudx * 4; v += dvdx * 4; t += dtdx * 4; iw += diwdx * 4; continue; }
				}
				else if constexpr (depth_read || depth_write) {
					const st::float1 zf4 = st::float1(zf) + lanes * dzdx;
					const st::int1   z_ints = st::to_int1(zf4 * 65535.f);
					int active = 0;
					for (int i = 0; i < 4; i++) {
						const uint16_t zi = (uint16_t)z_ints[i];
						if constexpr (depth_read) { if (zi >= core::_depth_buffer[rowIndex + x + i]) continue; }
						if constexpr (depth_write) core::_depth_buffer[rowIndex + x + i] = zi;
						active |= (1 << i);
					}
					core::_write_mask = active;
					if (!active) {
						u += dudx * 4; v += dvdx * 4; t += dtdx * 4; iw += diwdx * 4; zf += dzdx * 4;
						if constexpr (depth_read && !is_skybox) { nx += dnxdx * 4; ny += dnydx * 4; nz += dnzdx * 4; }
						continue;
					}
					if constexpr (depth_read && !is_skybox) {
						frag4.normal.x = (st::float1(nx) + lanes * dnxdx) * rcp;
						frag4.normal.y = (st::float1(ny) + lanes * dnydx) * rcp;
						frag4.normal.z = (st::float1(nz) + lanes * dnzdx) * rcp;
					}
				}
				else {
					core::_write_mask = 0xF;
				}

				// -- Shade + write --------------------------------------------
				const st::int1 result = FS(frag4);
				for (int i = 0; i < 4; i++) {
					if (core::_write_mask & (1 << i))
						write_pixel(rowIndex + x + i, i, result);
				}

				u += dudx * 4; v += dvdx * 4; t += dtdx * 4; iw += diwdx * 4;
				if constexpr (depth_read || depth_write)  zf += dzdx * 4;
				if constexpr (depth_read && !is_skybox) { nx += dnxdx * 4; ny += dnydx * 4; nz += dnzdx * 4; }
			}

			// -----------------------------------------------------------------
			// Scalar tail (0–3 remaining pixels)
			// -----------------------------------------------------------------

			for (; x <= span_end; x++) {
				const int idx = rowIndex + x;

				if constexpr (is_skybox) {
					if (core::_depth_buffer[idx] != 0xFFFF) goto next_scalar;
				}
				else if constexpr (depth_read || depth_write) {
					const uint16_t zi = (uint16_t)(zf * 65535.f);
					if constexpr (depth_read) { if (zi >= core::_depth_buffer[idx]) goto next_scalar; }
					if constexpr (depth_write) core::_depth_buffer[idx] = zi;
				}

				{
					// Scalar Newton-Raphson reciprocal
					float rcp = _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ss(iw)));
					rcp = rcp * (2.f - iw * rcp);

					fragment_input4 frag4;
					frag4.uv.x = st::float1(u * rcp);
					frag4.uv.y = st::float1(v * rcp);
					frag4.uv.z = st::float1(t * rcp);
					frag4.normal.x = frag4.normal.y = frag4.normal.z = st::float1(0.f);
					frag4.position.x = frag4.position.y = frag4.position.z = st::float1(0.f);
					if constexpr (depth_read && !is_skybox) {
						frag4.normal.x = st::float1(nx * rcp);
						frag4.normal.y = st::float1(ny * rcp);
						frag4.normal.z = st::float1(nz * rcp);
					}
					core::_write_mask = 1;
					write_pixel(idx, 0, FS(frag4));
				}

			next_scalar:
				u += dudx; v += dvdx; t += dtdx; iw += diwdx;
				if constexpr (depth_read || depth_write)  zf += dzdx;
				if constexpr (depth_read && !is_skybox) { nx += dnxdx; ny += dnydx; nz += dnzdx; }
			}
		}

	next_row:
		w0_row += dw0_dy; w1_row += dw1_dy; w2_row += dw2_dy;
		u_row += dudy;   v_row += dvdy;   t_row += dtdy;   iw_row += diwdy;
		if constexpr (depth_read || depth_write)  z_row += dzdy;
		if constexpr (depth_read && !is_skybox) { nx_row += dnxdy; ny_row += dnydy; nz_row += dnzdy; }
	}
}


} // namespace core
