#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "sign.h"

int sign_file(const char *input_file, const char *signature_file, const char *private_key_file) {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Открываем приватный ключ
    FILE *key_fp = fopen(private_key_file, "r");
    if (!key_fp) {
        perror("Ошибка открытия файла приватного ключа");
        return -1;
    }

    // Читаем приватный ключ
    EVP_PKEY *pkey = PEM_read_PrivateKey(key_fp, NULL, NULL, NULL);
    fclose(key_fp);

    if (!pkey) {
        fprintf(stderr, "Ошибка чтения приватного ключа\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // Создаем контекст для подписи
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_SignInit(mdctx, EVP_sha256());


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
        EVP_SignUpdate(mdctx, buffer, len);
    }
    

    if (ferror(in_fp)) {
        perror("Ошибка чтения входного файла");
        goto cleanup;
    }

    // Вырабатываем подпись
    unsigned char *sig = malloc(EVP_PKEY_size(pkey));
    unsigned int sig_len;
    if (!sig) {
        fprintf(stderr, "Ошибка выделения памяти для подписи\n");
        goto cleanup;
    }

    EVP_SignFinal(mdctx, sig, &sig_len, pkey);


    // Сохраняем подпись
    FILE *out_fp = fopen(signature_file, "wb");
    if (!out_fp) {
        perror("Ошибка открытия файла подписи");
        free(sig);
        goto cleanup;
    }

    if (fwrite(sig, 1, sig_len, out_fp) != sig_len) {
        perror("Ошибка записи подписи в файл");
        free(sig);
        fclose(out_fp);
        goto cleanup;
    }

    fclose(out_fp);
    free(sig);
    ret = 0; // Успешное выполнение

cleanup:
    fclose(in_fp);
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    return ret;
}

