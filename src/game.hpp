#pragma once

#define ENGINE_NAME "BudgetEngine"

#include "application.hpp"
#include "vulkan_window.hpp"

namespace voxelfield {
    class Game {
    public:
        static int Run(int numberOfArguments, char** arguments);
    };
}