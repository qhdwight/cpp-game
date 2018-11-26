#include "game.hpp"

int main(int numberOfArguments, char** arguments) {
    return voxelfield::Game::Run(numberOfArguments, arguments);
}

namespace voxelfield {
    int Game::Run(int numberOfArguments, char** arguments) {
        const std::string gameName = "Voxelfield";
        Application application(gameName);
        window::VulkanWindow window(application, gameName);
        if (!window.Open())
            return EXIT_FAILURE;
        window.Loop();
        return EXIT_SUCCESS;
    }
}