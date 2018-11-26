cmake_minimum_required(VERSION 3.12)
project(game)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILES "src/*.cpp" "src/*.hpp")

add_compile_definitions(VALIDATION_LAYERS_ENABLED)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

find_library(Vulkan REQUIRED)

include_directories(${Vulkan_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARY})
