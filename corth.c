#include "corth.h"

void escape_str(char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if(str[i] == '\\')
        {
            switch (str[i + 1])
            {
            case 'n':
                str[i] = '\n';
                break;
            case 'a':
                str[i] = '\a';
                break;
            case 'b':
                str[i] = '\b';
                break;
            case 't':
                str[i] = '\t';
                break;
            case 'v':
                str[i] = '\v';
                break;
            case 'f':
                str[i] = '\f';
                break;
            case 'r':
                str[i] = '\r';
                break;
            case 'e':
                str[i] = '\e';
                break;
            case '\"':
                str[i] = '\"';
                break;
            case '0':
                str[i] = '\0';
                break;
            default:
                ERROR("trying to escape inescapable");
                break;
            }
            for (size_t j = i + 1; j < strlen(str); j++)
            {
                str[j] = str[j + 1];
            }
        }
    }    
}

int32_t parse_file(WordVec *word_vec, FILE *file)
{
    char str[1024] = {0};
    uint16_t str_len = 0;
    char c;
    Word word = {0};
    while(true)
    {
        c = getc(file);
        if(str_len == 0)
        {
            if(c == EOF) return EOF;
            if(c == '\'')
            {
                str[str_len++] = c;
                c = getc(file);
                if(c == EOF)
                {
                    ERROR("not expect end of file");
                }
                while (!(c == '\'' && str[str_len - 1] != '\\'))
                {
                    if(c == EOF)
                    {
                        ERROR("not expect end of file");
                    }
                    str[str_len++] = c;
                    c = getc(file);
                }
                str[str_len++] = c;
                escape_str(str);
                if(strlen(str) > 3)
                {
                    ERROR("lengh of char must be one");
                }
                word = get_word(str);
                word_vec_push(word_vec, &word);
                for (size_t i = 0; str[i] != 0; i++)
                {
                    str[i] = 0;
                }
                str_len = 0;
                continue;
            }
            if(c == '\"')
            {
                str[str_len++] = c;
                c= getc(file);
                if(c == EOF)
                {
                    ERROR("not expect end of file");
                }
                while (c != '\"' || str[str_len - 1] != '\\')
                {
                    if(c == EOF)
                    {
                        ERROR("not expect end of file");
                    }
                    str[str_len] = c;
                    c = getc(file);
                }
                str[str_len++] = c;
                escape_str(str);
                word = get_word(str);
                word_vec_push(word_vec, &word);
                for (size_t i = 0; str[i] != 0; i++)
                {
                    str[i] = 0;
                }
                str_len = 0;
                continue;
            }
        }
        if(c == '#')
        {
            while ((c = getc(file)) != '\n');
        }
        if(c != ' ' && c != '\n' && c != EOF)
        {
            str[str_len++] = c;
            continue;
        }

        if(str_len > 0)
        {
            word = get_word(str);   
            word_vec_push(word_vec, &word);
            for (size_t i = 0; str[i] != 0; i++)
            {
                str[i] = 0;
            }
            str_len = 0;
            continue;
        }
        if(c == EOF) return EOF;
    }
}

void word_vec_push(WordVec *word_vec, Word* word)
{
    if(word_vec->size >= MAX_NUMBER_OF_STRINGS)
    {
        ERROR("vector overflow");
    }
    if(word_vec->words[word_vec->size] != NULL)
    {
        if(word_vec->words[word_vec->size]->size > 0)
        {
            free(word_vec->words[word_vec->size]->value);
        }
        free(word_vec->words[word_vec->size]);
        word_vec->words[word_vec->size] = NULL;
    }
    word_vec->words[word_vec->size] = malloc(sizeof(Word));
    word_vec->words[word_vec->size]->type = word->type;
    word_vec->words[word_vec->size]->size = word->size;
    if(word->size > 0)
    {
        word_vec->words[word_vec->size]->value = malloc(word->size);
        memcpy(word_vec->words[word_vec->size]->value, word->value, word->size);
    }
    switch (word->type)
    {
    case WT_DATA_TYPE:
        word_vec->words[word_vec->size]->data_type = word->data_type;
        break;
    case WT_KEY_WORD:
        word_vec->words[word_vec->size]->key_word = word->key_word;
        break;
    case WT_OP:
        word_vec->words[word_vec->size]->op = word->op;
        break;
    case WT_SYSCALL:
        word_vec->words[word_vec->size]->sys_call = word->sys_call;
        break;
    case WT_COMMENT:
    case WT_MACRO:
    case WT_NONE:
        break;
    default:
        ERROR("unknown word type");
        break;
    }
    word_vec->size++;
}

