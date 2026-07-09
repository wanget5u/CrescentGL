cmake_minimum_required(VERSION 3.20)
project(CrescentGL VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
        Source/main.cpp
        Source/glad.c
        Source/Core/Application.cpp
        Source/Core/Window.cpp
        Source/Core/Log.cpp
        Source/Core/Time.cpp
        Source/Asset/Loader.cpp
        Source/Render/Mesh.cpp
        Source/Render/Primitives/BoxMesh.cpp
        Source/Scene/DemoScene.cpp
        Source/Input/InputAction.cpp
        Source/Input/InputContext.cpp
        Source/Input/InputSystem.cpp
        Source/Render/Shader/Shader.cpp
        Source/Render/Material/Material.cpp
        Source/Util/FileOperations.cpp
        include/Asset/Loader.h
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
        include/Collection/DynamicList.h
        include/Collection/Collections.h
        include/Render/Material/Material.h
        include/Core/Random.h
        include/Collection/DynamicQueue.h
        include/Asset/Type/Type.h
        Source/Render/Texture/Texture.cpp
        include/Render/Texture/Texture.h
        Source/Asset/Registry.cpp
        include/Asset/Registry.h
        Source/Scene/Nodes3D/Camera3D.cpp
        include/Scene/Nodes3D/Camera3D.h
        Source/Scene/Tree.cpp
        include/Scene/Tree.h
        Source/Scene/Node.cpp
        include/Scene/Node.h
        Source/Scene/Nodes3D/Node3D.cpp
        include/Scene/Nodes3D/Node3D.h
        Source/Scene/Transform.cpp
        include/Scene/Transform.h
        include/Math/Quaternion/Quaternion.h
        Source/Scene/Nodes3D/Geometry/MeshInstance3D.cpp
        include/Scene/Nodes3D/Geometry/MeshInstance3D.h
        Source/Render/BatchRenderer.cpp
        include/Render/BatchRenderer.h
        Source/Scene/Nodes3D/Geometry/MultiMeshInstance3D.cpp
        include/Scene/Nodes3D/Geometry/MultiMeshInstance3D.h
        Source/Scene/Scene.cpp
        Source/Scene/Nodes3D/VisualInstance3D.cpp
        include/Scene/Nodes3D/VisualInstance3D.h
        Source/Scene/Nodes3D/Geometry/GeometryInstance3D.cpp
        include/Scene/Nodes3D/Geometry/GeometryInstance3D.h
        Source/Scene/Nodes3D/Light/Light3D.cpp
        include/Scene/Nodes3D/Light/Light3D.h
        Source/Scene/Nodes3D/Light/PointLight3D.cpp
        include/Scene/Nodes3D/Light/PointLight3D.h
        Source/Scene/Nodes3D/Geometry/InstancedVisual3D.cpp
        include/Scene/Nodes3D/Geometry/InstancedVisual3D.h
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