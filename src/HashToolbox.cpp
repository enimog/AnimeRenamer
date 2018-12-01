#include "HashToolbox.h"

namespace Toolbox
{
    namespace HashToolbox
    {
        size_t md4_hash(std::vector<BYTE> const& data)
        {
            return md4_hash(&data[0], data.size());
        }

        size_t md4_hash(const BYTE* start, size_t length)
        {
            return 0;
        }

        size_t ed2k_hash(std::vector<BYTE> const& data)
        {
            const auto size = data.size();

            if (size <= 9728000)
            {
                return md4_hash(data);
            }

            std::vector<BYTE> hashlist;
            for (size_t ptr_pos = 0; ptr_pos < size; ptr_pos += 9728000)
            {
                if (ptr_pos + 9728000 > size)
                {
                    hashlist.push_back(md4_hash(&data[0] + ptr_pos, size - ptr_pos));
                }
                else
                {
                    hashlist.push_back(md4_hash(&data[0] + ptr_pos, 9728000));
                }
            }

            return md4_hash(hashlist);
        }
    }
}
