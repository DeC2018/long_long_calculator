#ifndef BIGINT_H
#define BIGINT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct bigint_t bigint_t;

/* Create a bigint from n-length array u. Leading zeros or n = 0 are allowed. */
bigint_t *bigint_create(int n, const uint32_t *u, bool negative);

/* Create a bigint from n-character string str, consisting of one or more
   decimal characters, with an optional preceding hyphen. */
bigint_t *bigint_create_str(int n, const char *str);

/* Get the maximum required string length for x. */
size_t bigint_max_stringlen(const bigint_t *x);

/* Convert bigint to decimal string. */
void bigint_tostring(const bigint_t *x, char *str);

/* Print a bigint to stdout in decimal. */
void bigint_print(const bigint_t *x);

/* Arithmetic. Division does truncation towards zero, and the remainder will
   have the same sign as the divisor. Division by zero is not allowed. */
bigint_t *bigint_add(const bigint_t *x, const bigint_t *y);
bigint_t *bigint_sub(const bigint_t *x, const bigint_t *y);
bigint_t *bigint_mul(const bigint_t *x, const bigint_t *y);
bigint_t *bigint_div(const bigint_t *x, const bigint_t *y);
bigint_t *bigint_rem(const bigint_t *x, const bigint_t *y);
bigint_t *bigint_neg(const bigint_t *x);

/* Comparison: returns -1 if x < y, 1 if x > y, and 0 if they are equal. */
int bigint_cmp(const bigint_t *x, const bigint_t *y);

/* Check whether x is zero. */
bool bigint_is_zero(const bigint_t *x);

#endif