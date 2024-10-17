#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include "keygen.h"

int generate_keys(const char *private_key_file, const char *public_key_file, int key_size) {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    int ret = -1;

    // Генерация ключей без промежуточных проверок ошибок
    BIGNUM *bn = BN_new();
    BN_set_word(bn, RSA_F4);

    RSA *rsa = RSA_new();
    RSA_generate_key_ex(rsa, key_size, bn, NULL);

    EVP_PKEY *pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pkey, rsa);
    rsa = NULL; // rsa теперь принадлежит pkey

    // Сохранение приватного ключа
    FILE *priv_fp = fopen(private_key_file, "wb");
    PEM_write_PrivateKey(priv_fp, pkey, NULL, NULL, 0, NULL, NULL);
    fclose(priv_fp);

    // Сохранение публичного ключа
    FILE *pub_fp = fopen(public_key_file, "wb");
    PEM_write_PUBKEY(pub_fp, pkey);
    fclose(pub_fp);

    ret = 0; // Успешное выполнение

    // Очистка ресурсов
    BN_free(bn);
    EVP_PKEY_free(pkey);

    return ret;
}
