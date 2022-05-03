#ifndef CORTH_H
#define CORTH_H

#include "macros.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>

//máximo de dados que podem ser empilhados
#define MAX_STACK_CAP 1024*200

typedef enum DataType DataType;
typedef enum Op Op;
typedef enum WordType WordType;
typedef enum KeyWord KeyWord;

typedef struct Stack Stack;
typedef struct Word Word;

//tipos de dados aceitos
enum DataType {
    DT_INT,
    DT_CHAR,
    DT_STRING,
    DT_BOOL,
    DT_FLOAT
};

//operadores aceitos
enum Op {
    OP_PLUS = '+',
    OP_MINUS = '-',
    OP_DIVISION = '/',
    OP_MULTIPLY = '*',
    OP_PRINT = '.',
    OP_EQUAL = '=',
    OP_PRINT_STACK = '?'
};

//tipos de palavras
enum WordType {
    WT_DATA_TYPE,
    WT_OP,
    WT_KEY_WORD,
    WT_NONE = ' '
};

//keywords da linguagem corth, é usado como índice do array com as keywords
enum KeyWord {
    KW_DROP,
    KW_DUP,
    KW_SWAP,
    KW_OVER,
    KW_ROT,
    KW_MACRO,
    KW_END,
    KW_SET_FLOAT_PRECISION,

    KW_CAST_CHAR,
    KW_CAST_INT,
    KW_CAST_BOOL
};

//a pilha da linguagem
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

void start(char *path);

//operaçoes da pilha
void stack_push(Stack *stack, Word *word);
Word stack_pop(Stack *stack);


// dup  ( a -- a a )
// drop ( a -- )
// swap ( a b -- b a )
// over ( a b -- a b a )
// rot  ( a b c -- b c a )
static const char const *key_word[] = {
    "drop",
    "dup",
    "swap",
    "over",
    "rot",
    "macro",//TODO
    "end",//TODO
    "set_float_precision",

    "cast(char)",
    "cast(int)",
    "cast(bool)",
    "cast(float)"
};

static const int KEY_WORD_COUNT = sizeof(key_word) / sizeof(*key_word);
static long long float_precision = 5;

#endif /* CORTH_H */