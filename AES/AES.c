#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define GETBIT(bits, which) ((bits >> which) & 1)   // iterate bits


// F I N I T E   F I E L D   A R I T H M E T I C

// Adds/Subtracts two polynomials within GF(2^8)
uint8_t ffAdd(const uint8_t a, const uint8_t b) {
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


// K E Y   E X P A N S I O N
// Helper function for substituting a byte by the SBOX table

uint8_t subByte(uint8_t byte) {
    const uint8_t SBOX[16][16] = {  // Table to substitute bytes with
        { 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76 },
        { 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0 },
        { 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15 },
        { 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75 },
        { 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84 },
        { 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf },
        { 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8 },
        { 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2 },
        { 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73 },
        { 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb },
        { 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79 },
        { 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08 },
        { 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a },
        { 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e },
        { 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf },
        { 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }
    };

    return SBOX[byte >> 4][byte & 0b1111];
}

// Takes a pointer to a four bit word and substitues each corresponding byte with S-Box
void subWord(uint8_t* word) {

    for (int i = 0; i < 4; i++) {
        word[i] = subByte(word[i]);
    }
}

// Rotates the bytes down through the word
void rotWord(uint8_t* word) {
    uint8_t lastByte = word[3];
    for (int i = 0; i < 3; i++) {
        word[(i + 3) % 4] = word[i];
    }
    word[2] = lastByte;
}


// C I P H E R   F U N C T I O N S

// THIS IS FOR SUBSTITUTING EVERY BYTE IN THE STATE
void subBytes(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = subByte(state[i]);
    }
}

// Helper function for shiftRows
void shiftRow(uint8_t* state, int row) {

    uint8_t lastByte = state[row + 3 * 4];
    for (int j = 0; j < 4; j++) {
        state[row + (((j + 3) * 4) % 16)] = state[row + j * 4];
    }
    state[row + 2 * 4] = lastByte;

}
// Rotates the first row by 0,
//         second row by    1,
//         third row by     2,
//         fourth row by    3
void shiftRows(uint8_t* state) {    // This is slow but I'm just trying to get something that works
    shiftRow(state, 1);
    shiftRow(state, 2);
    shiftRow(state, 2);
    shiftRow(state, 3);
    shiftRow(state, 3);
    shiftRow(state, 3);
}

void mixColumns(uint8_t* state) {
    const uint8_t poly[4][4] = {
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}
    };

    uint8_t tmpCol[4];
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < 4; i++) {
            tmpCol[i] = 0;
            for (int j = 0; j < 4; j++) {
                tmpCol[i] = ffAdd(ffMultiply(poly[i][j], state[k * 4 + j]), tmpCol[i]);
                // state[i * 4 + j] = ffMultiply(0x02, 0xd4);

            }
        }

        // Copy over tmpCol to state
        for (int i = 0; i < 4; i++) {
            state[k * 4 + i] = tmpCol[i];
        }
    }

}

void addRoundKey(uint8_t* state, const uint8_t* roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] = ffAdd(state[i], roundKey[i]);
    }
}

void invSubBytes(uint8_t* state) {
    const uint8_t INVSBOX[16][16] = {
        { 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb } ,
        { 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb } ,
        { 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e } ,
        { 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25 } ,
        { 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92 } ,
        { 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84 } ,
        { 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06 } ,
        { 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b } ,
        { 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73 } ,
        { 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e } ,
        { 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b } ,
        { 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4 } ,
        { 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f } ,
        { 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef } ,
        { 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61 } ,
        { 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d }
    };
    for (int i = 0; i < 16; i++) {
        state[i] = INVSBOX[state[i] >> 4][state[i] & 0b1111];
    }
}

void invShiftRows(uint8_t* state) {
    shiftRow(state, 3);
    shiftRow(state, 2);
    shiftRow(state, 2);
    shiftRow(state, 1);
    shiftRow(state, 1);
    shiftRow(state, 1);
}

