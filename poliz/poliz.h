#pragma once

// opaque structure for poliz calculation state
struct PolizState;

// poliz operation handler
typedef int (*poliz_func_t)(struct PolizState *state, int iextra);

struct PolizItem
{
    poliz_func_t handler;
    int iextra;
};

// runtime errors
enum
{
    PE_OK,  // no error
    PE_STACK_UNDERFLOW, // not enough elements on stack
    PE_INVALID_INDEX, // s or d operations refer to invalid index
    PE_DIVISION_BY_ZERO,
    PE_INT_OVERFLOW,
    PE_READ_FAILED, // read from stdin failed convert integer
    PE_OUT_OF_MEMORY,
};

struct PolizItem *poliz_compile(const char *str);

struct PolizState *poliz_new_state(void);
void poliz_free_state(struct PolizState *state);
int poliz_last_error(struct PolizState *state);
