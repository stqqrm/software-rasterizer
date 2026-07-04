#include "app.h"

static std::vector<core::vertex> g_skybox_vertices = {
	// Right face (+X)
	{{ 1, -1, -1}, {-1, 0, 0}, { 1, -1, -1}},
	{{ 1,  1, -1}, {-1, 0, 0}, { 1,  1, -1}},
	{{ 1,  1,  1}, {-1, 0, 0}, { 1,  1,  1}},
	{{ 1, -1,  1}, {-1, 0, 0}, { 1, -1,  1}},
	// Left face (-X)
	{{-1, -1,  1}, { 1, 0, 0}, {-1, -1,  1}},
	{{-1,  1,  1}, { 1, 0, 0}, {-1,  1,  1}},
	{{-1,  1, -1}, { 1, 0, 0}, {-1,  1, -1}},
	{{-1, -1, -1}, { 1, 0, 0}, {-1, -1, -1}},
	// Top face (+Y)
	{{-1,  1, -1}, { 0, -1, 0}, {-1,  1, -1}},
	{{-1,  1,  1}, { 0, -1, 0}, {-1,  1,  1}},
	{{ 1,  1,  1}, { 0, -1, 0}, { 1,  1,  1}},
	{{ 1,  1, -1}, { 0, -1, 0}, { 1,  1, -1}},
	// Bottom face (-Y)
	{{-1, -1,  1}, { 0,  1, 0}, {-1, -1,  1}},
	{{-1, -1, -1}, { 0,  1, 0}, {-1, -1, -1}},
	{{ 1, -1, -1}, { 0,  1, 0}, { 1, -1, -1}},
	{{ 1, -1,  1}, { 0,  1, 0}, { 1, -1,  1}},
	// Front face (+Z)
	{{ 1, -1,  1}, { 0, 0, -1}, { 1, -1,  1}},
	{{ 1,  1,  1}, { 0, 0, -1}, { 1,  1,  1}},
	{{-1,  1,  1}, { 0, 0, -1}, {-1,  1,  1}},
	{{-1, -1,  1}, { 0, 0, -1}, {-1, -1,  1}},
	// Back face (-Z)
	{{-1, -1, -1}, { 0, 0,  1}, {-1, -1, -1}},
	{{-1,  1, -1}, { 0, 0,  1}, {-1,  1, -1}},
	{{ 1,  1, -1}, { 0, 0,  1}, { 1,  1, -1}},
	{{ 1, -1, -1}, { 0, 0,  1}, { 1, -1, -1}},
};

static std::vector<uint32_t> g_skybox_indices = {
	 0,  2,  1,  0,  3,  2,
	 4,  6,  5,  4,  7,  6,
	 8, 10,  9,  8, 11, 10,
	12, 14, 13, 12, 15, 14,
	16, 18, 17, 16, 19, 18,
	20, 22, 21, 20, 23, 22,
};

inline void gfx_draw(const core::model& m) {
	for (auto& mesh : m.GetMeshes()) {
		core::core::BindMaterialID(mesh.GetMaterialIndex());
		core::core::Submit(mesh);
	}
}

inline void gfx_draw(const std::vector<core::vertex>& vertices, const std::vector<uint32_t>& indices) {
	for (size_t i = 0; i < indices.size(); i += 3) {
		core::vertex v0 = vertices[indices[i]];
		core::vertex v1 = vertices[indices[i + 1]];
		core::vertex v2 = vertices[indices[i + 2]];
		core::core::Submit(v0, v1, v2);
	}
}

typedef enum toggle_t : int {
	NIL = 0,
	OFF = 1,
	ON  = 2,
} toggle_t;

toggle_t key_toggle(GLFWwindow* win, int k) {
	static bool s_key_down[1024] = {};
	static bool s_state[1024]    = {};
	static bool s_reported[1024] = {};
	if (glfwGetKey(win, k)) {
		s_key_down[k] = true;
		s_reported[k] = false;
	} else if (s_key_down[k]) {
		s_state[k]    = !s_state[k];
		s_key_down[k] = false;
		s_reported[k] = false;
	}
	if (!s_reported[k]) {
		s_reported[k] = true;
		return s_state[k] ? ON : OFF;
	}
	return NIL;
}

