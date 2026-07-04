#include "camera.h"
#include <algorithm>

namespace core { 
	camera::camera(void) :
	    _position(0.0f, 0.0f, 0.0f),
	    _rotation(0.0f, 0.0f, 0.0f),
	    _forward(_default_forward),
	    _right(_default_right),
	    _up(_default_up),
	    _fov(90.f),
	    _aspectRatio(16.0f / 9.0f),
	    _near(0.1f),
	    _far(1000.0f) {
	    Update();
	}
	
	camera::camera(const camera& cam) :
	    _position(cam._position),
	    _rotation(cam._rotation),
	    _forward(cam._forward),
	    _right(cam._right),
	    _up(cam._up),
	    _fov(cam._fov),
	    _aspectRatio(cam._aspectRatio),
	    _near(cam._near),
	    _far(cam._far) {
	    Update();
	}
	
	camera::camera(camera&& cam) noexcept
	    : _view_mat(std::move(cam._view_mat))
	    , _proj_mat(std::move(cam._proj_mat))
	    , _position(std::move(cam._position))
	    , _rotation(std::move(cam._rotation))
	    , _forward(std::move(cam._forward))
	    , _right(std::move(cam._right))
	    , _up(std::move(cam._up))
	    , _fov(cam._fov)
	    , _aspectRatio(cam._aspectRatio)
	    , _near(cam._near)
	    , _far(cam._far)
	{
	    // The default vectors are const, so no need to move them
	    // Leave source in a valid state
	    cam._position = glm::vec3(0.0f);
	    cam._rotation = glm::vec3(0.0f);
	    cam._forward = _default_forward;
	    cam._right = _default_right;
	    cam._up = _default_up;
	    cam._view_mat = glm::mat4(1.0f);
	    cam._proj_mat = glm::mat4(1.0f);
	}
	
	void camera::SetProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) {
	    _fov = fov;
	    _aspectRatio = aspectRatio;
	    _near = nearPlane;
	    _far = farPlane;
	    _proj_mat = glm::perspective(glm::radians(_fov), _aspectRatio, _near, _far);
	
	}
	
	void camera::MoveAbsolute(const glm::vec3& v) {
	    _position = v;
	}
	
	void camera::MoveRelative(const glm::vec3& v) {
	    glm::vec3 flat_forward = glm::normalize(glm::vec3(_forward.x, 0.0f, _forward.z));
	    glm::vec3 flat_right = glm::normalize(glm::vec3(_right.x, 0.0f, _right.z));
	
	    _position += flat_right * v.x;
	    _position += glm::vec3(0, 1, 0) * v.y;  // world up, not camera up
	    _position += flat_forward * v.z;
	}
	
	void camera::RotateAbsolute(const glm::vec3& v) {
	    _rotation = v;
	}
	
	void camera::RotateRelative(const glm::vec3& v) {
	    _rotation = _rotation + v;
	}
	
	void camera::Update(void) {
	    // Wrap yaw to [0, 360)
	    while (_rotation.y >= 360.0f) _rotation.y -= 360.0f;
	    while (_rotation.y < 0.0f)    _rotation.y += 360.0f;
	
	    _rotation.x = glm::clamp(_rotation.x, -89.9f, 89.9f);
	
	    _forward.x = cos_deg(_rotation.y) * cos_deg(_rotation.x);
	    _forward.y = -sin_deg(_rotation.x);
	    _forward.z = sin_deg(_rotation.y) * cos_deg(_rotation.x);
	    _forward = glm::normalize(_forward);
	
	    _right = glm::normalize(glm::cross(_forward, _default_up));
	    _up = glm::normalize(glm::cross(_right, _forward));
	
	    _view_mat = glm::lookAtRH(_position, _position + _forward, _default_up);
	
	    glm::mat4 vp = _proj_mat * _view_mat;
	
	    // GLM is column-major: mat[col][row]
	    _frustum.planes[0] = { vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0] }; // left
	    _frustum.planes[1] = { vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0] }; // right
	    _frustum.planes[2] = { vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1] }; // bottom
	    _frustum.planes[3] = { vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1] }; // top
	    _frustum.planes[4] = { vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2], vp[3][3] + vp[3][2] }; // near
	    _frustum.planes[5] = { vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2], vp[3][3] - vp[3][2] }; // far
	
	    for (int i = 0; i < 6; i++) {
	        float inv_len = rsqrt(
	            _frustum.planes[i].x * _frustum.planes[i].x +
	            _frustum.planes[i].y * _frustum.planes[i].y +
	            _frustum.planes[i].z * _frustum.planes[i].z);
	        _frustum.planes[i] *= inv_len;
	    }
	}
	
	bool camera::IsInFrustum(const glm::vec3& center, float radius) const {
	    for (int i = 0; i < 6; i++) {
	        const glm::vec4& p = _frustum.planes[i];
	        float dist = p.x * center.x + p.y * center.y + p.z * center.z + p.w;
	        if (dist < -radius)
	            return false;
	    }
	    return true;
	}
	
	camera& camera::operator=(camera&& cam) noexcept {
	    if (this != &cam) {
	        _view_mat = std::move(cam._view_mat);
	        _proj_mat = std::move(cam._proj_mat);
	        _position = std::move(cam._position);
	        _rotation = std::move(cam._rotation);
	        _forward = std::move(cam._forward);
	        _right = std::move(cam._right);
	        _up = std::move(cam._up);
	        _fov = cam._fov;
	        _aspectRatio = cam._aspectRatio;
	        _near = cam._near;
	        _far = cam._far;
	
	        // reset source
	        cam._position = glm::vec3(0.f);
	        cam._rotation = glm::vec3(0.f);
	        cam._forward = _default_forward;
	        cam._right = _default_right;
	        cam._up = _default_up;
	        cam._view_mat = glm::mat4(1.f);
	        cam._proj_mat = glm::mat4(1.f);
	    }
	    return *this;
	}
}
