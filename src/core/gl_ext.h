#pragma once
// GL/gl.h (as shipped by Mesa/Linux) only declares OpenGL 1.1 entry points.
// Everything from VAOs onward is a driver extension that must be resolved
// at runtime. GLFW already gives us a portable way to do that, so we load
// just the handful of functions this project actually calls.
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>

#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_ELEMENT_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_TEXTURE0
#define GL_TEXTURE0 0x84C0
#endif

namespace core::gl {

    using PFNGLGENVERTEXARRAYSPROC = void (*)(GLsizei, GLuint*);
    using PFNGLGENBUFFERSPROC = void (*)(GLsizei, GLuint*);
    using PFNGLBINDVERTEXARRAYPROC = void (*)(GLuint);
    using PFNGLBINDBUFFERPROC = void (*)(GLenum, GLuint);
    using PFNGLBUFFERDATAPROC = void (*)(GLenum, ptrdiff_t, const void*, GLenum);
    using PFNGLVERTEXATTRIBPOINTERPROC = void (*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
    using PFNGLENABLEVERTEXATTRIBARRAYPROC = void (*)(GLuint);
    using PFNGLCREATESHADERPROC = GLuint (*)(GLenum);
    using PFNGLSHADERSOURCEPROC = void (*)(GLuint, GLsizei, const GLchar* const*, const GLint*);
    using PFNGLCOMPILESHADERPROC = void (*)(GLuint);
    using PFNGLGETSHADERIVPROC = void (*)(GLuint, GLenum, GLint*);
    using PFNGLGETSHADERINFOLOGPROC = void (*)(GLuint, GLsizei, GLsizei*, GLchar*);
    using PFNGLCREATEPROGRAMPROC = GLuint (*)(void);
    using PFNGLATTACHSHADERPROC = void (*)(GLuint, GLuint);
    using PFNGLLINKPROGRAMPROC = void (*)(GLuint);
    using PFNGLDELETESHADERPROC = void (*)(GLuint);
    using PFNGLUSEPROGRAMPROC = void (*)(GLuint);
    using PFNGLGETUNIFORMLOCATIONPROC = GLint (*)(GLuint, const GLchar*);
    using PFNGLUNIFORM1IPROC = void (*)(GLint, GLint);
    using PFNGLACTIVETEXTUREPROC = void (*)(GLenum);
    using PFNGLGETPROGRAMIVPROC = void (*)(GLuint, GLenum, GLint*);
    using PFNGLGETPROGRAMINFOLOGPROC = void (*)(GLuint, GLsizei, GLsizei*, GLchar*);

    inline PFNGLGENVERTEXARRAYSPROC GenVertexArrays = nullptr;
    inline PFNGLGENBUFFERSPROC GenBuffers = nullptr;
    inline PFNGLBINDVERTEXARRAYPROC BindVertexArray = nullptr;
    inline PFNGLBINDBUFFERPROC BindBuffer = nullptr;
    inline PFNGLBUFFERDATAPROC BufferData = nullptr;
    inline PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer = nullptr;
    inline PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray = nullptr;
    inline PFNGLCREATESHADERPROC CreateShader = nullptr;
    inline PFNGLSHADERSOURCEPROC ShaderSource = nullptr;
    inline PFNGLCOMPILESHADERPROC CompileShader = nullptr;
    inline PFNGLGETSHADERIVPROC GetShaderiv = nullptr;
    inline PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog = nullptr;
    inline PFNGLCREATEPROGRAMPROC CreateProgram = nullptr;
    inline PFNGLATTACHSHADERPROC AttachShader = nullptr;
    inline PFNGLLINKPROGRAMPROC LinkProgram = nullptr;
    inline PFNGLDELETESHADERPROC DeleteShader = nullptr;
    inline PFNGLUSEPROGRAMPROC UseProgram = nullptr;
    inline PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation = nullptr;
    inline PFNGLUNIFORM1IPROC Uniform1i = nullptr;
    inline PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;
    inline PFNGLGETPROGRAMIVPROC GetProgramiv = nullptr;
    inline PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog = nullptr;

    // Call once, after glfwMakeContextCurrent(). Requires a current GL context.
    inline void LoadExtensions() {
        GenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(glfwGetProcAddress("glGenVertexArrays"));
        GenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(glfwGetProcAddress("glGenBuffers"));
        BindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(glfwGetProcAddress("glBindVertexArray"));
        BindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(glfwGetProcAddress("glBindBuffer"));
        BufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(glfwGetProcAddress("glBufferData"));
        VertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(glfwGetProcAddress("glVertexAttribPointer"));
        EnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(glfwGetProcAddress("glEnableVertexAttribArray"));
        CreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(glfwGetProcAddress("glCreateShader"));
        ShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(glfwGetProcAddress("glShaderSource"));
        CompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(glfwGetProcAddress("glCompileShader"));
        GetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(glfwGetProcAddress("glGetShaderiv"));
        GetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(glfwGetProcAddress("glGetShaderInfoLog"));
        CreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(glfwGetProcAddress("glCreateProgram"));
        AttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(glfwGetProcAddress("glAttachShader"));
        LinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(glfwGetProcAddress("glLinkProgram"));
        DeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(glfwGetProcAddress("glDeleteShader"));
        UseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(glfwGetProcAddress("glUseProgram"));
        GetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(glfwGetProcAddress("glGetUniformLocation"));
        Uniform1i = reinterpret_cast<PFNGLUNIFORM1IPROC>(glfwGetProcAddress("glUniform1i"));
        ActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(glfwGetProcAddress("glActiveTexture"));
        GetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(glfwGetProcAddress("glGetProgramiv"));
        GetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(glfwGetProcAddress("glGetProgramInfoLog"));

        struct entry { const char* name; void* ptr; };
        entry entries[] = {
            {"glGenVertexArrays", (void*)GenVertexArrays},
            {"glGenBuffers", (void*)GenBuffers},
            {"glBindVertexArray", (void*)BindVertexArray},
            {"glBindBuffer", (void*)BindBuffer},
            {"glBufferData", (void*)BufferData},
            {"glVertexAttribPointer", (void*)VertexAttribPointer},
            {"glEnableVertexAttribArray", (void*)EnableVertexAttribArray},
            {"glCreateShader", (void*)CreateShader},
            {"glShaderSource", (void*)ShaderSource},
            {"glCompileShader", (void*)CompileShader},
            {"glGetShaderiv", (void*)GetShaderiv},
            {"glGetShaderInfoLog", (void*)GetShaderInfoLog},
            {"glCreateProgram", (void*)CreateProgram},
            {"glAttachShader", (void*)AttachShader},
            {"glLinkProgram", (void*)LinkProgram},
            {"glDeleteShader", (void*)DeleteShader},
            {"glUseProgram", (void*)UseProgram},
            {"glGetUniformLocation", (void*)GetUniformLocation},
            {"glUniform1i", (void*)Uniform1i},
            {"glActiveTexture", (void*)ActiveTexture},
            {"glGetProgramiv", (void*)GetProgramiv},
            {"glGetProgramInfoLog", (void*)GetProgramInfoLog},
        };
        bool missing = false;
        for (auto& e : entries) {
            if (!e.ptr) {
                fprintf(stderr, "Failed to resolve GL function: %s (no current GL context, or driver doesn't expose it)\n", e.name);
                missing = true;
            }
        }
        if (missing) {
            fprintf(stderr, "One or more required OpenGL entry points are missing — aborting before we segfault on a null call.\n");
            std::abort();
        }
    }

} // namespace core::gl

// Route the plain gl* call sites in app.cpp to the loaded pointers without
// having to rewrite every call site.
#define glGenVertexArrays core::gl::GenVertexArrays
#define glGenBuffers core::gl::GenBuffers
#define glBindVertexArray core::gl::BindVertexArray
#define glBindBuffer core::gl::BindBuffer
#define glBufferData core::gl::BufferData
#define glVertexAttribPointer core::gl::VertexAttribPointer
#define glEnableVertexAttribArray core::gl::EnableVertexAttribArray
#define glCreateShader core::gl::CreateShader
#define glShaderSource core::gl::ShaderSource
#define glCompileShader core::gl::CompileShader
#define glGetShaderiv core::gl::GetShaderiv
#define glGetShaderInfoLog core::gl::GetShaderInfoLog
#define glCreateProgram core::gl::CreateProgram
#define glAttachShader core::gl::AttachShader
#define glLinkProgram core::gl::LinkProgram
#define glDeleteShader core::gl::DeleteShader
#define glUseProgram core::gl::UseProgram
#define glGetUniformLocation core::gl::GetUniformLocation
#define glUniform1i core::gl::Uniform1i
#define glActiveTexture core::gl::ActiveTexture
#define glGetProgramiv core::gl::GetProgramiv
#define glGetProgramInfoLog core::gl::GetProgramInfoLog
