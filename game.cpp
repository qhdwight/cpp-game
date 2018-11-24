#include "window.hpp"
#include "game.hpp"
#include "vulkan_window.hpp"


int main(int numberOfArguments, char** arguments) {
    return voxelfield::Game::Run(numberOfArguments, arguments);
}

namespace voxelfield {
    int Game::Run(int numberOfArguments, char** arguments) {
        const std::string gameName = "Voxelfield";
        Application application(gameName);
        window::VulkanWindow window(application, gameName);
        window.Open();
        return 0;
    }
}