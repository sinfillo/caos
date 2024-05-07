# hamming-code
## Условие задачи
Напишите функции `encode` и `decode` кодирующие и декодирующие данные таким образом, чтобы случайный один (или ноль) [битфлип](https://en.wikipedia.org/wiki/RAM_parity) не мог помешать их восстановить

`void encode(const void *data, void *encoded, size_t n)` кодирует $n$ байт по указателю `data` в указатель `encoded`

`void decode(const void *encoded, void *data, size_t n)` декодирует $n$ байт из указателя `encoded` в указатель `data` ($n$ - размер изначальных данных до кодирования)

Гарантируется что массив `data` содержит ровно $n$ байт

Гарантируется что массив `encoded` содержит ровно $2*n$ байт
