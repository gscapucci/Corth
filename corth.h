#ifndef CORTH_H
#define CORTH_H

#include "mem.h"
#include "macros.h"
#include "syscall.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>

//máximo de dados que podem ser empilhados
#define MAX_STACK_CAP 1024*200
#define MAX_MACRO_NAMES 1024*100
#define MAX_NUMBER_OF_STRINGS 1024*200

typedef enum DataType DataType;
typedef enum Op Op;
typedef enum WordType WordType;
typedef enum KeyWord KeyWord;

typedef struct Stack Stack;
typedef struct Word Word;
typedef struct Macro Macro;
typedef struct MacroVec MacroVec;
typedef struct DataTypeStack DataTypeStack;
typedef struct WordVec WordVec;
typedef struct String String;
typedef struct WordVec WordVec;

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
    OP_ADD,
    OP_SUB,
    OP_DIVMOD,
    OP_MULTIPLY,
    OP_PRINT,
    OP_EQUAL,
    OP_LESS,
    OP_GREAT,
    OP_LESS_EQUAL,
    OP_GREAT_EQUAL,
    OP_NOT,
    OP_PRINT_STACK,
    OP_COUNT
};

//tipos de palavras
enum WordType {
    WT_DATA_TYPE,
    WT_OP,
    WT_KEY_WORD,
    WT_MACRO,
    WT_SYSCALL,
    WT_NONE = ' ',
    WT_COMMENT = '#'
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

    KW_IF,
    KW_ELSE,

    KW_CAST_CHAR,
    KW_CAST_INT,
    KW_CAST_BOOL,
    KW_CAST_FLOAT,

    KW_SYSCALL,

    KW_TRUE,
    KW_FALSE,

    KW_COUNT
};

//a pilha da linguagem
struct Stack
{
    Word *item[MAX_STACK_CAP];
    uint64_t size;
};

struct DataTypeStack
{
    DataType types[MAX_STACK_CAP];
    uint64_t size;
};

void data_type_stack_push(DataTypeStack *dt_stack, DataType *data_type);
DataType data_type_stack_pop(DataTypeStack *dt_stack);

struct Word
{
    void *value;
    uint64_t size, id;
    WordType type;
    struct
    {
        uint64_t coll, line;
    }pos;
    union
    {
        Op op;
        DataType data_type;
        KeyWord key_word;
        Syscall sys_call;
    };
};

void compile(char *path);
//operaçoes da pilha
void stack_push(Stack *stack, Word *word);
Word stack_pop(Stack *stack);

struct Macro
{
    char *name;
    Stack macro_stack;
};
struct MacroVec
{
    Macro *macros[MAX_MACRO_NAMES];
    uint64_t size;
};

void create_macro(FILE *file);
void macro_vec_push(Word *word);
Word macro_vec_get_at(uint64_t index, uint64_t pos);
void clear_macro_vec();


struct WordVec
{
    Word *words[MAX_NUMBER_OF_STRINGS];
    uint64_t size;
};

void word_vec_push(WordVec *word_vec, Word* str);
char *word_vec_pop(WordVec *word_vec);
void word_vec_clear(WordVec *word_vec);
void word_vec_remove_at(WordVec *word_vec, uint64_t pos);
uint64_t word_vec_get_by_id(WordVec *word_vec, uint64_t id);

void read_corth_file(FILE *fasm_file, WordVec *word_vec);

static const char const *type_names[] = {
    "int",
    "char",
    "str",
    "bool",
    "float"
};

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

    "macro",
    "end",

    "if",
    "else",

    "cast(char)",
    "cast(int)",
    "cast(bool)",
    "cast(float)",

    "syscall",

    "true",
    "false"
};

static const char const *ops[] = {
    "+",
    "-",
    "/%",
    "*",
    ".",
    "=",
    "<",
    ">",
    "<=",
    ">=",
    "!",
    "?"
};
int32_t parse_file(WordVec *word_vec, FILE *file);
void write_fasm_file(FILE *fasm_file, WordVec *parsed_file);
Word get_word(char *str);
bool is_number(char *str);
bool is_float(char *str);
void create_if_block(WordVec *parsed_file, uint64_t *i);
void create_blocks(WordVec *parsed_file);
void write_syscall(FILE *fasm_file, DataTypeStack *data_type_stack, Syscall *syscall);
void print_parsed_file(WordVec *word_vec);
#endif /* CORTH_H */