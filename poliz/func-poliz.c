#include "poliz.h"
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum UsingAnEnumForIntegerConstants { BUFF_SIZE = 30 };

struct PolizState {
    size_t size;
    size_t capacity;
    int *ptr;
    int err;
};

static int push(struct PolizState *arr, int value) {
    if (arr->size + 1 > arr->capacity) {
        size_t newcap = 2 * (arr->capacity + 1);
        int *tmp = realloc(arr->ptr, newcap * sizeof(*tmp));
        if (!tmp) {
            return 1;
        }
        arr->ptr = tmp;
        arr->capacity = newcap;
    }
    arr->ptr[arr->size++] = value;
    return 0;
}

void pop(struct PolizState *arr) {
    --arr->size;
}

static int do_plus(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num1 = state->ptr[state->size - 1];
    pop(state);
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num2 = state->ptr[state->size - 1];
    pop(state);
    int sum;
    if (__builtin_sadd_overflow(num1, num2, &sum)) {
        state->err = PE_INT_OVERFLOW;
        return -state->err;
    }
    if (push(state, sum) == 1) {
        state->err = PE_OUT_OF_MEMORY;
        return -state->err;
    }
    return PE_OK;
}

static int do_minus(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num1 = state->ptr[state->size - 1];
    pop(state);
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num2 = state->ptr[state->size - 1];
    pop(state);
    int diff;
    if (__builtin_ssub_overflow(num2, num1, &diff)) {
        state->err = PE_INT_OVERFLOW;
        return -state->err;
    }
    if (push(state, diff) == 1) {
        state->err = PE_OUT_OF_MEMORY;
        return -state->err;
    }
    return PE_OK;
}

static int do_mul(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num1 = state->ptr[state->size - 1];
    pop(state);
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num2 = state->ptr[state->size - 1];
    pop(state);
    int mul;
    if (__builtin_smul_overflow(num1, num2, &mul)) {
        state->err = PE_INT_OVERFLOW;
        return -state->err;
    }
    if (push(state, mul) == 1) {
        state->err = PE_OUT_OF_MEMORY;
        return -state->err;
    }
    return PE_OK;
}

static int do_inv(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num1 = state->ptr[state->size - 1];
    pop(state);
    int mul;
    if (__builtin_smul_overflow(num1, -1, &mul)) {
        state->err = PE_INT_OVERFLOW;
        return -state->err;
    }
    if (push(state, mul) == 1) {
        state->err = PE_OUT_OF_MEMORY;
        return -state->err;
    }
    return PE_OK;
}

static int do_pop(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    pop(state);
    return PE_OK;
}

static int do_read(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    int num;
    int f1 = scanf("%d", &num);
    if (f1 != 1) {
        state->err = PE_READ_FAILED;
        return -state->err;
    }
    if (push(state, num) == 1) {
        state->err = PE_OUT_OF_MEMORY;
        return -state->err;
    }
    return PE_OK;
}

static int do_write(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num = state->ptr[state->size - 1];
    pop(state);
    printf("%d", num);
    return PE_OK;
}

static int do_end_symbol(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    printf("\n");
    return PE_OK;
}

static int do_swap(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (iextra == 0) {
        return PE_OK;
    }
    if (iextra < 0 || iextra >= state->size) {
        state->err = PE_INVALID_INDEX;
        return -state->err;
    }
    int tmp = state->ptr[state->size - 1];
    state->ptr[state->size - 1] = state->ptr[state->size - iextra - 1];
    state->ptr[state->size - iextra - 1] = tmp;
    return PE_OK;
}

static int do_copy(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (iextra < 0 || iextra >= state->size) {
        state->err = PE_INVALID_INDEX;
        return -state->err;
    }
    int num = state->ptr[state->size - iextra - 1];
    if (push(state, num) == 1) {
        state->err = PE_OUT_OF_MEMORY;
        return -state->err;
    }
    return PE_OK;
}

static int do_div(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num1 = state->ptr[state->size - 1];
    pop(state);
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num2 = state->ptr[state->size - 1];
    pop(state);
    if (num1 == 0) {
        state->err = PE_DIVISION_BY_ZERO;
        return -state->err;
    }
    // printf("%d\n", num1);
    // printf("%d\n", num2);
    int64_t div = (int64_t)num2 / (int64_t)num1;
    int64_t mod = (int64_t)num2 % (int64_t)num1;
    if (num1 < 0) {
        while (mod < 0) {
            mod -= num1;
            ++div;
        }
    } else {
        while (mod < 0) {
            mod += num1;
            --div;
        }
    }
    // printf("%lld", div);
    if ((int64_t)INT_MIN > div || div > (int64_t)INT_MAX) {
        state->err = PE_INT_OVERFLOW;
        return -state->err;
    }
    if (push(state, div) == 1) {
        state->err = PE_OUT_OF_MEMORY;
        return -state->err;
    }

    return PE_OK;
}

