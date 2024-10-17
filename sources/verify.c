#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "verify.h"

int verify_file(const char *input_file, const char *signature_file, const char *public_key_file) {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Открываем публичный ключ
    FILE *key_fp = fopen(public_key_file, "r");
    if (!key_fp) {
        perror("Ошибка открытия файла публичного ключа");
        return -1;
    }

    // Читаем публичный ключ
    EVP_PKEY *pkey = PEM_read_PUBKEY(key_fp, NULL, NULL, NULL);
    fclose(key_fp);

    if (!pkey) {
        fprintf(stderr, "Ошибка чтения публичного ключа\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // Создаем контекст для проверки
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_VerifyInit(mdctx, EVP_sha256());

    // Читаем входной файл
    FILE *in_fp = fopen(input_file, "rb");
    if (!in_fp) {
        perror("Ошибка открытия входного файла");
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return -1;
    }

    unsigned char buffer[1024];
    size_t len;
    int ret = -1; // Инициализируем код возврата ошибкой

    while ((len = fread(buffer, 1, sizeof(buffer), in_fp)) > 0) {
        EVP_VerifyUpdate(mdctx, buffer, len);
    }

    if (ferror(in_fp)) {
        perror("Ошибка чтения входного файла");
        goto cleanup;
    }

    fclose(in_fp);

    // Читаем подпись
    FILE *sig_fp = fopen(signature_file, "rb");
    if (!sig_fp) {
        perror("Ошибка открытия файла подписи");
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return -1;
    }

    fseek(sig_fp, 0, SEEK_END);
    long sig_len = ftell(sig_fp);
    fseek(sig_fp, 0, SEEK_SET);
    unsigned char *sig = malloc(sig_len);
    if (!sig) {
        fprintf(stderr, "Ошибка выделения памяти для подписи\n");
        fclose(sig_fp);
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return -1;
    }
    if (fread(sig, 1, sig_len, sig_fp) != sig_len) {
        perror("Ошибка чтения файла подписи");
        free(sig);
        fclose(sig_fp);
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return -1;
    }
    fclose(sig_fp);

    // Проверяем подпись
    ret = EVP_VerifyFinal(mdctx, sig, sig_len, pkey);

    free(sig);
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);

    if (ret == 1) {
        // Подпись действительна
        return 1;
    } else if (ret == 0) {
        // Подпись недействительна
        return 0;
    } else {
        // Ошибка при проверке
        fprintf(stderr, "Ошибка при проверке подписи\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

cleanup:
    fclose(in_fp);
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    return ret;
}
