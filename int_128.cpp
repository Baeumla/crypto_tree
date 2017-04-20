#include "int_128.h"


int_128 int_128::operator+(const int_128 & rhs)
{
    int_128 sum;
    sum.high = high + rhs.high;
    sum.low = low + rhs.low;
    // check for overflow of low 64 bits, add carry to high
    if (sum.low < low)
        ++sum.high;
    return sum;
}


int_128 int_128::operator-(const int_128 & rhs)
{
    int_128 difference;
    difference.high = high - rhs.high;
    difference.low = low - rhs.low;
    // check for underflow of low 64 bits, subtract carry to high
    if (difference.low > low)
        --difference.high;
    return difference;
}

uint8_t int_128::get_bytes(const uint8_t byte_num)
{
    uint8_t result = 0;

    if (byte_num <= 7)
    {
        // Return portion of high order bits
        result = high >> ((8 - byte_num) * 8);
    } else if (byte_num < 16) {
        // Return portion of low order bits
        result = low >> ((8 - byte_num) * 8);
    }

    return result;
}

