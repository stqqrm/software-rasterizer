#include "math.h"

namespace core {

#define TAN_TABLE_SIZE 4096
#define COS_SIN_TABLE_SIZE 4096
#define COS_SIN_TABLE_MASK (COS_SIN_TABLE_SIZE-1)

static float g_tan_table[TAN_TABLE_SIZE];
static float g_inv_tan_table[TAN_TABLE_SIZE];
static float g_cos_table[COS_SIN_TABLE_SIZE];
static float g_sin_table[COS_SIN_TABLE_SIZE];

static void _init_tan_table() {
	for (int i = 0; i < TAN_TABLE_SIZE; i++) {
		float angle = i * PI / 180.0f;
		g_tan_table[i] = tanf(angle);
	}
}

static void _init_inv_tan_table() {
	for (int i = 0; i < TAN_TABLE_SIZE; i++) {
		float angle = i * PI / 180.0f;
		g_inv_tan_table[i] = 1.0f / tanf(angle);
	}
}

static void _init_cos_sin_tables() {
	for (int i = 0; i < COS_SIN_TABLE_SIZE; i++) {
		float rad = (i * 360.0f / COS_SIN_TABLE_SIZE) * PI / 180.0f;
		g_cos_table[i] = cosf(rad);
		g_sin_table[i] = sinf(rad);
	}
}

void math_init() {
	_init_tan_table();
	_init_inv_tan_table();
	_init_cos_sin_tables();
}

float tan_deg(float angle) {
	while (angle < 0) angle += 360.0f;
	while (angle >= 360) angle -= 360.0f;
	int index = int(angle * 10) % TAN_TABLE_SIZE;
	return g_tan_table[index];
}

float inv_tan_deg(float angle) {
	while (angle < 0) angle += 360.0f;
	while (angle >= 360) angle -= 360.0f;
	int index = int(angle * 10) % TAN_TABLE_SIZE;
	return g_inv_tan_table[index];
}

float cos_deg(float angle) {
	int idx = (int)(angle * (COS_SIN_TABLE_SIZE / 360.0f));
	idx &= COS_SIN_TABLE_MASK;
	return g_cos_table[idx];
}

float sin_deg(float angle) {
	int idx = (int)(angle * (COS_SIN_TABLE_SIZE / 360.0f));
	idx &= COS_SIN_TABLE_MASK;
	return g_sin_table[idx];
}

float rsqrt(float number) {
	float x2 = number * 0.5f;
	float y = number;
	union {
		float f;
		uint32_t i;
	} u = { y };
	u.i = 0x5f3759df - (u.i >> 1);
	y = u.f;
	y = y * (1.5f - x2 * y * y);
	return y;
}

float edge(glm::vec3 a, glm::vec3 b, float x, float y) {
	return (x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x);
}

float edge(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

glm::mat4 look_at(glm::vec3 eye, glm::vec3 target, glm::vec3 up) {
	glm::vec3 f = glm::normalize(target - eye);
	glm::vec3 r = glm::normalize(cross(f, up));
	glm::vec3 u = cross(r, f);
	glm::mat4 result(0.0f);
	result[0][0] = r.x;  result[0][1] = u.x;  result[0][2] = -f.x;  result[0][3] = 0.0f;
	result[1][0] = r.y;  result[1][1] = u.y;  result[1][2] = -f.y;  result[1][3] = 0.0f;
	result[2][0] = r.z;  result[2][1] = u.z;  result[2][2] = -f.z;  result[2][3] = 0.0f;
	result[3][0] = -dot(r, eye);
	result[3][1] = -dot(u, eye);
	result[3][2] = dot(f, eye);
	result[3][3] = 1.0f;
	return result;
}

glm::mat4 perspective(float fov_rad, float aspect_ratio, float _near, float _far) {
	const float inv_tan = 1.0f / tanf(fov_rad * 0.5f);
	glm::mat4 result(0.0f);
	result[0][0] = inv_tan / aspect_ratio;
	result[1][1] = inv_tan;
	result[2][2] = -(_far + _near) / (_far - _near);
	result[2][3] = -1.0f;
	result[3][2] = -(2.0f * _far * _near) / (_far - _near);
	result[3][3] = 0.0f;
	return result;
}

} // namespace core