static int do_mod(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num1 = state->ptr[state->size - 1];
    pop(state);
    if (state->size == 0) {
        state->err = PE_STACK_UNDERFLOW;
        return -state->err;
    }
    int num2 = state->ptr[state->size - 1];
    pop(state);
    if (num1 == 0) {
        state->err = PE_DIVISION_BY_ZERO;
        return -state->err;
    }
    int64_t div = (int64_t)num2 / (int64_t)num1;
    int64_t mod = (int64_t)num2 % (int64_t)num1;
    if (num1 < 0) {
        while (mod < 0) {
            mod -= num1;
            ++div;
        }
    } else {
        while (mod < 0) {
            mod += num1;
            --div;
        }
    }

    if ((int64_t)INT_MIN > mod || mod > (int64_t)INT_MAX) {
        state->err = PE_INT_OVERFLOW;
        return -state->err;
    }
    if (push(state, mod) == 1) {
        state->err = PE_OUT_OF_MEMORY;
        return -state->err;
    }

    return PE_OK;
}

static int do_push_in_stack(struct PolizState *state, int iextra) {
    if (state->err) {
        return -state->err;
    }
    if (push(state, iextra) == 1) {
        state->err = PE_OUT_OF_MEMORY;
        return -state->err;
    }
    return PE_OK;
}

struct Vector {
    size_t size;
    size_t capacity;
    struct PolizItem *ptr;
};

static int push_in_vec(struct Vector *arr, struct PolizItem value) {
    if (arr->size + 1 > arr->capacity) {
        size_t newcap = 2 * (arr->capacity + 1);
        struct PolizItem *tmp = realloc(arr->ptr, newcap * sizeof(*tmp));
        if (!tmp) {
            return 1;
        }
        arr->ptr = tmp;
        arr->capacity = newcap;
    }
    arr->ptr[arr->size++] = value;
    return 0;
}

struct PolizItem *poliz_compile(const char *str) {
    struct Vector *ans = calloc(1, sizeof(*ans));
    ans->size = 0;
    ans->capacity = 0;
    ans->ptr = NULL;
    while (*str != '\0') {
        if (isspace(*str)) {
            ++str;
            continue;
        }
        if (*str == 'r') {
            struct PolizItem elem;
            elem.handler = do_read;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == 'w') {
            struct PolizItem elem;
            elem.handler = do_write;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == 'n') {
            struct PolizItem elem;
            elem.handler = do_end_symbol;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == ';') {
            struct PolizItem elem;
            elem.handler = do_pop;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == '#') {
            struct PolizItem elem;
            elem.handler = do_inv;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == '+' && (isspace(*(str + 1)) || *(str + 1) == '\0')) {
            struct PolizItem elem;
            elem.handler = do_plus;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == '-' && (isspace(*(str + 1)) || *(str + 1) == '\0')) {
            struct PolizItem elem;
            elem.handler = do_minus;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == '*') {
            struct PolizItem elem;
            elem.handler = do_mul;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == '/') {
            struct PolizItem elem;
            elem.handler = do_div;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == '%') {
            struct PolizItem elem;
            elem.handler = do_mod;
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            ++str;
            continue;
        }
        if (*str == 'd') {
            struct PolizItem elem;
            elem.handler = do_copy;
            ++str;
            char buff[BUFF_SIZE];
            char *end;
            size_t ptr = 0;
            while (!(isspace(*str) || *str == '\0')) {
                buff[ptr++] = *str;
                ++str;
            }
            if (ptr == 0) {
                elem.iextra = 0;
            } else {
                buff[ptr] = 0;
                int num = strtol(buff, &end, 10);
                elem.iextra = num;
            }
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            continue;
        }
        if (*str == 's') {
            struct PolizItem elem;
            elem.handler = do_swap;
            ++str;
            char buff[BUFF_SIZE];
            char *end;
            size_t ptr = 0;
            while (!(isspace(*str) || *str == '\0')) {
                buff[ptr++] = *str;
                ++str;
            }
            if (ptr == 0) {
                elem.iextra = 1;
            } else {
                buff[ptr] = 0;
                int num = strtol(buff, &end, 10);
                elem.iextra = num;
            }
            if (push_in_vec(ans, elem) == 1) {
                return NULL;
            }
            continue;
        }
        struct PolizItem elem;
        elem.handler = do_push_in_stack;
        char buff[BUFF_SIZE];
        char *end;
        size_t ptr = 0;
        while (!(isspace(*str) || *str == '\0')) {
            buff[ptr++] = *str;
            ++str;
        }
        buff[ptr] = 0;
        int num = strtol(buff, &end, 10);
        elem.iextra = num;
        // printf("%d\n", num);
        if (push_in_vec(ans, elem) == 1) {
            return NULL;
        }
    }
    struct PolizItem elem;
    elem.handler = NULL;
    if (push_in_vec(ans, elem) == 1) {
        return NULL;
    }

    struct PolizItem *tmp = ans->ptr;
    free(ans);
    return tmp;
}

struct PolizState *poliz_new_state(void) {
    struct PolizState *ans = calloc(1, sizeof(*ans));
    ans->capacity = 0;
    ans->size = 0;
    ans->ptr = NULL;
    ans->err = 0;
    return ans;
}

int poliz_last_error(struct PolizState *state) {
    return state->err;
}

void poliz_free_state(struct PolizState *state) {
    free(state->ptr);
    free(state);
}