void word_vec_clear(WordVec *word_vec)
{
    for (size_t i = 0; i < word_vec->size; i++)
    {
        free(word_vec->words[i]->value);
        free(word_vec->words[i]);
    }
    word_vec->size = 0;
}

void data_type_stack_push(DataTypeStack *dt_stack, DataType *data_type)
{
    if(dt_stack->size >= MAX_STACK_CAP)
    {
        ERROR("stack overflow");
    }
    dt_stack->types[dt_stack->size++] = *data_type;
}

DataType data_type_stack_pop(DataTypeStack *dt_stack)
{
    if(dt_stack->size == 0)
    {
        ERROR("stack underflow");
    }
    return dt_stack->types[--dt_stack->size];
}

bool is_number(char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if(str[i] < '0' || str[i] > '9')
        {
            return false;
        }
    }
    return true;
}

bool is_float(char *str)
{
    uint8_t dot_count = 0;
    if(str[0] == '.' || str[strlen(str) - 1] == '.')
    {
        return false;
    }
    for (size_t i = 0; i < strlen(str); i++)
    {
        if(str[i] == '.')
        {
            dot_count++;
            if(dot_count >= 2)
            {
                return false;
            }
        }
        else if(str[i] < '0' || str[i] > '9')
        {
            return false;
        }
    }
    return true;
}

Word get_word(char *str)
{
    Word word;
    for (size_t i = 0; i < KW_COUNT; i++)
    {
        if(!strcmp(str, key_word[i]))
        {
            word.type = WT_KEY_WORD;
            word.key_word = (KeyWord)i;
            word.size = 0;
            word.value = NULL;
            return word;
        }
    }
    for (size_t i = 0; i < SYSCALL_COUNT; i++)
    {
        if(!strcmp(str, syscalls[i]))
        {
            word.type = WT_SYSCALL;
            word.sys_call = (Syscall)i;
            word.size = 0;
            word.value = NULL;
            return word;
        }
    }
    if(is_number(str))
    {
        word.type = WT_DATA_TYPE;
        word.data_type = DT_INT;
        word.size = sizeof(long long);
        word.value = malloc(word.size);
        memcpy(word.value, &(long long){strtoll(str, NULL, 10)}, word.size);
        return word;
    }
    if(is_float(str))
    {
        word.type = WT_DATA_TYPE;
        word.data_type = DT_FLOAT;
        word.size = sizeof(long double);
        word.value = malloc(word.size);
        memcpy(word.value, &(long double){strtold(str, NULL)}, word.size);
        return word;
    }
    if(strlen(str) == 1)
    {
        if(str[0] == ' ')
        {
            ERROR("Not expected none");
        }
        word.type = WT_OP;
        switch (str[0])
        {
        case OP_DIVISION:
        case OP_EQUAL:
        case OP_SUB:
        case OP_MULTIPLY:
        case OP_ADD:
        case OP_PRINT:
        case OP_PRINT_STACK:
            word.op = (Op)str[0];
            break;
        default:
            fprintf(stderr, "%s ", str);
            ERROR("unknown op");
            break;
        }
        word.size = 0;
        word.value = NULL;
        return word;
    }
    if(strlen(str) == 3 && (str[0] == '\'' && str[2] == '\''))
    {
        word.type = WT_DATA_TYPE;
        word.data_type = DT_CHAR;
        word.size = sizeof(char);
        word.value = (char *)malloc(word.size);
        memcpy(word.value, &str[1], word.size);
        return word;
    }
    if(str[0] == '\"' && str[strlen(str) - 1] == '\"')
    {
        word.type = WT_DATA_TYPE;
        word.data_type = DT_STRING;
        word.size = strlen(str) + 1;
        word.value = (char *)malloc(word.size);
        memcpy(word.value, str, word.size);
        return word;
    }
    ERROR("unknown word");
}

