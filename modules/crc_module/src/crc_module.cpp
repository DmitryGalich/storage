#include "crc_module.h"

#include "CRC.h"

namespace crc_module
{
    uint32_t compute_crc(const char *data)
    {
        return CRC::Calculate(data, sizeof(data), CRC::CRC_32());
    }
}
