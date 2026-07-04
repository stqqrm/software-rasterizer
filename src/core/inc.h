#pragma once
// #define ST_AVX_256
#include "st/st.h"

#include <math.h>
#include <stdint.h>
#include <filesystem>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <unordered_map>
#include <string>
#include <optional>
#include <array>

#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#include "math.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// #pragma comment(lib, "assimp-vc143-mt.lib")

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// #define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
// #pragma comment(lib, "glfw3_mt.lib")
// #pragma comment(lib, "glfw3dll.lib")

#undef min
#undef max

#define MIN(a, b) ((a) < (b) ? (int)(a) : (int)(b))
#define MAX(a, b) ((a) > (b) ? (int)(a) : (int)(b))

namespace fs = std::filesystem;

namespace core {
	struct color {
		//uint8_t r, g, b, a;
		uint8_t b, g, r, a; // BGRA order for windows DIB compatibility
		color() : r(0), g(0), b(0), a(255) {}
		color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
		color(glm::vec4 c) : r((uint8_t)(c.x * 255)), g((uint8_t)(c.y * 255)), b((uint8_t)(c.z * 255)), a((uint8_t)(c.w * 255)) {}
		void operator=(const color& other) {
			r = other.r; g = other.g; b = other.b; a = other.a;
		}
		// * op
		color operator*(float s) const {
			return color(
				(uint8_t)MIN(255, (int)(r * s)),
				(uint8_t)MIN(255, (int)(g * s)),
				(uint8_t)MIN(255, (int)(b * s)),
				(uint8_t)MIN(255, (int)(a * s))
			);
		}
		//add op
		color operator+(const color& other) const {
			return color(
				(uint8_t)MIN(255, (int)r + other.r),
				(uint8_t)MIN(255, (int)g + other.g),
				(uint8_t)MIN(255, (int)b + other.b),
				(uint8_t)MIN(255, (int)a + other.a)
			);
		}
	};
	
	struct vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 uv;
	};
	
	typedef uint32_t index;
	
	struct fragment_input {
		glm::vec3 normal;
		glm::vec3 uv;
	};
}