void write_fasm_file(FILE *fasm_file, WordVec *parsed_file)
{
    static DataTypeStack data_type_stack = {0};
    DataType aux[3] = {0};
    for (size_t i = 0; i < parsed_file->size; i++)
    {
        switch (parsed_file->words[i]->type)
        {
        case WT_COMMENT:
            break;
        case WT_DATA_TYPE:
            data_type_stack_push(&data_type_stack, &parsed_file->words[i]->data_type);
            switch (parsed_file->words[i]->data_type)
            {
            case DT_BOOL:
                UNIMPLEMENTED("DT_BOOL");
                break;
            case DT_CHAR:
                fprintf(fasm_file, ";;--PUSH_CHAR ascii(%lld)--;;\n", (long long)*(char *)parsed_file->words[i]->value);
                fprintf(fasm_file, "    push %lld\n", (long long)*(char *)parsed_file->words[i]->value);
                break;
            case DT_FLOAT:
                UNIMPLEMENTED("DT_FLOAT");
                break;
            case DT_INT:
                fprintf(fasm_file, ";;--PUSH_INT %lld--;;\n", *(long long *)parsed_file->words[i]->value);
                fprintf(fasm_file, "    mov rax, %lld\n", *(long long *)parsed_file->words[i]->value);
                fprintf(fasm_file, "    push rax\n");
                break;
            case DT_STRING:
                UNIMPLEMENTED("DT_STRING");
                break;
            default:
                ERROR("unknown DataType");
                break;
            }
            break;
        case WT_KEY_WORD:
            switch (parsed_file->words[i]->key_word)
            {
            case KW_SYSCALL:
                aux[0] = data_type_stack_pop(&data_type_stack);
                if(aux[0] != DT_INT)
                {
                    ERROR("expect int");
                }
                write_syscall(fasm_file, &data_type_stack, &(Syscall){aux[0]});
                break;
            case KW_IF:
                aux[0] = data_type_stack_pop(&data_type_stack);
                if(aux[0] != DT_BOOL)
                {
                    ERROR("expect bool");
                }
                UNIMPLEMENTED("KW_IF");
                break;
            case KW_DROP:
                aux[0] = data_type_stack_pop(&data_type_stack);
                fprintf(fasm_file, ";;--KW_DROP--;;\n");
                fprintf(fasm_file, "    pop rax\n");
                break;
            case KW_DUP:
                aux[0] = data_type_stack_pop(&data_type_stack);
                fprintf(fasm_file, ";;--KW_DUP--;;\n");
                fprintf(fasm_file, "    pop rax\n");
                fprintf(fasm_file, "    push rax\n");
                fprintf(fasm_file, "    push rax\n");
                data_type_stack_push(&data_type_stack, &(DataType){aux[0]});
                data_type_stack_push(&data_type_stack, &(DataType){aux[0]});
                break;
            default:
                fprintf(stderr, "%d ", parsed_file->words[i]->key_word);
                ERROR("unknown Key Word");
                break;
            }
            break;
        case WT_SYSCALL:
            UNIMPLEMENTED("WT_SYSCALL");
            // write_syscall(fasm_file, &data_type_stack, &word.sys_call);
            break;
        case WT_MACRO:
            UNIMPLEMENTED("WT_MACRO");
            break;
        case WT_NONE:
            UNIMPLEMENTED("WT_NONE");
            break;
        case WT_OP:
            switch (parsed_file->words[i]->op)
            {
            case OP_PRINT:
                switch(data_type_stack_pop(&data_type_stack))
                {
                case DT_BOOL:
                    fprintf(fasm_file, ";;--PRINT_BOOL--;;\n");
                    fprintf(fasm_file, "    pop rdi\n");
                    fprintf(fasm_file, "    call dump\n");
                    break;
                case DT_CHAR:
                    fprintf(fasm_file, ";;--PRINT_CHAR--;;\n");
                    fprintf(fasm_file, "    pop rax\n");
                    fprintf(fasm_file, "    lea rsi, [rsp+15]\n");
                    fprintf(fasm_file, "    mov QWORD [rsp+15], rax\n");
                    fprintf(fasm_file, "    mov edi, 1\n");
                    fprintf(fasm_file, "    mov edx, 1\n");
                    fprintf(fasm_file, "    mov rax, 1\n");
                    fprintf(fasm_file, "    syscall\n");
                    break;
                case DT_FLOAT:
                    UNIMPLEMENTED("DT_FLOAT");
                    break;
                case DT_INT:
                    fprintf(fasm_file, ";;--PRINT_INT--;;\n");
                    fprintf(fasm_file, "    pop rdi\n");
                    fprintf(fasm_file, "    call dump\n");
                    break;
                case DT_STRING:
                    UNIMPLEMENTED("DT_STRING");
                    break;
                default:
                    ERROR("can not print this data type");
                    break;
                }
                break;
            case OP_ADD:
                aux[0] = data_type_stack_pop(&data_type_stack);
                aux[1] = data_type_stack_pop(&data_type_stack);
                if(aux[0] != aux[1])
                {
                    ERROR("add error");
                }
                switch (aux[0])
                {
                case DT_INT:
                    fprintf(fasm_file, ";;--OP_ADD--;;\n");
                    fprintf(fasm_file, "    pop rdi\n");
                    fprintf(fasm_file, "    pop rax\n");
                    fprintf(fasm_file, "    add rax, rdi\n");
                    fprintf(fasm_file, "    push rax\n");
                    data_type_stack_push(&data_type_stack, &aux[0]);
                    break;
                default:
                    ERROR("unsuported data type");
                    break;
                }
                break;
            case OP_SUB:
                aux[0] = data_type_stack_pop(&data_type_stack);
                aux[1] = data_type_stack_pop(&data_type_stack);
                if(aux[0] != aux[1])
                {
                    ERROR("SUB error");
                }
                switch (aux[0])
                {
                case DT_INT:
                    fprintf(fasm_file, ";;--OP_SUB--;;\n");
                    fprintf(fasm_file, "    pop rdi\n");
                    fprintf(fasm_file, "    pop rax\n");
                    fprintf(fasm_file, "    sub rax, rdi\n");
                    fprintf(fasm_file, "    push rax\n");
                    data_type_stack_push(&data_type_stack, &aux[0]);
                    break;
                default:
                    ERROR("unsuported data type");
                    break;
                }
                break;
            case OP_EQUAL:
                aux[0] = data_type_stack_pop(&data_type_stack);
                aux[1] = data_type_stack_pop(&data_type_stack);
                fprintf(fasm_file, ";;--OP_EQUAL--;;\n");
                fprintf(fasm_file, "    pop rdi\n");
                fprintf(fasm_file, "    pop rax\n");
                fprintf(fasm_file, "    xor rax, rdi\n");
                fprintf(fasm_file, "    push rax\n");
                data_type_stack_push(&data_type_stack, &(DataType){DT_BOOL});
                break;
            case OP_DIVISION:
                UNIMPLEMENTED("OP_DIVISION");
                break;
            case OP_MULTIPLY:
                UNIMPLEMENTED("OP_MULTIPLY");
                break;
            default:
                ERROR("unknown op");
                break;
            }
            break;
        default:
            ERROR("unknown WordType");
            break;
        }
    }
    if(data_type_stack.size > 0)
    {
        ERROR("unhandle data on stack");
    }
}

