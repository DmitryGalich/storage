#define CRC_MODULE
#ifdef CRC_MODULE

#include <stdint.h>

namespace crc_module
{
    uint32_t compute_crc(const char *);
}

#endif