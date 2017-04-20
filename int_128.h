#ifndef __INT_128_H__
#define __INT_128_H__
#include <stdint.h>
#include <inttypes.h>

class int_128
{
public:
	int_128(int64_t high, uint64_t low) : high(high), low(low){};
	int_128() : high(0), low(0){};


    int_128 operator+(const int_128 & rhs);
    int_128 operator-(const int_128 & rhs);
    uint8_t get_bytes(const uint8_t byte_num);

private:

    int64_t  high;
    uint64_t low;
};

#endif //__INT128_H__