#ifndef CORTH_H
#define CORTH_H

#include "mem.h"
#include "macros.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>

#define DEBUG 0
#define MEM_CAP 1024*1024
#define MAX_STACK_CAP 1024*200
#define MAX_MACRO_NAMES 1024*100
#define MAX_NUMBER_OF_STRINGS 1024*200
#define MAX_NUMBER_OF_FUNCTIONS 1024*200

typedef enum DataType DataType;
typedef enum Op Op;
typedef enum WordType WordType;
typedef enum KeyWord KeyWord;

typedef struct Word Word;
typedef struct Macro Macro;
typedef struct MacroVec MacroVec;
typedef struct DataTypeStack DataTypeStack;
typedef struct WordVec WordVec;
typedef struct String String;
typedef struct StringVec StringVec;
typedef struct Func Func;
typedef struct FuncVec FuncVec;

enum DataType {
    DT_INT,
    DT_CHAR,
    DT_STRING,
    DT_BOOL,
    DT_FLOAT,
    DT_PTR
};

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

enum WordType {
    WT_DATA_TYPE,
    WT_OP,
    WT_KEY_WORD,
    WT_MACRO,
    WT_FUNC,
    WT_NONE = ' ',
    WT_COMMENT = '#'
};

enum KeyWord {
    KW_DROP,
    KW_DUP,
    KW_SWAP,
    KW_OVER,
    KW_ROT,
    
    KW_WHILE,
    KW_DO,

    KW_MACRO,
    KW_END,

    KW_IF,
    KW_ELSE,

    KW_CAST_CHAR,
    KW_CAST_INT,
    KW_CAST_BOOL,
    KW_CAST_FLOAT,
    KW_CAST_PTR,
    KW_TRUE,
    KW_FALSE,

    KW_SHR,
    KW_SHL,
    KW_MEM,
    
    KW_READ8,
    KW_READ16,
    KW_READ32,
    KW_READ64,
    
    KW_WRITE8,
    KW_WRITE16,
    KW_WRITE32,
    KW_WRITE64,
    KW_FUNC,
    KW_COUNT
};

struct DataTypeStack
{
    DataType *types;
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
    };
};

void compile(char *path);

struct WordVec
{
    Word **words;
    uint64_t size, capacity;
};

void word_vec_push(WordVec *word_vec, Word* str);
char *word_vec_pop(WordVec *word_vec);
void word_vec_clear(WordVec *word_vec);
void word_vec_remove_at(WordVec *word_vec, uint64_t pos);
uint64_t word_vec_get_by_id(WordVec *word_vec, uint64_t id);

struct Macro
{
    char *name;
    Word *words;
    uint64_t size;
};
struct MacroVec
{
    Macro **macros;
    uint64_t size, capacity;
};

void create_macro(FILE *file, uint64_t *id);
void macro_vec_push(Macro* macro, Word *word);
void clear_macro_vec(MacroVec *macroVec);

struct String
{
    char *str;
    uint64_t size;
};

struct StringVec
{
    String **strings;
    uint64_t size, capacity;
};

void string_vec_push(StringVec *string_vec, Word *word);
void clear_string_vec(StringVec *string_vec);

struct Func
{
    char *name;
    WordVec word_vec;
    uint64_t size;
};

struct FuncVec
{
    Func **funcs;
    uint64_t size, capacity;
};

void create_func(FILE *file, uint64_t *id);

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

    "while",
    "do",

    "macro",
    "end",

    "if",
    "else",

    "cast(char)",
    "cast(int)",
    "cast(bool)",
    "cast(float)",
    "cast(ptr)",

    "true",
    "false",

    "shr",
    "shl",
    "mem",
    "r8",
    "r16",
    "r32",
    "r64",
    "w8",
    "w16",
    "w32",
    "w64",
    "func"
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

static MacroVec macro_vec = {0};
static StringVec str_vec = {0};
static FuncVec func_vec = {0};

void print_data_type_stack(DataTypeStack *data_type_stack);
int32_t parse_file(WordVec *word_vec, FILE *file);
void write_fasm_file(FILE *fasm_file, Word *word, DataTypeStack *data_type_stack, uint64_t *stack_size, uint64_t index);
Word get_word(char *str);
bool is_number(char *str);
bool is_float(char *str);
void create_macro_if_block(Macro *macro, uint64_t *i, uint64_t macro_vec_index, uint64_t parsed_file_size);
void create_macro_while_block(Macro *macro, uint64_t *i, uint64_t macro_vec_index, uint64_t parsed_file_size);
void create_if_block(WordVec *parsed_file, uint64_t *i);
void create_while_block(WordVec *parsed_file, uint64_t *i);
void create_blocks(WordVec *parsed_file, uint64_t *i);
void print_parsed_file(WordVec *word_vec);
#endif /* CORTH_H */