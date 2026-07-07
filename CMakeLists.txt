cmake_minimum_required(VERSION 3.20)
project(CrescentGL VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/main.cpp
    src/glad.c
    src/Core/Application.cpp
    src/Core/Window.cpp
    src/Core/Log.cpp
    src/Core/Time.cpp
    src/Asset/AssetLoader.cpp
    src/Render/Mesh.cpp
    src/Render/Primitives/BoxMesh.cpp
    src/Scene/DemoScene.cpp
    src/Input/InputAction.cpp
    src/Input/InputContext.cpp
    src/Input/InputSystem.cpp
    src/Render/Shader/Shader.cpp
    src/Render/Material/Material.cpp
    src/Util/FileOperations.cpp
    include/Asset/AssetLoader.h
    include/Core/Application.h
    include/Core/Core.h
    include/Core/Log.h
    include/Core/Time.h
    include/Core/Window.h
    include/Render/Mesh.h
    include/Render/Primitives/BoxMesh.h
    include/Scene/Scene.h
    include/Scene/DemoScene.h
    include/Render/Shader/Shader.h
    include/Math/Constants.h
    include/Math/Matrix/Matrix4x4.h
    include/Math/Vector/Vector2.h
    include/Math/Vector/Vector3.h
    include/Math/Vector/Vector4.h
    include/Math/Arithmetic.h
    include/Math/Exponential.h
    include/Math/Interpolation.h
    include/Math/Trigonometry.h
    include/Math/Math.h
    include/Util/Util.h
    include/Util/FileOperations.h
    include/Input/InputCodes.h
    include/Input/InputAction.h
    include/Input/InputContext.h
    include/Input/InputSystem.h
    include/Collections/DynamicList.h
    include/Collections/Collections.h
    include/Render/Material/Material.h
    include/Core/Random.h
        include/Collections/DynamicQueue.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

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