void compile(char *path)
{
    WordVec parsed_file = {0};
    char *out_file = (char *)malloc(strlen(path) * sizeof(char));

    memcpy(out_file, path, strlen(path) - 5);
    memcpy(out_file + strlen(path) - 5, "fasm", 5);
    
    FILE *fasm_file = fopen(out_file, "w");
    FILE *corth_file = fopen(path, "r");

    parse_file(&parsed_file, corth_file);

    fprintf(fasm_file, "format ELF64 executable 3\n");
    fprintf(fasm_file, "segment readable executable\n");

    fprintf(fasm_file, "dump:\n");
    fprintf(fasm_file, "    mov  r9, -3689348814741910323\n");
    fprintf(fasm_file, "    sub     rsp, 40\n");
    fprintf(fasm_file, "    lea     rcx, [rsp+31]\n");
    fprintf(fasm_file, ".L2:\n");
    fprintf(fasm_file, "    mov     rax, rdi\n");
    fprintf(fasm_file, "    lea     r8, [rsp+32]\n");
    fprintf(fasm_file, "    mul     r9\n");
    fprintf(fasm_file, "    mov     rax, rdi\n");
    fprintf(fasm_file, "    sub     r8, rcx\n");
    fprintf(fasm_file, "    shr     rdx, 3\n");
    fprintf(fasm_file, "    lea     rsi, [rdx+rdx*4]\n");
    fprintf(fasm_file, "    add     rsi, rsi\n");
    fprintf(fasm_file, "    sub     rax, rsi\n");
    fprintf(fasm_file, "    add     eax, 48\n");
    fprintf(fasm_file, "    mov     BYTE [rcx], al\n");
    fprintf(fasm_file, "    mov     rax, rdi\n");
    fprintf(fasm_file, "    mov     rdi, rdx\n");
    fprintf(fasm_file, "    mov     rdx, rcx\n");
    fprintf(fasm_file, "    sub     rcx, 1\n");
    fprintf(fasm_file, "    cmp     rax, 9\n");
    fprintf(fasm_file, "    ja      .L2\n");
    fprintf(fasm_file, "    lea     rax, [rsp+32]\n");
    fprintf(fasm_file, "    mov     edi, 1\n");
    fprintf(fasm_file, "    sub     rdx, rax\n");
    fprintf(fasm_file, "    lea     rsi, [rsp+32+rdx]\n");
    fprintf(fasm_file, "    mov     rdx, r8\n");
    fprintf(fasm_file, "    mov     rax, 1\n");
    fprintf(fasm_file, "    syscall\n");
    fprintf(fasm_file, "    add     rsp, 40\n");
    fprintf(fasm_file, "    ret\n");
    
    fprintf(fasm_file, "entry main\n");
    fprintf(fasm_file, "main:\n");

    write_fasm_file(fasm_file, &parsed_file);

    fprintf(fasm_file, "    mov rax, 60\n");
    fprintf(fasm_file, "    mov rdi, 0\n");
    fprintf(fasm_file, "    syscall\n");
    fprintf(fasm_file, "    ret\n");
    fprintf(fasm_file, "segment readable writable\n");

    fclose(fasm_file);
    fclose(corth_file);
}

