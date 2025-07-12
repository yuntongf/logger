#include <gtest/gtest.h>
#include "util.h"

TEST(AESCtrTest, EncryptDecryptRoundTrip) {
    std::vector<unsigned char> key(32, 0x01);
    std::vector<unsigned char> plaintext = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};

    std::vector<unsigned char> iv = util::encryption::generate_iv();
    std::vector<unsigned char> ciphertext;
    std::vector<unsigned char> decrypted;

    util::encryption::aes_ctr_encrypt(key, plaintext, ciphertext, iv);
    util::encryption::aes_ctr_decrypt(key, ciphertext, decrypted, iv);

    ASSERT_EQ(decrypted, plaintext);
}