void invMixColumns(uint8_t* state) {
    const uint8_t poly[4][4] = {
        {0x0e, 0x0b, 0x0d, 0x09},
        {0x09, 0x0e, 0x0b, 0x0d},
        {0x0d, 0x09, 0x0e, 0x0b},
        {0x0b, 0x0d, 0x09, 0x0e}
    };

    uint8_t tmpCol[4];
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < 4; i++) {
            tmpCol[i] = 0;
            for (int j = 0; j < 4; j++) {
                tmpCol[i] = ffAdd(ffMultiply(poly[i][j], state[k * 4 + j]), tmpCol[i]);
                // state[i * 4 + j] = ffMultiply(0x02, 0xd4);

            }
        }

        // Copy over tmpCol to state
        for (int i = 0; i < 4; i++) {
            state[k * 4 + i] = tmpCol[i];
        }
    }


}

// Expands the key to the 10, 12, or 14 round keys (depending on 128, 192, or 256)
// Once called, stores the expanded round key within w. w must be preacllocated.
// w is formatted as a 1d array of columns. Each round key begins at a multiple of 16.
void keyExpansion(const uint8_t* key, uint8_t* w, const int Nk) {
    const uint32_t Rcon[52] = { 0x00000000,
           0x01000000, 0x02000000, 0x04000000, 0x08000000,
           0x10000000, 0x20000000, 0x40000000, 0x80000000,
           0x1B000000, 0x36000000, 0x6C000000, 0xD8000000,
           0xAB000000, 0x4D000000, 0x9A000000, 0x2F000000,
           0x5E000000, 0xBC000000, 0x63000000, 0xC6000000,
           0x97000000, 0x35000000, 0x6A000000, 0xD4000000,
           0xB3000000, 0x7D000000, 0xFA000000, 0xEF000000,
           0xC5000000, 0x91000000, 0x39000000, 0x72000000,
           0xE4000000, 0xD3000000, 0xBD000000, 0x61000000,
           0xC2000000, 0x9F000000, 0x25000000, 0x4A000000,
           0x94000000, 0x33000000, 0x66000000, 0xCC000000,
           0x83000000, 0x1D000000, 0x3A000000, 0x74000000,
           0xE8000000, 0xCB000000, 0x8D000000
    };

    for (int i = 0; i < Nk; i++) {      // Placing the key into w
        for (int j = 0; j < 4; j++) {
            w[i*4 + j] = key[i*4 + j];
        }
    }

    const int wCols = 4 * ((Nk + 6) + 1);
    uint8_t copy[4];
    for (int col = Nk; col < wCols; col++) {
        // // Printing w
        // for (int i = 0; i < 4; i++) {
        //     for (int j = 0; j < (4 * ((Nk + 6) + 1)); j++) {
        //         printf("%.2x ", w[i + j * 4]);   // i*(4 * (Nk + 6 + 1))
        //         if (j % 4 == 3) {
        //             printf("    ");
        //         }
        //     }
        //     printf("\n");
        // }
        // printf("\n");

        if (col % Nk == 0) {        // Is a multiple of Nk
            // make a copy of w[col - 1]
            for (int i = 0; i < 4; i++) {
                copy[i] = w[(col - 1) * 4 + i];
            }

            rotWord(copy);
            for (int i = 0; i < 4; i++) {
                copy[i] = subByte(copy[i]);
            }

            // XOR with Rcon
            // printf("%.2x %.2x\n", copy[0], Rcon[col/4] >> 24);
            copy[0] = copy[0] ^ (Rcon[col/Nk] >> 24); 
            for (int i = 0; i < 4; i++) {
                copy[i] = copy[i] ^ w[(col - Nk) * 4 + i];
            }

            // Copying back into w
            for (int i = 0; i < 4; i++) {
                w[(col) * 4 + i] = copy[i];
            }

        }
        else if (Nk > 6 && col % Nk == 4) {
            for (int i = 0; i < 4; i++) {   
                copy[i] = w[(col - 1) * 4 + i];             // making a copy of w[i -1]
                copy[i] = subByte(copy[i]);                 // subword(copy)
                copy[i] = copy[i] ^ w[(col - Nk) * 4 + i];  // XOR with w[i - Nk]
                w[(col) * 4 + i] = copy[i];                 // Copying back into w
            }
        }
        else {
            // XOR by w[col - 1] and w[col - Nk]
            for (int i = 0; i < 4; i++) {
                w[col*4 + i] = w[(col - 1)*4 + i] ^ w[(col - Nk)*4 + i];
            }
        }
    }
}

