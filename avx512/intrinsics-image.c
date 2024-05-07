#include <immintrin.h>

enum UsingAnEnumForIntegerConstants {
    SZ = 16,
    STEP4 = 4,
    STEP3 = 3,
    STEP12 = 12,
};

void swap_line(const float *src, float *dst, unsigned n, const int order[4],
               float extra) {
    int new_order[SZ];
    for (size_t i = 0; i < STEP4; ++i) {
        new_order[i] = order[i];
    }
    for (size_t i = 0; i < STEP4; ++i) {
        new_order[i + STEP4] = order[i] + STEP4;
    }
    for (size_t i = 0; i < STEP4; ++i) {
        new_order[i + 2 * STEP4] = order[i] + 2 * STEP4;
    }
    for (size_t i = 0; i < STEP4; ++i) {
        new_order[i + 3 * STEP4] = order[i] + 3 * STEP4;
    }
    int order_extra[SZ];
    for (size_t i = 0; i < STEP3; ++i) {
        order_extra[i] = i;
    }
    order_extra[STEP3] = STEP12;
    for (size_t i = 0; i < STEP3; ++i) {
        order_extra[i + STEP4] = i + STEP3;
    }
    order_extra[STEP4 + STEP3] = STEP12 + 1;
    for (size_t i = 0; i < STEP3; ++i) {
        order_extra[i + 2 * STEP4] = i + 2 * STEP3;
    }
    order_extra[2 * STEP4 + STEP3] = STEP12 + 2;
    for (size_t i = 0; i < STEP3; ++i) {
        order_extra[i + 3 * STEP4] = i + 3 * STEP3;
    }
    order_extra[3 * STEP4 + STEP3] = STEP12 + 3;
    __m512 mask = _mm512_set_ps(0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
    __m512 mask_with_extra = _mm512_set_ps(extra, extra, extra, extra, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0, 0);
    __m512i avx_order = _mm512_loadu_epi32(new_order);
    __m512i avx_order_extra = _mm512_loadu_epi32(order_extra);
    for (size_t i = 0; i < 3 * n; i += STEP12) {
        __m512 data = _mm512_loadu_ps(&src[i]);
        data = _mm512_mul_ps(data, mask);
        data = _mm512_add_ps(data, mask_with_extra);
        data = _mm512_permutexvar_ps(avx_order_extra, data);
        data = _mm512_permutexvar_ps(avx_order, data);
        _mm512_storeu_ps(&dst[(i / STEP12) * SZ], data);
    }
}