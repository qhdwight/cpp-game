#include "file_reader.hpp"

namespace voxelfield::file {
    std::vector<char> ReadFile(const std::string& fileName) {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            logging::Log(logging::LogType::ERROR_LOG, util::Format("Could not open file with name %s", MAX_MESSAGE_LENGTH, fileName.c_str()));
        }
        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(std::ios::beg);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }
}