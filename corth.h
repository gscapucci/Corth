#ifndef CORTH_H
#define CORTH_H

#include "macros.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>


#define MAX_STACK_CAP 1024*200

typedef enum DataType DataType;
typedef enum Op Op;
typedef enum WordType WordType;
typedef enum KeyWord KeyWord;

typedef struct Stack Stack;
typedef struct Word Word;

enum DataType {
    DT_INT,
    DT_CHAR,
    DT_STRING
};

enum Op {
    OP_PLUS = '+',
    OP_MINUS = '-',
    OP_DIVISION = '/',
    OP_MULTIPLY = '*',
    OP_PRINT = '.'
};

enum WordType {
    WT_DATA_TYPE,
    WT_OP,
    WT_KEY_WORD,
    WT_NONE = ' '
};

enum KeyWord {
    KW_DROP,
    KW_DUP,
    KW_SWAP,
    KW_OVER
};

struct Stack
{
    Word *item[MAX_STACK_CAP];
    size_t size;
};

struct Word
{
    void *value;
    size_t size;
    WordType type;
    union
    {
        Op op;
        DataType data_type;
        KeyWord key_word;
    };
};

void start(char *asdfasdf);

void stack_push(Stack *stack, Word *word);
Word stack_pop(Stack *stack);


// dup  ( a -- a a )
// drop ( a -- )
// swap ( a b -- b a )
// over ( a b -- a b a )
// rot  ( a b c -- b c a )
static char *key_word[] = {
    "drop",
    "dup",
    "swap",
    "over"
};

static const int KEY_WORD_COUNT = sizeof(key_word) / sizeof(*key_word);

#endif /* CORTH_H */