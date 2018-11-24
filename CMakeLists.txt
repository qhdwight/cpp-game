cmake_minimum_required(VERSION 3.12)
project(game)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES game.cpp window.cpp window.hpp type_definitions.hpp windows_definitions.hpp logger.cpp logger.hpp game.hpp application.cpp application.hpp vulkan_window.cpp vulkan_window.hpp string_util.cpp string_util.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

find_library(Vulkan REQUIRED)

include_directories(${Vulkan_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARY})
