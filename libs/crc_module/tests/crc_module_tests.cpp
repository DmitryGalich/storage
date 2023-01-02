#include <gtest/gtest.h>

#include "crc_module.h"

TEST(CrcModule, SuccessComputing)
{
    crc_module::compute_crc();
}