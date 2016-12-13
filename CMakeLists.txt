cmake_minimum_required(VERSION 2.6)

project(immaterial)

include(win-cpp-deps.cmake/win-cpp-deps.cmake)

install_dep("https://bitbucket.org/wincppdeps/glfw.git")
install_dep("https://bitbucket.org/wincppdeps/sdl2.git")
install_dep("https://bitbucket.org/wincppdeps/glm.git")
install_dep("https://bitbucket.org/wincppdeps/bullet.git")
install_dep("https://bitbucket.org/wincppdeps/glextloader.git")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

find_package(OpenGL REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/common)

set(SRC_APP
    common/font.cpp
    common/shader.cpp
    input.cpp
    program.cpp
    sdl2-setup.cpp
    )

add_executable(immaterial ${SRC_APP})

target_link_libraries(immaterial
    ${SDL2_LIBRARY} Ole32 Oleaut32 Imm32 Winmm Version
    ${OPENGL_LIBRARIES}
)
