#include "Voximport.h"
#include "VoxelModel.h"

#include "glm/glm.hpp"

#include <cstring>
#include <iostream>
#include <vector>

namespace LKT
{
    Voximport &Voximport::Get()
    {
        static Voximport instance;
        return instance;
    }

    bool Voximport::Import(const std::string &path, VoxelModel &outData)
    {
        std::ifstream stream(path);
        if (stream.is_open())
        {
            char header[4] = {0};
            stream.read(header, 4);

            if (header[0] != 'V')
            {
                stream.close();
                std::cerr << "The current file is not of type .vox, cannot import the voxel model!";
                return false;
            }

            stream.read(reinterpret_cast<char *>(&outData.version), sizeof(int32_t));

            // Skipping 12 bytes (Main, chunk size and children size)
            stream.seekg(12, std::ios_base::cur);

            while (stream)
            {
                ReadChunk(stream, outData);
            }

            stream.close();
            return true;
        }
    }

    void Voximport::ReadChunk(std::ifstream &stream, VoxelModel &outData)
    {
        char id[4];
        stream.read(id, 4);

        uint32_t chunkSize;
        stream.read(reinterpret_cast<char *>(&chunkSize), 4);

        uint32_t childrenSize;
        stream.read(reinterpret_cast<char *>(&childrenSize), 4);

        const std::string sId(id, 4);
        if (sId == "SIZE")
        {
            ExtractSize(stream, outData);
            return;
        }
        else if (sId == "XYZI")
        {
            ExtractVoxelData(stream, outData);
            return;
        }
        else if (sId == "RGBA")
        {
            ExtractColor(stream, outData);
            return;
        }

        // Skipping the rest since I don't need it in my engine
        stream.seekg(childrenSize + chunkSize, std::ios::cur);
    }

    void Voximport::ExtractSize(std::ifstream &stream, VoxelModel &outData)
    {
        stream.read(reinterpret_cast<char *>(&outData.size), 12);
    }

    void Voximport::ExtractVoxelData(std::ifstream &stream, VoxelModel &outData)
    {
        stream.read(reinterpret_cast<char *>(&outData.nVoxels), 4);

        outData.voxels = new VoxelData[outData.nVoxels];
        stream.read(reinterpret_cast<char *>(&outData.voxels[0]), outData.nVoxels * 4);
    }

    void Voximport::ExtractColor(std::ifstream &stream, VoxelModel &outData)
    {
        stream.read(reinterpret_cast<char *>(&outData.palette[0]), 1024);
    }
}