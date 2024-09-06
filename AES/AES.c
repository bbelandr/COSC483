#include <stdio.h>
#include <stdint.h>

#define GETBIT(bits, which) ((bits >> which) & 1)

// Adds/Subtracts two polynomials within GF(2^8)
uint8_t ffAdd(uint8_t a, uint8_t b) {
    return a ^ b;
}

// Multiplies a polynomial (represented by 8 bits) by x and modulo's
// it to make sure that it stays within GF(2^8)
uint8_t xtime(uint8_t bits) {   
    uint16_t temp = bits << 1;
    if (GETBIT(temp, 8)) {
        temp = temp ^ 0x1b;
    }
    return (uint8_t)temp;
}

// Multiplies two polynomials by each other within GF(2^8)
uint8_t ffMultiply(uint8_t a, uint8_t b) {
    uint8_t sum = 0;
    if (GETBIT(b, 0)) {
        sum = ffAdd(sum, a);
    }
    for (int i = 1; i < 8; i++) {
        a = xtime(a);
        if (GETBIT(b, i)) {
            sum = ffAdd(sum, a);
        }
    }
    return sum;
}

int main() {
    printf("%x\n", ffMultiply(0x57, 0x13));
}