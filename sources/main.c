#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "sign.h"
#include "verify.h"
#include "keygen.h"

void print_usage(const char *prog_name) {
    printf("Использование: %s [опции]\n", prog_name);
    printf("Опции:\n");
    printf("  -g               Генерация пары ключей\n");
    printf("  -s               Подписать файл\n");
    printf("  -v               Проверить подпись\n");
    printf("  -i <входной>     Входной файл\n");
    printf("  -o <выходной>    Выходной файл\n");
    printf("  -k <файл ключа>  Файл ключа (приватный или публичный)\n");
    printf("  -p <публ. ключ>  Файл публичного ключа (для генерации ключей)\n");
    printf("  -b <размер>      Размер ключа в битах (по умолчанию 2048)\n");
    printf("  -h               Показать эту справку\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    char *key_file = NULL;
    char *public_key_file = NULL;
    int key_size = 2048;
    int mode = 0; // 1: генерация ключей, 2: подпись, 3: проверка

    while ((opt = getopt(argc, argv, "gsvhi:o:k:p:b:")) != -1) {
        switch (opt) {
            case 'g':
                mode = 1;
                break;
            case 's':
                mode = 2;
                break;
            case 'v':
                mode = 3;
                break;
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'k':
                key_file = optarg;
                break;
            case 'p':
                public_key_file = optarg;
                break;
            case 'b':
                key_size = atoi(optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    switch (mode) {
        case 1: // Генерация ключей
            if (!key_file || !public_key_file) {
                fprintf(stderr, "Не указаны файлы для ключей.\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            if (generate_keys(key_file, public_key_file, key_size) != 0) {
                fprintf(stderr, "Ошибка при генерации ключей.\n");
                exit(EXIT_FAILURE);
            }
            printf("Ключи успешно сгенерированы.\n");
            break;
        case 2: // Подпись
            if (!input_file || !output_file || !key_file) {
                fprintf(stderr, "Отсутствуют необходимые аргументы для подписи.\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            if (sign_file(input_file, output_file, key_file) != 0) {
                fprintf(stderr, "Ошибка при выработке подписи.\n");
                exit(EXIT_FAILURE);
            } else {
                printf("Файл успешно подписан. Подпись сохранена в %s\n", output_file);
            }
            break;
        case 3: // Проверка
            if (!input_file || !output_file || !key_file) {
                fprintf(stderr, "Отсутствуют необходимые аргументы для проверки.\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            int verify_result = verify_file(input_file, output_file, key_file);
            if (verify_result == 1) {
                printf("Подпись действительна.\n");
            } else if (verify_result == 0) {
                printf("Подпись недействительна.\n");
                exit(EXIT_FAILURE);
            } else {
                fprintf(stderr, "Ошибка при проверке подписи.\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Не указан режим работы.\n");
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
    }

    return 0;
}

