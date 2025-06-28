#pragma once

#include <filesystem>
#include <vector>
#include <string>

#include "util.h"

class Encryptor {
    using fpath = std::filesystem::path;
public:
    Encryptor(fpath server_key_fpath, fpath client_key_fpath);

    Encryptor(const Encryptor& other) = delete;

    Encryptor& operator=(const Encryptor& other) = delete;

    ~Encryptor();

    void encrypt(void* data, const size_t size);

private:
    void get_server_public_key_(fpath path);

    void init_client_key_();

private:
    EVP_PKEY* server_public_key_;
    EVP_PKEY* client_pair_key_;
    std::vector<unsigned char> aes_encrypt_key_;
};