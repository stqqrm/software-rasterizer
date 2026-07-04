#pragma once
#include "inc.h"
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265358979323846f

namespace core {

void math_init();
float tan_deg(float angle);
float inv_tan_deg(float angle);
float cos_deg(float angle);
float sin_deg(float angle);
float rsqrt(float number);
float edge(glm::vec3 a, glm::vec3 b, float x, float y);
float edge(glm::vec2 a, glm::vec2 b, glm::vec2 c);
glm::mat4 look_at(glm::vec3 eye, glm::vec3 target, glm::vec3 up);
glm::mat4 perspective(float fov_rad, float aspect_ratio, float _far, float _near);

} // namespace core
