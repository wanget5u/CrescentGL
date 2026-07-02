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
)

add_executable(${PROJECT_NAME} ${SOURCES} "include/Math/Common.h" "include/Math/Math.h" "include/Util/Util.h" "include/Util/FileOperations.h")

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
)