cmake_minimum_required(VERSION 3.12)
project(game)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILES "src/*.cpp" "src/*.hpp")

add_compile_definitions(VALIDATION_LAYERS_ENABLED VK_USE_PLATFORM_WIN32_KHR)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} src/file_reader.cpp src/file_reader.hpp)

# find_library(Vulkan REQUIRED)
set(Vulkan_LIBRARY $ENV{VULKAN_SDK}/Lib/vulkan-1.lib)
set(Vulkan_INCLUDE_DIR $ENV{VULKAN_SDK}/Include)

include_directories(${Vulkan_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARY})
