#include <iomanip>
#include <iostream>
#include <cstdint>

#include "CRC.h"

int main()
{
    const char myString[] = {'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D'};

    std::uint32_t crc = CRC::Calculate(myString, sizeof(myString), CRC::CRC_32());

    std::cout << "kek" << std::endl;

    std::cout << std::hex << crc << std::endl;

    return 0;
}