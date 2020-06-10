#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>

#include "TinyRSA.h"
#include <gmpxx.h>

using namespace std;


int main()
{
    std::cout << "***" << endl;
    TinyRSA rsa;

    mpz_class public_key;
    mpz_class private_key;
    mpz_class n;

    std::cout << "Generating Keys ... " << endl;
    
    rsa.generate_keys(public_key, private_key, n, 613);

    std::cout << "done" << endl;

    mpz_class testnum;
    testnum = "213";

    mpz_class encrypted;
    encrypted = rsa.encrypt(testnum, public_key, n);

    cout << "Encrypted: " << encrypted << endl;

    mpz_class decrypted;
    decrypted = rsa.decrypt(encrypted, private_key, n);

    cout << "Decrypted: " << decrypted << endl;
    return 0;
}
