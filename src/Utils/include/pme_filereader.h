#pragma once
#include <vector>
#include <fstream>

#define ENGINE_PATH "../"
namespace pme
{
    class FileReader
    {
    public:
        static std::vector<char> ReadFile(const std::string &fileName);
    };
}