typedef bool switch_t;
switch_t key_switch(GLFWwindow* win, int k) {
	static bool s_key_down[1024] = {};
	static bool s_state[1024]    = {};
	if (glfwGetKey(win, k)) {
		s_key_down[k] = true;
	} else if (s_key_down[k]) {
		s_state[k]    = !s_state[k];
		s_key_down[k] = false;
	}
	return s_state[k];
}

app::app() : 
	_font(core::asset_manager::GetFont("fonts/arial.ttf").value()),
	_cubemap(core::asset_manager::GetTexture("cubemaps/sky.png").value()),
	_plant(core::asset_manager::GetModel("plant/plant1.fbx").value()) {
	glfwSetErrorCallback([](int code, const char* desc) {
		fprintf(stderr, "GLFW error %d: %s\n", code, desc ? desc : "no description");
	});
#if defined(GLFW_PLATFORM)
	// Force X11 (via XWayland if needed) — GLFW's native Wayland backend
	// currently has issues creating an OpenGL window in this project.
	glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
	if (!glfwInit()) {
		const char* desc = nullptr;
		int code = glfwGetError(&desc);
		fprintf(stderr, "glfwInit failed: %d %s\n", code, desc ? desc : "no description");
		throw std::runtime_error("glfwInit failed");
	}
}

app::~app() {
	glfwDestroyWindow(_win);
	glfwTerminate();
}

