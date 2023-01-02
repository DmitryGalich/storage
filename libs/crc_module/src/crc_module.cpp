#include "crc_module.h"

#include "../../../build/third_party/crcpp/crcpp_src/inc/CRC.h"

namespace crc_module
{
    uint32_t compute_crc(const char *data)
    {
        return CRC::Calculate(data, sizeof(data), CRC::CRC_32());
    }
}
