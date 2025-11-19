cmake_minimum_required(VERSION 3.10)
project(opengl_project VERSION 0.1.0)

# Use modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically gather all source files in src/ and lib/
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/lib/*.cpp"
    "${CMAKE_SOURCE_DIR}/lib/*.c"
)

# Create executable from gathered sources
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include directories (so headers in src/ are visible)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Include directories located in /lib
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
)

# Find and link GLFW
find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# Find and link OpenGL
cmake_policy(SET CMP0072 NEW)
set(OPENGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# Find and link GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

# Find and link ASSIMP
find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)

# Find and link Freetype
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(opengl_project PRIVATE ${FREETYPE_LIBRARIES})

# Find and link JsonCPP
find_package(jsoncpp REQUIRED)
target_link_libraries(opengl_project PRIVATE jsoncpp_lib)

# Optional: On some systems, you may also need dl or m
if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl m)
endif()