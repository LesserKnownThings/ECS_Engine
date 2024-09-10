#pragma once

#include <fstream>

namespace LKT
{
    class ISerializable
    {
    protected:
        virtual bool Serialize(std::ofstream &outStream) const = 0;
        virtual bool Deserialize(std::ifstream &inStream) = 0;
    };
}