#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <openssl/err.h>

#include "util.h"

#define SERVER_KEY_FILE_PATH "reader_public.pem"
#define CLIENT_KEY_FILE_PATH "writer.pem"

class Encryptor {
    using fpath = std::filesystem::path;
public:
    Encryptor();

    Encryptor(const Encryptor& other) = delete;

    Encryptor& operator=(const Encryptor& other) = delete;

    ~Encryptor();

    void encrypt(u_int8_t*& data, const size_t size);

private:
    void read_server_public_key_();

    void init_client_key_();

private:
    EVP_PKEY* server_public_key_;
    EVP_PKEY* client_pair_key_;
    std::vector<unsigned char> aes_encrypt_key_;
};

class OpenSSLError : public std::runtime_error {
public:
    OpenSSLError(const std::string& msg)
        : std::runtime_error(msg + ": " + getOpenSSLError()) {}

private:
    static std::string getOpenSSLError() {
        char buf[256];
        ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
        return std::string(buf);
    }
};