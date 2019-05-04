#include "game.hpp"

int main(int numberOfArguments, char** arguments) {
    return voxelfield::Game::Run(numberOfArguments, arguments);
}

namespace voxelfield {
    int Game::Run(int numberOfArguments, char** arguments) {
        const std::string gameName = "Voxelfield";
        Application application(gameName);
        window::VulkanWindow window(application, gameName);
        try {
            window.Open();
            window.Loop();
        } catch (const std::exception& exception) {
            logging::Log(logging::LogType::ERROR_LOG, exception.what());
            MessageBox(nullptr, exception.what(), gameName.c_str(), MB_ICONERROR);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}