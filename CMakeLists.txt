cmake_minimum_required(VERSION 3.20)
project(CrescentGL VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/main.cpp
    src/glad.c
    src/Core/Application.cpp
    src/Core/Window.cpp
    src/Input/InputAction.cpp
    src/Input/InputContext.cpp
    src/Input/InputSystem.cpp
    src/Shader/Shader.cpp
    include/Shader/Shader.h
    include/Math/Constants.h
    include/Math/Matrix/Matrix4x4.h
    include/Math/Vector/Vector3.h
    include/Math/Vector/Vector4.h
        include/Math/Vector/Vector2.h
)

add_executable(${PROJECT_NAME} ${SOURCES} "include/Math/Arithmetic.h" "include/Math/Exponential.h" "include/Math/Interpolation.h" "include/Math/Trigonometry.h" "include/Math/Math.h" "include/Util/Util.h" "include/Util/FileOperations.h" "include/Core/Time.h" "include/Input/InputCodes.h" "include/Input/InputAction.h" "include/Input/InputContext.h" "include/Input/InputSystem.h")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Dependencies/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Dependencies/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw3
    opengl32
    gdi32
    winmm
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /sdl)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
    "${CMAKE_CURRENT_BINARY_DIR}/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
    "${CMAKE_CURRENT_BINARY_DIR}/Assets"
)