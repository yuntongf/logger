#include "encryptor.h"

Encryptor::Encryptor(fpath server_key_fpath, fpath client_key_fpath) {
    get_server_public_key_(server_key_fpath);
    init_client_key_();

    // Save client public key
    FILE* fp = fopen(client_key_fpath.c_str(), "w");
    PEM_write_PUBKEY(fp, client_pair_key_);
    // error handling
}

Encryptor::~Encryptor() {
    EVP_PKEY_free(server_public_key_);
    EVP_PKEY_free(client_pair_key_);
    EVP_cleanup();
}

void Encryptor::encrypt(void* data, const size_t size) {
    unsigned char* input_data = static_cast<unsigned char*>(data);
    std::vector<unsigned char> ciphertext, iv;
    util::encryption::aes_ctr_encrypt(
        aes_encrypt_key_, 
        std::vector<unsigned char>(input_data, input_data + size), 
        ciphertext, 
        iv
    );
    memcpy(data, ciphertext.data(), size);
}

void Encryptor::get_server_public_key_(fpath path) {
    FILE* fp = fopen(path.c_str(), "r");
    server_public_key_ = PEM_read_PUBKEY(fp, nullptr, nullptr, nullptr);
}

void Encryptor::init_client_key_() {
    client_pair_key_ = util::encryption::generate_ec_key();
    auto client_secret = util::encryption::derive_shared_secret(client_pair_key_, server_public_key_);
    aes_encrypt_key_ = util::encryption::sha256(client_secret);
}