#include "encryptor.h"

Encryptor::Encryptor() {
    read_server_public_key_();
    init_client_key_();

    // Save client public key
    FILE* fp = fopen(CLIENT_KEY_FILE_PATH, "w");
    PEM_write_PUBKEY(fp, client_pair_key_);
    // error handling
}

Encryptor::~Encryptor() {
    EVP_PKEY_free(server_public_key_);
    EVP_PKEY_free(client_pair_key_);
    EVP_cleanup();
}

void Encryptor::encrypt(std::vector<uint8_t>& data) {
    std::vector<unsigned char> ciphertext, iv;
    util::encryption::aes_ctr_encrypt(
        aes_encrypt_key_, 
        data,
        ciphertext, 
        iv
    );
    std::copy(ciphertext.begin(), ciphertext.end(), data.begin());
}

void Encryptor::read_server_public_key_() {
    FILE* fp = fopen(SERVER_KEY_FILE_PATH, "r");
    if (!fp) {
        throw std::runtime_error("Failed to open server public key file: " + std::string(SERVER_KEY_FILE_PATH));
    }

    EVP_PKEY* key = PEM_read_PUBKEY(fp, nullptr, nullptr, nullptr);
    fclose(fp);

    if (!key) {
        throw OpenSSLError("Failed to read public key");
    }

    server_public_key_ = key;
}

void Encryptor::init_client_key_() {
    client_pair_key_ = util::encryption::generate_ec_key();
    auto client_secret = util::encryption::derive_shared_secret(client_pair_key_, server_public_key_);
    aes_encrypt_key_ = util::encryption::sha256(client_secret);
}