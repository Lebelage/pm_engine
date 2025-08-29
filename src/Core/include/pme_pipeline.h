#pragma once
#include <string>
namespace pme
{
    class PmePipeline
    {
    public:
        PmePipeline(const std::string &vertFilePath, const std::string &fragFilePath);
    };

}