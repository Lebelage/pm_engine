#include "Utils/include/pme_filereader.h"

std::vector<char> pme::FileReader::ReadFile(const std::string &fileName)
{
    std::string filePath = ENGINE_PATH + fileName;

    std::ifstream file{filePath, std::ios::ate | std::ios::binary};

    if(!file.is_open()){
        throw std::runtime_error("failed to open file: " + filePath);
    }

    size_t fileSize = (size_t) file.tellg();

    std::vector<char> buffer(fileSize);

    file.seekg(0);

    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}