#include "crypto_module.h"

#include <iostream>

#include "aes.h"

namespace crypto_module
{
    void make_fun()
    {
        struct AES_ctx ctx;
        uint8_t key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

        AES_init_ctx(&ctx, key);

        std::cout << "crypto" << std::endl;
    }
}