// Helper function for displaying debug information about the ciphering
void printRound(const int round, const char* str, const uint8_t* state) {
    printf("round[%2d].%-10s", round, str);
    for (int i = 0; i < 16; i++) {
        printf("%.2x", state[i]);
    }
    printf("\n");
}
// Takes the state and the round keys and places it into cipherText
void cipher(const uint8_t* state, uint8_t* cipherText, const uint8_t* w, const int Nk) {
    
    printRound(0, "input", state);
    // Copying over the state into the ciphertext
    for (int i = 0; i < 16; i++) {
        cipherText[i] = state[i];
    }
    addRoundKey(cipherText, w);
    printRound(0, "k_sch", w);

    const int wRoundKeys = ((Nk + 6) + 1);
    for (int i = 1; i < wRoundKeys; i++) {
        printRound(i, "start", cipherText);
        subBytes(cipherText);
        printRound(i, "s_box", cipherText);
        shiftRows(cipherText);
        printRound(i, "s_row", cipherText);
        if (i != wRoundKeys - 1) {
            mixColumns(cipherText);
            printRound(i, "m_col", cipherText);
        }
        addRoundKey(cipherText, w + i * 16);
        printRound(i, "k_sch", w + i * 16);
        
        // for (int i = 0; i < 4; i++) {
        //     for (int j = 0; j < 4; j++) {
        //         printf("%.2x ", cipherText[j * 4 + i]);
        //     }
        //     printf("\n");
        // }
        // printf("\n");
        
    }
    printRound(wRoundKeys - 1, "output", cipherText);
}

void invCipher(const uint8_t* cipherText, uint8_t* plainText, const uint8_t* w, const int Nk) {
    int currentRound = 0;
    printRound(currentRound, "iinput", cipherText);
    
    // Copying over the cipherText to the plaintext
    for (int i = 0; i < 16; i++) {
        plainText[i] = cipherText[i];
    }

    const int wRoundKeys = ((Nk + 6) + 1);
    addRoundKey(plainText, w + (wRoundKeys - 1) * 16);
    printRound(currentRound, "ik_sch", w + (wRoundKeys - 1) * 16);
    currentRound++;

    for (int i = wRoundKeys - 2; i > 0; i--) {  // Continues until i == 0 for a different first step
        printRound(currentRound, "istart", plainText);
        invShiftRows(plainText);
        printRound(currentRound, "is_row", plainText);
        invSubBytes(plainText);
        printRound(currentRound, "is_box", plainText);
        addRoundKey(plainText, w + (i) * 16);
        printRound(currentRound, "ik_sch", w + i * 16);
        printRound(currentRound, "ik_add", plainText);
        invMixColumns(plainText);
        currentRound++;
    }

    printRound(currentRound, "istart", plainText);
    invShiftRows(plainText);
    printRound(currentRound, "is_row", plainText);
    invSubBytes(plainText);
    printRound(currentRound, "is_box", plainText);
    addRoundKey(plainText, w);
    printRound(currentRound, "ik_sch", w);
    printRound(currentRound, "ioutput", plainText);

}


