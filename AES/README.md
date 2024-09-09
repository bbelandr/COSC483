# AES Incryption
This program is able to encrypt and decrypt plaintext using the Advanced Encryption Standard (AES). The program supports AES-128, AES-192, and AES-256.

The program is also available on [GitHub](https://github.com/bbelandr/COSC483/tree/main/AES).

### How to compile
In order to compile AES.c, a makefile has been provided. Ensure that you create an exe directory from wherever you try to run the code.

"make test" will run a diff command against appendix_c.txt. If you want to run the program without the already provided test cases, you can call "make run" which will run without any test checks.

```zsh
mkdir exe
make text
```

Should the makefile not work, AES.c can be compiled with `gcc -o AES AES.c`.

### Resources Used
- [FIPS 197 Documentation on AES](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf)
- [Step by step animation of AES-128](https://formaestudio.com/rijndaelinspector/archivos/Rijndael_Animation_v4_eng-html5.html)
- Collaberative coding with Alec Merkle

### Test Cases
This program passes all test cases provided by Appendix C of the [FIPS 197 Documentation](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf). Specifically, the test cases, named [appendix_c.txt](https://utk.instructure.com/courses/207074/files/22408255?wrap=1), were downloaded from the canvas page.