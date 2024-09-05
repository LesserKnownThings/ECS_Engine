#pragma once

#include <fstream>
#include <ostream>

namespace LKT
{
    class ISerializable
    {
    protected:
        virtual bool Serialize(std::ostream &outStream) const = 0;
        virtual bool Deserialize(std::ifstream &inStream) = 0;
    };
}