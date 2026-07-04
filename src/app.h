#pragma once
// opengl
#include <GL/gl.h>
#include "core/gl_ext.h"
#include <stdint.h>
#include <stdexcept>
#include <string>
#include <queue>
#include "core/core.h"
#include "core/model.h"
#include "core/asset_manager.h"
#include "core/components.h"
#include "core/timer.h"
#include <miniaudio/miniaudio.h>
#include <reactphysics3d/reactphysics3d.h>
// #pragma comment(lib, "reactphysics3d.lib")
#include <entt/entt.hpp>
#include "core/meshoptimizer/meshoptimizer.h"

#define APP_NAME "Software Rasterizer"
#define APP_WIDTH 1920
#define APP_HEIGHT 1080

class app {
public:
	app();
	~app();
	void CreateSurface();
	void RenderSurface();
    int Run(const std::string& title, int w, int h);
	void OnUpdate(float dt);
	void OnRender(float dt);
private:
	static void _CursorPositionCallbackThunk(GLFWwindow* window, double xpos, double ypos);
	void _CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
private:
	GLFWwindow* _win = nullptr;
	GLuint _gl_vao = 0;
	GLuint _gl_texture = 0;
	GLuint _gl_shader = 0;
	int _mouse_dx = 0, _mouse_dy = 0;
	bool _capture_cursor = false;
    void* _pixel_buffer = nullptr;
	entt::registry _reg;
	entt::entity _player;
	core::timer _timer;
	core::camera _cam;
	core::font& _font;
	core::texture _font_texture;
	core::texture& _cubemap;
	core::model& _plant;
};