Syscall get_syscall(char *str)
{
    for (size_t i = 0; i < SYSCALL_COUNT; i++)
    {
        if(strlen(str) == strlen(syscalls[i]) && strcmp(str, syscalls[i]) == 0)
        {
            return (Syscall)i;
        }
    }
    ERROR("unknown syscall");
}

void write_syscall(FILE *fasm_file, DataTypeStack *data_type_stack, Syscall *syscall)
{
    DataType aux = {0};
    switch (number_of_syscall_args[*syscall])
    {
    case 0:
        UNIMPLEMENTED("0 args syscall");
        break;
    case 1:
        UNIMPLEMENTED("1 args syscall");
        break;
    case 2:
        UNIMPLEMENTED("2 args syscall");
        break;
    case 3:
        // data_type_stack_pop(data_type_stack);
        // data_type_stack_pop(data_type_stack);
        // data_type_stack_pop(data_type_stack);
        // fprintf(fasm_file, "    pop rax\n");
        // fprintf(fasm_file, "    pop rdx\n");
        // fprintf(fasm_file, "    \n");
        // fprintf(fasm_file, "\n");
        break;
    case 4:
        UNIMPLEMENTED("4 args syscall");
        break;
    case 5:
        UNIMPLEMENTED("5 args syscall");
        break;
    case 6:
        UNIMPLEMENTED("6 args syscall");
        break;
    
    default:
        ERROR();
        break;
    }
}