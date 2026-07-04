cmake_minimum_required(VERSION 3.25)
project(software-rasterizer
    VERSION 1.0
    LANGUAGES CXX
)

# Tell CMake to export compile_commands.json (required for clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        5c205738c191bcb0abc65c4febfa9bd25ff35234  # pinned commit, not master
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v3.13.2
)
FetchContent_Declare(
    reactphysics3d
    GIT_REPOSITORY https://github.com/DanielChappuis/reactphysics3d.git
    GIT_TAG        v0.10.2
)
FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(entt reactphysics3d miniaudio)

# stb's repo has stb_image.h at the root, but the code does
# #include <stb/stb_image.h> — create a "stb" folder that points at it.
set(stb_include_dir "${CMAKE_BINARY_DIR}/stb_include")
file(MAKE_DIRECTORY "${stb_include_dir}")
if(NOT EXISTS "${stb_include_dir}/stb")
    file(CREATE_LINK "${stb_SOURCE_DIR}" "${stb_include_dir}/stb" SYMBOLIC)
endif()

set(source_dir "${PROJECT_SOURCE_DIR}/src")
file(GLOB_RECURSE source_files CONFIGURE_DEPENDS "${source_dir}/*.cpp")

add_executable(software-rasterizer ${source_files})

target_include_directories(software-rasterizer PRIVATE
    ${source_dir}
    ${stb_include_dir}
    ${miniaudio_SOURCE_DIR}
)

target_link_libraries(software-rasterizer PRIVATE
    glfw
    glm::glm
    assimp
    Freetype::Freetype
    OpenGL::GL
    EnTT::EnTT
    reactphysics3d
)

# The default SIMD backend (st::backend::sse) uses SSE4.1 intrinsics
# (_mm_min_epi32, _mm_max_epi32, _mm_mullo_epi32, _mm_floor_ps, _mm_round_ps).
# The plain x86-64 baseline is SSE2 only, so these must be enabled explicitly
# or the intrinsics fail to compile ("target specific option mismatch").
target_compile_options(software-rasterizer PRIVATE -msse4.1)

# Set C++23 on this specific target (preferred over global settings)
target_compile_features(software-rasterizer PRIVATE cxx_std_23)

# Optional: be strict about the standard (don't fall back to an older one)
set_target_properties(software-rasterizer PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF        # disables GNU extensions; use -std=c++23 not -std=gnu++23
)