void app::CreateSurface() {
    static const float kQuadVerts[] = {
        -1.f, -1.f,  0.f, 1.f,
         1.f, -1.f,  1.f, 1.f,
         1.f,  1.f,  1.f, 0.f,
        -1.f,  1.f,  0.f, 0.f,
    };
    static const unsigned int kQuadIdx[] = { 0, 1, 2, 2, 3, 0 };

    GLuint vbo, ebo;
    glGenVertexArrays(1, &_gl_vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(_gl_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVerts), kQuadVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kQuadIdx), kQuadIdx, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glGenTextures(1, &_gl_texture);
    glBindTexture(GL_TEXTURE_2D, _gl_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, BUFFER_WIDTH, BUFFER_HEIGHT,
                 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    const char* vert_src = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUV;
        out vec2 vUV;
        void main() {
            vUV = aUV;
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";
    const char* frag_src = R"(
        #version 330 core
        in vec2 vUV;
        out vec4 fragColor;
        uniform sampler2D uTex;
        void main() {
            fragColor = texture(uTex, vUV);
        }
    )";
    auto compile = [](GLenum type, const char* src) -> GLuint {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[512]; glGetShaderInfoLog(s, 512, nullptr, log);
            fprintf(stderr, "Shader error: %s\n", log);
        }
        return s;
    };
    GLuint vs = compile(GL_VERTEX_SHADER,   vert_src);
    GLuint fs = compile(GL_FRAGMENT_SHADER, frag_src);
    _gl_shader = glCreateProgram();
    glAttachShader(_gl_shader, vs);
    glAttachShader(_gl_shader, fs);
    glLinkProgram(_gl_shader);
    GLint link_ok; glGetProgramiv(_gl_shader, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        char log[512]; glGetProgramInfoLog(_gl_shader, 512, nullptr, log);
        fprintf(stderr, "Shader link error: %s\n", log);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void app::RenderSurface() {
    glBindTexture(GL_TEXTURE_2D, _gl_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0, BUFFER_WIDTH, BUFFER_HEIGHT,
                    GL_BGRA, GL_UNSIGNED_BYTE, _pixel_buffer);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(_gl_shader);
    glUniform1i(glGetUniformLocation(_gl_shader, "uTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _gl_texture);
    glBindVertexArray(_gl_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glfwSwapBuffers(_win);
}

core::model _plant2;
core::model _plant3;
core::model _plant4;
core::model _plant5;

int app::Run(const std::string& title, int w, int h) {
	// Configure OpenGL context BEFORE creating the window.
	// Do NOT set GLFW_CLIENT_API to GLFW_NO_API — that disables the GL context.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_win = glfwCreateWindow(APP_WIDTH, APP_HEIGHT, title.c_str(), nullptr, nullptr);
	if (!_win) {
		const char* desc = nullptr;
		int code = glfwGetError(&desc);
		fprintf(stderr, "glfwCreateWindow failed: %d %s\n", code, desc ? desc : "no description");
		return -1;
	}

	// Make the OpenGL context current on this thread — required before any GL calls.
	glfwMakeContextCurrent(_win);
	glfwSwapInterval(0); // disable vsync for max fps

	core::gl::LoadExtensions();

	glfwSetWindowUserPointer(_win, this);
	glfwSetCursorPosCallback(_win, (GLFWcursorposfun)_CursorPositionCallbackThunk);

	CreateSurface();

	_font_texture.Load(_font);
	core::core::WriteBuffer(&_cubemap, 1, 0);

	_plant.Rotate(glm::vec3(-90.0f, 0.0f, 0.0f));
	_plant.Translate(glm::vec3(5.0f, -2.0f, 0.0f));
	_plant2 = _plant; _plant2.Translate(glm::vec3(0.0f, 0.0f,  3.0f));
	_plant3 = _plant; _plant3.Translate(glm::vec3(0.0f, 0.0f,  6.0f));
	_plant4 = _plant; _plant4.Translate(glm::vec3(0.0f, 0.0f, -3.0f));
	_plant5 = _plant; _plant5.Translate(glm::vec3(0.0f, 0.0f, -6.0f));

	for (int i = 0; i < (int)_plant.GetMaterials().size(); i++)
		core::core::WriteBuffer(&_plant.GetMaterials()[i], 2, i);
	for (int i = 0; i < (int)_plant.GetTextures().size(); i++)
		core::core::WriteBuffer(&_plant.GetTextures()[i], 3, i + 1);

	_cam.SetProjectionMatrix(90.0f, float(BUFFER_WIDTH) / float(BUFFER_HEIGHT), 0.1f, 1000.0f);
	core::core::SetCamera(&_cam);
	core::core::CreateImageBuffer(BUFFER_WIDTH, BUFFER_HEIGHT);
	core::core::CreateDepthBuffer(BUFFER_WIDTH, BUFFER_HEIGHT);
	_cam.MoveAbsolute(glm::vec3(-75.f, 0.f, 0.f));

	core::pipeline_desc pd = {
		core::_default_vertex_shader4,
		core::_default_fragment_shader4,
		core::DEPTH_READ_WRITE,
		core::BLEND_NONE,
		core::CULL_BACK,
		core::FILL_SOLID,
		core::WINDING_CW
	};
	core::handle hpipeline = core::core::CreatePipeline(pd);
	(void)hpipeline;

	while (!glfwWindowShouldClose(_win)) {
		glfwPollEvents();
		float dt = core::core::GetDelta();
		OnUpdate(dt);
		OnRender(dt);
	}
	return 0;
}

void app::OnUpdate(float dt) {
	static float movement_speed = 10.f;
	if (glfwGetKey(_win, GLFW_KEY_LEFT_SHIFT))
		movement_speed = 100.0f;
	else
		movement_speed = 10.0f;

	if (glfwGetKey(_win, GLFW_KEY_W)) _cam.MoveRelative({ 0,  0, movement_speed *  dt });
	if (glfwGetKey(_win, GLFW_KEY_S)) _cam.MoveRelative({ 0,  0, movement_speed * -dt });
	if (glfwGetKey(_win, GLFW_KEY_D)) _cam.MoveRelative({ movement_speed * dt,  0, 0 });
	if (glfwGetKey(_win, GLFW_KEY_A)) _cam.MoveRelative({ movement_speed * -dt, 0, 0 });
	if (glfwGetKey(_win, GLFW_KEY_SPACE))        _cam.MoveRelative({ 0, movement_speed *  dt, 0 });
	if (glfwGetKey(_win, GLFW_KEY_LEFT_CONTROL)) _cam.MoveRelative({ 0, movement_speed * -dt, 0 });
	if (glfwGetKey(_win, GLFW_KEY_R)) { _cam.MoveAbsolute({ 0, 0, 0 }); _cam.RotateAbsolute({ 0, 0, 0 }); }

	toggle_t e = key_toggle(_win, GLFW_KEY_E);
	if (e == ON) {
		_capture_cursor = true;
		glfwSetInputMode(_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (e == OFF) {
		_capture_cursor = false;
		glfwSetInputMode(_win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (glfwGetKey(_win, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(_win, 1);

	const float sensitivity = 0.1f;
	_cam.RotateRelative({ -_mouse_dy * sensitivity, _mouse_dx * sensitivity, 0.0f });
	_mouse_dx = 0;
	_mouse_dy = 0;
}

using pipeline_default = core::pipeline<
	core::_default_vertex_shader4, core::_default_fragment_shader4,
	core::DEPTH_READ_WRITE, core::BLEND_NONE, core::CULL_BACK, core::FILL_SOLID, core::WINDING_CW>;
using pipeline_normals = core::pipeline<
	core::_default_vertex_shader4, core::_normals_fragment_shader4,
	core::DEPTH_READ_WRITE, core::BLEND_NONE, core::CULL_BACK, core::FILL_SOLID, core::WINDING_CW>;
using pipeline_skybox  = core::pipeline<
	core::_skybox_vertex_shader4, core::_skybox_fragment_shader4,
	core::DEPTH_READ_ONLY, core::BLEND_NONE, core::CULL_NONE, core::FILL_SOLID, core::WINDING_CW>;

void app::OnRender(float dt) {
	static std::string fps_str = "";
	static float delta = 0.0f;

	_timer.start();
	_cam.Update();

	glm::mat4 view                = core::core::GetCamera()->GetViewMatrix();
	glm::mat4 proj                = core::core::GetCamera()->GetProjectionMatrix();
	glm::mat4 view_no_translation = glm::mat4(glm::mat3(view));

	core::core::WriteBuffer(proj * view,                0, 0);
	core::core::WriteBuffer(proj * view_no_translation, 0, 1);

	st::float3 light(1.0f, 0.0f, 0.0f);
	core::core::WriteBuffer(st::normalize(light), 0, 2);

	core::core::Clear({ 0, 0, 0 });
	core::core::ClearDepth(1.0f);

	if (key_switch(_win, GLFW_KEY_M) == false) {
		gfx_draw(g_skybox_vertices, g_skybox_indices);
		core::core::BindPipeline<pipeline_skybox>();
		core::core::Draw();
	}

	gfx_draw(_plant);

	if (key_switch(_win, GLFW_KEY_N) == false) {
		core::core::BindPipeline<pipeline_default>();
		core::core::Draw();
	} else {
		core::core::BindPipeline<pipeline_normals>();
		core::core::Draw();
	}

	core::core::DrawText(fps_str, _font, _font_texture, 3, 3,
		core::color(200, 200, 200), 0.6f, core::color(30, 30, 30), 1);
	core::core::DrawText("Binds\nToggle normals: N\nToggle skybox: M\nToggle wireframe", _font, _font_texture, 3, 40,
		core::color(200, 200, 200), 0.35f, core::color(30, 30, 30), 1);

	_pixel_buffer = core::core::GetImageBuffer();
	RenderSurface();

	delta = (float)_timer.stop();
	static std::deque<double> fps_queue;
	fps_queue.push_back(1.0 / delta);
	if (fps_queue.size() > 60) fps_queue.pop_front();
	double fps_avg = 0.0;
	for (double s : fps_queue) fps_avg += s;
	fps_avg /= fps_queue.size();
	fps_str = "fps " + std::to_string((int)fps_avg);
}

void app::_CursorPositionCallbackThunk(GLFWwindow* win, double x, double y) {
	app* a = reinterpret_cast<app*>(glfwGetWindowUserPointer(win));
	if (!a) return;
	a->_CursorPositionCallback(win, x, y);
}

void app::_CursorPositionCallback(GLFWwindow* win, double x, double y) {
	static int last_x = 0, last_y = 0;
	if (!_capture_cursor) {
		last_x = 0;
		last_y = 0;
		return;
	}
	_mouse_dx += (int)(x - last_x);
	_mouse_dy += -(int)(y - last_y);
	last_x = (int)x;
	last_y = (int)y;
}
