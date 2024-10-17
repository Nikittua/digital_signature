cmake_minimum_required(VERSION 3.10)
project(digital_signature C)

set(CMAKE_C_STANDARD 99)

# Указываем путь к заголовочным файлам
include_directories(headers)

# Ищем библиотеку OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Добавляем исходные файлы
add_executable(digital_signature
    sources/main.c
    sources/sign.c
    sources/verify.c
    sources/keygen.c
)

# Линкуем библиотеки OpenSSL
target_link_libraries(digital_signature ${OPENSSL_LIBRARIES})

