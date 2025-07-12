#pragma once

#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <filesystem>

namespace util::fs {
    using fpath = std::filesystem::path;

    inline int open_rw_file(fpath dir, const std::string& filename) {
        if (!std::filesystem::exists(dir)) {
            throw std::runtime_error("directory " + std::string(dir) + " does not exist");
        }
        fpath path = dir / filename;
        int fd = open(path.c_str(), O_RDWR | O_CREAT, 0644);
        if (fd == -1) {
            perror("failed to open cache file");
        }
        return fd;
    }

    inline void truncate_file(int fd, std::size_t size) {
        if (ftruncate(fd, size) == -1) {
            throw std::runtime_error("ftruncate failed: " + std::string(strerror(errno)));
        }
    }

    inline void close_file(int fd) {
        close(fd);
    }

    inline std::size_t get_file_size(int fd) {
        struct stat st;
        if (fstat(fd, &st) != 0) {
            close_file(fd);
            perror("failed to get file size");
        }
        return static_cast<std::size_t>(st.st_size);
    }

    inline void move_file_content(int fd_in, int fd_out) {
        constexpr size_t BUFFER_SIZE = 8192;
        char buffer[BUFFER_SIZE];

        while (true) {
            ssize_t bytes_read = read(fd_in, buffer, BUFFER_SIZE);
            if (bytes_read < 0) {
                if (errno == EINTR) continue;
                throw std::runtime_error("read failed: " + std::string(strerror(errno)));
            }
            if (bytes_read == 0) break; // EOF

            ssize_t total_written = 0;
            while (total_written < bytes_read) {
                ssize_t bytes_written = write(fd_out, buffer + total_written, bytes_read - total_written);
                if (bytes_written < 0) {
                    if (errno == EINTR) continue;
                    throw std::runtime_error("write failed: " + std::string(strerror(errno)));
                }
                total_written += bytes_written;
            }
        }
    }
}

namespace util::encryption {

#define THROW_OPENSSL_ERR(msg) throw std::runtime_error("OpenSSL error: " + std::string(msg))

inline EVP_PKEY* generate_ec_key() {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!ctx) {
        THROW_OPENSSL_ERR("EVP_PKEY_CTX_new_id");
    }
    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        THROW_OPENSSL_ERR("keygen_init");
    }
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0) {
        THROW_OPENSSL_ERR("set curve");
    }
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        THROW_OPENSSL_ERR("keygen");
    }
    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

inline std::vector<unsigned char> derive_shared_secret(EVP_PKEY* priv, EVP_PKEY* peer_pub) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(priv, nullptr);
    if (!ctx) {
        THROW_OPENSSL_ERR("CTX_new");
    }
    if (EVP_PKEY_derive_init(ctx) <= 0) {
        THROW_OPENSSL_ERR("derive_init");
    }
    if (EVP_PKEY_derive_set_peer(ctx, peer_pub) <= 0) {
        THROW_OPENSSL_ERR("set_peer");
    }
    size_t secret_len;
    if (EVP_PKEY_derive(ctx, nullptr, &secret_len) <= 0) {
        THROW_OPENSSL_ERR("get length");
    }
    std::vector<unsigned char> secret(secret_len);
    if (EVP_PKEY_derive(ctx, secret.data(), &secret_len) <= 0) {
        THROW_OPENSSL_ERR("derive");
    }
    EVP_PKEY_CTX_free(ctx);
    secret.resize(secret_len);
    return secret;
}

inline void aes_ctr_encrypt(const std::vector<unsigned char>& key,
                     const std::vector<unsigned char>& plaintext,
                     std::vector<unsigned char>& ciphertext,
                     const std::vector<unsigned char>& iv) {
    assert(key.size() == 32 && "AES-256 key must be 32 bytes");

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        THROW_OPENSSL_ERR("Encrypt context");
    }
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key.data(), iv.data()) != 1) {
        THROW_OPENSSL_ERR("EncryptInit");
    }
    ciphertext.resize(plaintext.size());
    int outlen;
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &outlen, plaintext.data(), plaintext.size()) != 1){
        THROW_OPENSSL_ERR("EncryptUpdate");
    }
    int tmplen;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + outlen, &tmplen);  // usually tmplen == 0

    EVP_CIPHER_CTX_free(ctx);
}

inline std::vector<unsigned char> generate_iv(std::size_t len = 16) {
    std::vector<unsigned char> iv(len);
    if (!RAND_bytes(iv.data(), len))
        THROW_OPENSSL_ERR("RAND_bytes failed");
    return iv;
}
inline void aes_ctr_decrypt(const std::vector<unsigned char>& key,
                     const std::vector<unsigned char>& ciphertext,
                     std::vector<unsigned char>& plaintext,
                     const std::vector<unsigned char>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        THROW_OPENSSL_ERR("Decrypt context");
    }
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key.data(), iv.data()) != 1) {
        THROW_OPENSSL_ERR("DecryptInit");
    }
    plaintext.resize(ciphertext.size());
    int outlen;
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &outlen, ciphertext.data(), ciphertext.size()) != 1) {
        THROW_OPENSSL_ERR("DecryptUpdate");
    }
    int tmplen;
    EVP_DecryptFinal_ex(ctx, plaintext.data() + outlen, &tmplen);  // usually tmplen == 0

    EVP_CIPHER_CTX_free(ctx);
}

inline std::vector<unsigned char> sha256(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    SHA256(data.data(), data.size(), hash.data());
    return hash;
}

}