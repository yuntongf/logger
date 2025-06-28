#pragma once

#include <filesystem>
#include <vector>
#include <string>

#include "util.h"

class Decryptor {
    using fpath = std::filesystem::path;
public:
    Decryptor(fpath server_key_fpath, fpath client_key_fpath);

    Decryptor(const Decryptor& other) = delete;

    Decryptor& operator=(const Decryptor& other) = delete;

    ~Decryptor();

    void encrypt(void* data, const size_t size);

private:
    void get_client_public_key_(fpath path);

    void init_server_key_();

private:
    EVP_PKEY* client_public_key_;
    EVP_PKEY* server_pair_key_;
    std::vector<unsigned char> aes_encrypt_key_;
};