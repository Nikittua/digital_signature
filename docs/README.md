

# Утилита для электронной подписи

Утилита командной строки для генерации цифровых подписей, проверки подписей и генерации пар ключей RSA.

## Возможности

- **Генерация пар ключей RSA**
- **Подпись файлов** с использованием приватного ключа
- **Проверка подписи файлов** с использованием публичного ключа

## Требования

- **Библиотека OpenSSL**
- **CMake** для сборки проекта

## Сборка проекта

   ```bash

   git clone digital_signature
   cd digital_signature
   mkdir build
   cd build

   # В случае ошибки CMake not able to find OpenSSL library установите libssl-dev
   cmake ..
   make

   ```


# **Использование**

**Генерация ключей:**

```shell
./digital_signature -g -k private_key.pem -p public_key.pem
```
- g: режим генерации ключей
- k: файл для приватного ключа
- p: файл для публичного ключа
- b: размер ключа в битах (опционально, по умолчанию 2048)

**Подпись файла:**


```shell
./digital_signature -s -i input.txt -o signature.bin -k private_key.pem
```
- s: режим подписи
- i: входной файл для подписи
- o: выходной файл для подписи
- k: файл приватного ключа

**Проверка подписи:**

```shell
./digital_signature -v -i input.txt -o signature.bin -k public_key.pem
```
- v: режим проверки
- i: входной файл для проверки
- o: файл с подписью
- k: файл публичного ключа
---

# **Тестирование программы**

## **Тест 1: Проверка обработки ошибок при поврежденном публичном ключе**

1. **Повредите публичный ключ:**
```shell
echo "Некорректные данные" > public_key.pem
``` 
2. **Проверка подписи:**
    
```shell
./digital_signature -v -i input.txt -o signature.bin -k public_key.pem
```

**Ожидаемый результат:**

```shell
Ошибка чтения публичного ключа
140708887415680:error:0909006C:PEM routines:get_name:no start line:../crypto/pem/pem_lib.c:745:Expecting: PUBLIC KEY
Ошибка при проверке подписи.
```

## **Тест 2: Проверка обработки ошибок при поврежденном приватном ключе**

1. **Повредите приватный ключ:**
    
```shell
echo "Некорректные данные" > private_key.pem
```
    
2. **Попытка подписать файл:**
    
```shell
./digital_signature -s -i input.txt -o signature.bin -k private_key.pem
```

**Ожидаемый результат:**

```shell
Ошибка чтения приватного ключа
140156451810176:error:0909006C:PEM routines:get_name:no start line:../crypto/pem/pem_lib.c:745:Expecting: ANY PRIVATE KEY
Ошибка при выработке подписи.
```

## **Тест 3: Проверка недействительности подписи при изменении файла**

1. **Подпишите исходный файл:**
    
```shell
./digital_signature -s -i input.txt -o signature.bin -k private_key.pem
```

2. **Измените файл после подписи:**
    
```shell
echo "Дополнительный текст" >> input.txt
```
    
3. **Проверьте подпись:**
```shell
./digital_signature -v -i input.txt -o signature.bin -k public_key.pem
```

**Ожидаемый результат:**

Программа должна сообщить, что подпись недействительна.


## **Тест 4: Использование неправильного типа ключа**

1. **Попытка проверки подписи с использованием приватного ключа:**
    
```shell
./digital_signature -v -i input.txt -o signature.bin -k private_key.pem
```
**Ожидаемый результат:**

```shell
Ошибка чтения публичного ключа
139640573381504:error:0909006C:PEM routines:get_name:no start line:../crypto/pem/pem_lib.c:745:Expecting: PUBLIC KEY
Ошибка при проверке подписи.
```


## **Тест 5: Некорректное использование команды**

1. **Пропуск обязательных аргументов:**
    
```shell
./digital_signature -s -i input.txt -o signature.bin
```

2. **Использование неверных опций:**
    
```shell
./digital_signature -z -i input.txt -o signature.bin -k private_key.pem
```
**Ожидаемый результат:**
```shell
#Программа должна вывести справку по использованию и указать на отсутствие необходимых аргументов или наличие неверных опций.

./digital_signature: invalid option -- 'z'
Использование: ./digital_signature [опции]
Опции:
  -g               Генерация пары ключей
  -s               Подписать файл
  -v               Проверить подпись
  -i <входной>     Входной файл
  -o <выходной>    Выходной файл
  -k <файл ключа>  Файл ключа (приватный или публичный)
  -p <публ. ключ>  Файл публичного ключа (для генерации ключей)
  -b <размер>      Размер ключа в битах (по умолчанию 2048)
  -h               Показать эту справку

```

## **Тест 6: Использование неверного публичного ключа**

1. **Генерация второй пары ключей:**
    
```shell
./digital_signature -g -k private_key2.pem -p public_key2.pem
```

2. **Подпись файла с помощью первого приватного ключа:**
    
```shell
./digital_signature -s -i input.txt -o signature.bin -k private_key.pem
```
    
3. **Проверка подписи с помощью второго публичного ключа:**
```shell
./digital_signature -v -i input.txt -o signature.bin -k public_key2.pem
```

**Ожидаемый результат:**

Программа должна сообщить, что подпись недействительна.


## **Тест 7: Отсутствующие файлы**

1. **Попытка подписать несуществующий файл:**
    
```shell
./digital_signature -s -i nonexistent.txt -o signature.bin -k private_key.pem
```

2. **Попытка проверить подпись с отсутствующим файлом подписи:**
    
```shell
./digital_signature -v -i testfile.txt -o nonexistent_signature.bin -k public_key.pem
```

**Ожидаемый результат:**

```shell
Ошибка открытия входного файла: No such file or directory
Ошибка при проверке подписи.

```
