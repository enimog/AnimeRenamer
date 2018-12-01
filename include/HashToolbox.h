#pragma once

#include <vector>

namespace Toolbox
{
    namespace HashToolbox
    {
        typedef unsigned char BYTE;

        size_t md4_hash(std::vector<BYTE> const& data);
        size_t md4_hash(const BYTE* start, size_t length);

        size_t ed2k_hash(std::vector<BYTE> const& data);
    }
}