int main() {
    // TEST CASES
    // Plaintext (the same for all 3 ciphers)
    const uint8_t plainText[16] = {
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb,
        0xcc, 0xdd, 0xee, 0xff
    };
    uint8_t* w;
    uint8_t cipherText[16];
    uint8_t decipherText[16];
    int Nk;

    // AES-128
    const uint8_t key128[16] = {
        0x00, 0x01, 0x02, 0x03, 
        0x04, 0x05, 0x06, 0x07, 
        0x08, 0x09, 0x0a, 0x0b, 
        0x0c, 0x0d, 0x0e, 0x0f
    };
    // Overhead
    Nk = 4;
    w = calloc(4 * 4 * (Nk + 6 + 1), sizeof(uint8_t));  // 4*4 blocks * num of Rounds

    // Ciphering
    printf("C.1   AES-128 (Nk=4, Nr=10)\n");
    printf("\n");
    printf("PLAINTEXT:          00112233445566778899aabbccddeeff\n");
    printf("KEY:                000102030405060708090a0b0c0d0e0f\n");
    printf("\n");
    printf("CIPHER (ENCRYPT):\n");

    keyExpansion(key128, w, Nk);
    cipher(plainText, cipherText, w, Nk);

    // Deciphering
    printf("\nINVERSE CIPHER (DECRYPT):\n");
    invCipher(cipherText, decipherText, w, Nk);

    free(w);

    // AES-192
    const uint8_t key192[24] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
        0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 
        0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 
        0x12, 0x13, 0x14, 0x15, 0x16, 0x17
    };
    // Overhead
    Nk = 6;
    w = calloc(4 * 4 * (Nk + 6 + 1), sizeof(uint8_t));  // 4*4 blocks * num of Rounds
    
    // Ciphering
    printf("\nC.2   AES-192 (Nk=6, Nr=12)\n\n");
    printf("PLAINTEXT:          00112233445566778899aabbccddeeff\n");
    printf("KEY:                000102030405060708090a0b0c0d0e0f1011121314151617\n\n");
    printf("CIPHER (ENCRYPT):\n");

    keyExpansion(key192, w, Nk);
    cipher(plainText, cipherText, w, Nk);

    // Deciphering
    printf("\nINVERSE CIPHER (DECRYPT):\n");
    invCipher(cipherText, decipherText, w, Nk);

    free(w);

    // AES-256
    const uint8_t key256[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    // Overhead
    Nk = 8;
    w = calloc(4 * 4 * (Nk + 6 + 1), sizeof(uint8_t));  // 4*4 blocks * num of Rounds
    
    // Ciphering
    printf("\nC.3   AES-256 (Nk=8, Nr=14)\n\n");
    printf("PLAINTEXT:          00112233445566778899aabbccddeeff\n");
    printf("KEY:                000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f\n\n");
    printf("CIPHER (ENCRYPT):\n");

    keyExpansion(key256, w, Nk);
    cipher(plainText, cipherText, w, Nk);

    // Deciphering
    printf("\nINVERSE CIPHER (DECRYPT):\n");
    invCipher(cipherText, decipherText, w, Nk);

    free(w);
    

    // uint8_t* w;
    // int Nk = 4;
    // const uint8_t key[16] = {
    //     0x2b, 0x7e, 0x15, 0x16,
    //     0x28, 0xae, 0xd2, 0xa6,
    //     0xab, 0xf7, 0x15, 0x88,
    //     0x09, 0xcf, 0x4f, 0x3c
    // };
    // w = calloc(4 * 4 * (Nk + 6 + 1), sizeof(uint8_t));  // 4*4 blocks * num of Rounds
    // keyExpansion(key, w, Nk);

    // // Printing w
    // for (int i = 0; i < 4; i++) {
    //     for (int j = 0; j < (4 * ((Nk + 6) + 1)); j++) {
    //         printf("%.2x ", w[i + j * 4]);   // i*(4 * (Nk + 6 + 1))
    //         if (j % 4 == 3) {
    //             printf("    ");
    //         }
    //     }
    //     printf("\n");
    // }


    // // uint8_t* state = (uint8_t*)calloc(16, sizeof(uint8_t));
    // // uint8_t* roundKey = (uint8_t*)calloc(16, sizeof(uint8_t));
    // const uint8_t stateArr[16] = {
    //     0x32, 0x43, 0xf6, 0xa8,
    //     0x88, 0x5a, 0x30, 0x8d,
    //     0x31, 0x31, 0x98, 0xa2,
    //     0xe0, 0x37, 0x07, 0x34
    // };

    // // for (int i = 0; i < 16; i++) {
    // //     state[i] = stateArr[i];
    // //     roundKey[i] = keyArr[i];
    // // }


    // uint8_t cipherText[16];
    // cipher(stateArr, cipherText, w, Nk);

    // uint8_t plaintext[16];
    // invCipher(cipherText, plaintext, w, Nk);

    // // Printing the State
    // // for (int i = 0; i < 4; i++) {
    // //     for (int j = 0; j < 4; j++) {
    // //         printf("%.2x ", cipherText[j * 4 + i]);
    // //     }
    // //     printf("\n");
    // // }

    // free(w);
    // // free(state);
    // // free(roundKey);
}