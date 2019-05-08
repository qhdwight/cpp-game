#include "file_reader.hpp"

#include <filesystem>

namespace voxelfield::file {
    std::vector<char> ReadFile(const std::string& fileName) {
        std::ifstream file(fileName, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error(util::Format("Could not open file with name %s", MAX_MESSAGE_LENGTH, fileName.c_str()));
        }
        uintmax_t fileSize = std::filesystem::file_size(fileName);
        std::vector<char> buffer(fileSize);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }
}