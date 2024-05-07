#include "bigint.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

enum token_kind { ADD, SUB, MUL, DIV, LP, RP, NUM, EOL, END };

struct token_t {
        enum token_kind kind;
        bigint_t *value;
};

static struct token_t current_token;

static void error(const char *msg, ...)
{
        va_list ap;

        va_start(ap, msg);
        fputs("error: ", stderr);
        vfprintf(stderr, msg, ap);
        fputc('\n', stderr);
        va_end(ap);

        exit(1);
}

#define BUFFER_SIZE (100*1024)
static char buffer[BUFFER_SIZE];

static void next_token(void)
{
        int c;
        size_t len;

        assert(current_token.kind != END && "Can't get token after END!");

        do {
                c = getchar();
        } while (c == ' ' || c == '\t');

        switch (c) {
        case '+':
                current_token.kind = ADD;
                return;
        case '-':
                current_token.kind = SUB;
                return;
        case '*':
                current_token.kind = MUL;
                return;
        case '/':
                current_token.kind = DIV;
                return;
        case '(':
                current_token.kind = LP;
                return;
        case ')':
                current_token.kind = RP;
                return;
        case '\n':
                current_token.kind = EOL;
                return;
        case EOF:
                current_token.kind = END;
                return;
        }

        len = 0;
        while (c >= '0' && c <= '9') {
                if (len == BUFFER_SIZE) {
                        error("number too long!");
                }

                buffer[len++] = c;
                c = getchar();
        }
        if (len > 0) {
                ungetc(c, stdin);
                current_token.kind = NUM;
                current_token.value = bigint_create_str(len, buffer);
                return;
        }

        error("unexpected character: '%c'", c);
}

/*
   Grammar:

   <expr>   ::= <sum> EOL | END
   <sum>    ::= <term> (ADD <term> | SUB <term>)*
   <term>   ::= <factor> (MUL <factor> | DIV <factor>)*
   <factor> ::= SUB <factor> | LP <sum> RP | <number>

   The functions below parse a string of tokens according to the grammar and
   return the corresponding bigint value. expr() leaves the last token
   unconsumed to avoid blocking on input.
*/

static bigint_t *expr(void);
static bigint_t *sum(void);
static bigint_t *term(void);
static bigint_t *factor(void);

static bigint_t *expr(void)
{
        bigint_t *res;

        if (current_token.kind == END) {
                return NULL;
        }

        res = sum();

        if (current_token.kind != EOL) {
                error("trailing character(s)");
        }

        return res;
}

static bigint_t *sum(void)
{
        bigint_t *x, *y, *t;

        x = term();

        while (true) {
                if (current_token.kind == ADD) {
                        next_token();
                        y = term();
                        t = bigint_add(x, y);
                        free(x);
                        free(y);
                        x = t;
                } else if (current_token.kind == SUB) {
                        next_token();
                        y = term();
                        t = bigint_sub(x, y);
                        free(x);
                        free(y);
                        x = t;
                } else {
                        break;
                }
        }

        return x;
}

static bigint_t *term(void)
{
        bigint_t *x, *y, *t;

        x = factor();

        while (true) {
                if (current_token.kind == MUL) {
                        next_token();
                        y = factor();
                        t = bigint_mul(x, y);
                        free(x);
                        free(y);
                        x = t;
                } else if (current_token.kind == DIV) {
                        next_token();
                        y = factor();
                        if (bigint_is_zero(y)) {
                                error("division by zero!");
                        }
                        t = bigint_div(x, y);
                        free(x);
                        free(y);
                        x = t;
                } else {
                        break;
                }
        }

        return x;
}

static bigint_t *factor(void)
{
        bigint_t *x, *res;

        if (current_token.kind == SUB) {
                next_token();
                x = factor();
                res = bigint_neg(x);
                free(x);
        } else if (current_token.kind == LP) {
                next_token();
                res = sum();
                if (current_token.kind != RP) {
                        error("expected ')'");
                }
                next_token();
        } else if (current_token.kind == NUM) {
                res = current_token.value;
                next_token();
        } else {
                error("expected '-', number or '('");
        }

        return res;
}

int main()
{
        bigint_t *x;

        for (;;) {
                next_token();
                x = expr();
                if (x == NULL) {
                        break;
                }
                bigint_print(x);
                free(x);
                printf("\n");
        }

        return 0;
}
