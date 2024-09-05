#include "UniqueID.h"

#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>


#ifdef _WIN32
#include <windows.h>
#include <rpc.h>
#pragma comment(lib, "rpcrt4.lib")
#elif __linux__
#include <uuid/uuid.h>
#endif

namespace LKT
{
    void UniqueID::Reset()
    {
        IDManager::Get().RemoveGUID(*this);
    }

    std::string UniqueID::ToString() const
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (int32_t i = 0; i < data.size(); ++i) {
            if (i == 4 || i == 6 || i == 8 || i == 10)
                oss << '-';
            oss << std::setw(2) << static_cast<int32_t>(data[i]);
        }
        return oss.str();
    }

    void UniqueID::FromString(const std::string& guid_string)
    {
        std::array<uint8_t, 16> result;

        int32_t pos = 0;
        for (int32_t i = 0; i < result.size(); ++i)
        {
            // Skip '-' characters
            if (guid_string[pos] == '-')
                ++pos;

            // Parse two hexadecimal characters
            std::string byte_string = guid_string.substr(pos, 2);
            result[i] = static_cast<uint8_t>(std::stoi(byte_string, nullptr, 16));

            pos += 2;
        }

        memcpy(data.data(), result.data(), sizeof(result));
    }

#ifdef _WIN32
    UniqueID UniqueID::GenerateGUID()
    {
        UniqueID guid;
        UUID uuid;
        UuidCreate(&uuid);
        memcpy(guid.data.data(), &uuid, sizeof(uuid));
        return guid;
    }
#elif __linux__
    UniqueID UniqueID::GenerateGUID()
    {
        UniqueID guid;
        uuid_t uuid;
        uuid_generate_random(uuid);
        memcpy(guid.data.data(), uuid, sizeof(uuid));
        return guid;
    }
#endif

    IDManager& IDManager::Get()
    {
        static IDManager instance;
        return instance;
    }

    UniqueID IDManager::GenerateGUID()
    {
        return IDManager::Get().InternalGenerateGUID();
    }

    UniqueID IDManager::InternalGenerateGUID()
    {
        UniqueID id;
        do
        {
            id = UniqueID::GenerateGUID();
        } while (GUIDExists(id));

        AddGUID(id);
        return id;
    }

    void IDManager::RemoveGUID(const UniqueID& guid)
    {
        guids.erase(guid);
    }

    bool IDManager::AddGUID(const UniqueID& guid)
    {
        return guids.insert(guid).second;
    }

    bool IDManager::GUIDExists(const UniqueID& guid)
    {
        return guids.find(guid) != guids.end();
    }
}