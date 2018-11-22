#include "window.hpp"
#include "game.hpp"


int main(int numberOfArguments, char** arguments) {
    return voxelfield::Game::Run(numberOfArguments, arguments);
}

namespace voxelfield {
    int Game::Run(int numberOfArguments, char** arguments) {
        const std::string gameName = "Voxelfield";
        Application application(gameName);
        window::Window window(application, gameName);
        window.Open();
        return 0;
    }
}