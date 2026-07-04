#pragma once
#include "inc.h"

namespace core {
	struct frustum_planes {
	    glm::vec4 planes[6]; // left, right, bottom, top, near, far
	};
	
	class camera {
	public:
	    camera(void);
	    camera(const camera& cam);
	    camera(camera&& cam) noexcept;
	
	    void SetProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane);
	    void MoveAbsolute(const glm::vec3& v);
	    void MoveRelative(const glm::vec3& v);
	    void RotateAbsolute(const glm::vec3& v);
	    void RotateRelative(const glm::vec3& v);
	    void Update(void);
	    bool IsInFrustum(const glm::vec3& center, float radius) const;
	    const frustum_planes& GetFrustum() const { return _frustum; }
	
	    inline const glm::mat4x4& GetViewMatrix(void) const { return _view_mat; }
	    inline const glm::mat4x4& GetProjectionMatrix(void) const { return _proj_mat; }
	
	    inline const glm::vec3& GetPositionVector(void) const { return _position; }
	    inline const glm::vec3& GetRotationVector(void) const { return _rotation; }
	
	    inline const glm::vec3& GetForwardVector(void) const { return _forward; }
	    inline const glm::vec3& GetDefaultForwardVector(void) const { return _default_forward; }
	    inline const glm::vec3& GetRightVector(void) const { return _right; }
	    inline const glm::vec3& GetDefaultRightVector(void) const { return _default_right; }
	    inline const glm::vec3& GetUpVector(void) const { return _up; }
	    inline const glm::vec3& GetDefaultUpVector(void) const { return _default_up; }
	
	    camera& operator=(camera&& cam) noexcept;
	
	private:
	    glm::mat4x4 _view_mat;
	    glm::mat4x4 _proj_mat;
	    glm::vec3 _position;
	    glm::vec3 _rotation; // Pitch, Yaw, Roll
	    glm::vec3 _forward;
	    glm::vec3 _right;
	    glm::vec3 _up;
	    float _fov;
	    float _aspectRatio;
	    float _near;
	    float _far;
	    const glm::vec3 _default_forward = { 0.0f, 0.0f, 1.0f };  // +Z
	    const glm::vec3 _default_right = { 1.0f, 0.0f, 0.0f };  // +X
	    const glm::vec3 _default_up = { 0.0f, 1.0f, 0.0f };  // +Y
	    frustum_planes _frustum;
	};
}
