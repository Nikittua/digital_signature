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

    // Сохраняем приватный ключ
    FILE *priv_fp = fopen(private_key_file, "wb");
    if (!priv_fp) {
        perror("Ошибка открытия файла приватного ключа");
        goto cleanup;
    }
    if (!PEM_write_PrivateKey(priv_fp, pkey, NULL, NULL, 0, NULL, NULL)) {
        fprintf(stderr, "Ошибка записи приватного ключа\n");
        ERR_print_errors_fp(stderr);
        fclose(priv_fp);
        goto cleanup;
    }
    fclose(priv_fp);

    // Сохраняем публичный ключ
    FILE *pub_fp = fopen(public_key_file, "wb");
    if (!pub_fp) {
        perror("Ошибка открытия файла публичного ключа");
        goto cleanup;
    }
    if (!PEM_write_PUBKEY(pub_fp, pkey)) {
        fprintf(stderr, "Ошибка записи публичного ключа\n");
        ERR_print_errors_fp(stderr);
        fclose(pub_fp);
        goto cleanup;
    }
    fclose(pub_fp);

    ret = 0; // Успешное выполнение

cleanup:
    if (bn) BN_free(bn);
    if (rsa) RSA_free(rsa);
    if (pkey) EVP_PKEY_free(pkey);

    return ret;
}