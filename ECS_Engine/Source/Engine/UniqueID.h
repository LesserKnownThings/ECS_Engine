#pragma once

#include <array>
#include <functional>
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <string>

namespace LKT
{
    struct UniqueID
    {
        friend class IDManager;

    public:
        void Reset();

        std::array<uint8_t, 16> data;
        std::string ToString() const;
        void FromString(const std::string& guid_string);

        bool operator==(const UniqueID& other) const
        {
            return data == other.data;
        }

        bool operator<(const UniqueID& other) const
        {
            return data < other.data;
        }

    private:
#ifdef _WIN32
        static UniqueID GenerateGUID();
#elif __linux__
        static UniqueID GenerateGUID();
#else
#error Platform not supported
#endif
    };
}

template<>
struct std::hash<LKT::UniqueID>
{
    size_t operator()(const LKT::UniqueID& ID) const
    {
        size_t hash = 0;
        for (uint8_t byte : ID.data)
        {
            hash ^= std::hash<uint8_t>()(byte) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

namespace LKT
{
    class IDManager
    {
        friend struct UniqueID;

    public:
        static IDManager& Get();

        static UniqueID GenerateGUID();

    private:
        UniqueID InternalGenerateGUID();
        void RemoveGUID(const UniqueID& guid);
        bool AddGUID(const UniqueID& guid);
        bool GUIDExists(const UniqueID& guid);

        std::unordered_set<UniqueID> guids;
    };
}