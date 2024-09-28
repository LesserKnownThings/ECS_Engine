#pragma once

#include <fstream>
#include <string>

namespace LKT
{
    struct VoxelModel;

    class Voximport
    {
    public:
        static Voximport &Get();

        static bool Import(const std::string &path, VoxelModel& outData);

    private:
        static void ReadChunk(std::ifstream &stream, VoxelModel& outData);
        static void ExtractSize(std::ifstream &stream, VoxelModel& outData);
        static void ExtractVoxelData(std::ifstream &stream, VoxelModel& outData);
        static void ExtractColor(std::ifstream &stream, VoxelModel& outData);
    };
}