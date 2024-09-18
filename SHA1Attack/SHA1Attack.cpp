#include <iostream>
#include <openssl/sha.h>
#include <bitset>
#include <cstring>
#include <vector>

#define NUM_HASHES 8

using namespace std;

// Prints the given bits to the screen
// The length is in bytes
void printBits(const unsigned char* bits, size_t length) {
    bool bitsFound = 0;
    for (size_t i = 0; i < length; i++) {
        if (bits[length - i - 1] == 0 && !bitsFound) {
            continue;
        }
        else {
            if (bitsFound == 0) {
                bitsFound = 1;
            }
            cout << bitset<8>(bits[length - i - 1]);
            if (i != length - 1) {
                cout << "_";
            }
        }
    }
    cout << endl;
}

// SHA1 documentation:
/*
SHA1() computes the SHA-1 message digest of the n bytes at 
d and places it in md (which must have space for 
SHA_DIGEST_LENGTH == 20 bytes of output). If md is NULL, the 
digest is placed in a static array. Note: setting md to NULL 
is not thread safe.
*/
class Hasher {
    const unsigned char preImageMask[NUM_HASHES][SHA_DIGEST_LENGTH] { // Used as a basis to create the preImageHashes
        0xff, 0x000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0x003, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0x00f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0x03f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0x0ff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0x0ff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0x0ff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0x0ff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    const size_t truncLengths[NUM_HASHES] = { 8, 10, 12, 14, 16, 18, 20, 22 };

    public:
        unsigned char preImageHashes[NUM_HASHES][SHA_DIGEST_LENGTH];     // These are the random hashes that we are trying to find using a preImageAttack
        Hasher() {
            srand(clock());
        }

        // Returns a digest that is as long as the specified length (in bits)
        // Recall that SHA1 produces 160 bit hashes
        unsigned char* hashTrunc(const unsigned char* plaintext, size_t plainLength, int hashLength) {
            static unsigned char hash[SHA_DIGEST_LENGTH];  // SHA_DIGEST_LENGTH == 20
            SHA1(plaintext, plainLength, hash);
            trunc(hash, hashLength);
            return hash;
        }

        void trunc(unsigned char* input, size_t truncLength) {  // Mutator that truncates the input to a given length of bits
            // go to input[truncLength]
            // set everything after and including that to 0's
            unsigned char mask = 0b11111111 >> (8 - (truncLength % 8));
            // printBits(input, SHA_DIGEST_LENGTH);
            input[truncLength / 8] = input[truncLength / 8] & mask;
            for (size_t i = truncLength/8 + 1; i < SHA_DIGEST_LENGTH; i++) {
                input[i] = 0;
            }
            // printBits(input, SHA_DIGEST_LENGTH);
        }
        
        // Uses the preImageMask along with pseudorandom generation to create random preImages to test against
        void generatePreImages() {
            for (int i = 0; i < NUM_HASHES; i++) {
                for (int j = 0; j < SHA_DIGEST_LENGTH; j++) {
                    preImageHashes[i][j] = rand() % (preImageMask[i][j] + 1);
                }
            }
            // for (int i = 0; i < 8; i++) {
            //     printBits(preImageHashes[i], SHA_DIGEST_LENGTH);
            // }
        }
        // Places the new random bits of length "length" into the bits pointer
        void generateBits(unsigned char* bits, size_t length) {
            for (size_t i = 0; i < length; i++) {
                bits[i] = rand() & 0xff;
            }
        }

        // Helper function to determine equvivalence between bitsets
        // Length is in bytes
        bool bitsAreEqual(unsigned char* a, unsigned char* b, size_t length) {
            for (size_t i = 0; i < length; i++) {
                if (a[i] != b[i]) {
                    return false;
                }
            }
            return true;
        }
        // Returns the amount of iterations that it took to find the hash
        size_t testPreImage(size_t whichTest) {
            vector<unsigned char> plainText{0};
            size_t iterations = 0;

            unsigned char* tempHash = hashTrunc(&plainText[0], plainText.size(), truncLengths[whichTest]);
            while (!bitsAreEqual(tempHash, preImageHashes[whichTest], (truncLengths[whichTest] + 7) / 8 )) {

                // ADD TO PLAINTEXT

                if (plainText[0] == 255) {
                    size_t i = 0;
                    while (plainText[i] == 0xff) {
                        // Check to see if a new unsigned char needs to be added
                        if (plainText.size() == i + 1) {    // We are in the last 8 bits of the vector
                            plainText.push_back(1);
                            plainText[i] = 0;
                        }
                        else {
                            plainText[i + 1] += 1;  // Incrementing the next char
                            plainText[i] = 0;       // Reseting the current char to 0
                            i++;
                        }
                    }
                }
                else {
                    plainText[0] += 1;
                }
                
                // RUN THE HASHING ALGORITHM

                tempHash = hashTrunc(&plainText[0], plainText.size(), truncLengths[whichTest]);
                iterations++;
            }
            // DEBUG printBits(tempHash, SHA_DIGEST_LENGTH);
            // DEBUG printBits(preImageHashes[whichTest], SHA_DIGEST_LENGTH);
            return iterations;
        }
        
        bool findBitsInVector(unsigned char* bits, vector<unsigned char*> bitVector) {
            for (size_t i = 0; i < bitVector.size(); i++) {
                if (bitsAreEqual(bits, bitVector[i], SHA_DIGEST_LENGTH)) {
                    printBits(bits, SHA_DIGEST_LENGTH);
                    printBits(bitVector[i], SHA_DIGEST_LENGTH);
                    return true;
                }
            }
            return false;
        }
        size_t testCollision(size_t whichTest) {
            size_t iterations = 0;

            // Create vector for all found hashes
            vector<unsigned char*> foundHashes;
            const size_t plainTextLength = 8;
            unsigned char plainText[plainTextLength];
            unsigned char* newHash;

            while (true) {
                // Find a new hash
                generateBits(plainText, plainTextLength);
                newHash = (hashTrunc(plainText, plainTextLength, truncLengths[whichTest]));
                printf("%s\n", newHash);

                // Check if the hash is already in the vector
                if (findBitsInVector(newHash, foundHashes)) {
                    // If so, return the number of iterations
                    return iterations;
                }
                
                // If not, add it to the vector and loop
                foundHashes.push_back(newHash);
                iterations++;
            }
        }

};

// Here are the test cases: { 8, 10, 12, 14, 16, 18, 20, 22 };
int main() {    
    Hasher hashTester;
    
    printf("did it asshole %ld\n", hashTester.testCollision(0));
    // // PreImage setup
    // for (size_t i = 0; i < 8; i++) {
    //     for (int j = 0; j < 50; j++) {
    //         hashTester.generatePreImages();
    //         printf("TestCase(%ld:%.2d): %10ld iterations\n", i, j, hashTester.testPreImage(i));
    //     }
    //     cout << endl;
    // }
}