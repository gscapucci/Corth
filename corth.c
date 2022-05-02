#include "corth.h"

void escape_str_or_char(char *str)
{
    size_t start = 0, end = 0;;
    if(str[0] == '\"')
    {
        start = 1;
    }
    if(str[strlen(str) - 1] == '\"')
    {
        end = 1;
    }
    for (size_t i = start; i < strlen(str) - end; i++)
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
            case '\0':
                str[i] = '\0';
                break;
            default:
                ERROR("trying to escape inescapable");
                break;
            }
            size_t j;
            for (j = i + 1; j < strlen(str) - end; j++)
            {
                str[j] = str[j + 1];
            }
            str[j] = '\0';
        }
    }
    
}

void stack_push(Stack *stack, Word *word)
{
    if(word->type == WT_NONE)
    {
        return;
    }
    if(stack->size >= MAX_STACK_CAP)
    {
        ERROR("Stack overflow");
    }
    if(stack->item[stack->size] != NULL)
    {
        free(stack->item[stack->size]);
    }
    stack->item[stack->size] = malloc(sizeof(Word));
    stack->item[stack->size]->value = malloc(word->size);
    memcpy(stack->item[stack->size]->value, word->value, word->size);
    stack->item[stack->size]->type = word->type;
    stack->item[stack->size]->size = word->size;
    if(word->type == WT_DATA_TYPE)
    {
        stack->item[stack->size]->data_type = word->data_type;
        stack->size++;
        return;
    }
    if(word->type == WT_OP)
    {
        stack->item[stack->size]->op = word->op;
        stack->size++;
        return;
    }
}

Word stack_pop(Stack *stack)
{
    if(stack->size == 0)
    {
        ERROR("Stack underflow");
    }
    stack->size--;
    Word word;
    word.type = stack->item[stack->size]->type;
    if(word.type == WT_DATA_TYPE)
    {
        word.data_type = stack->item[stack->size]->data_type;
    }
    else if(word.type == WT_OP)
    {
        word.op = stack->item[stack->size]->data_type;
    }
    word.size = stack->item[stack->size]->size;
    word.value = malloc(word.size);
    memcpy(word.value, stack->item[stack->size]->value, word.size);
    free(stack->item[stack->size]->value);
    return word;
}

long long str_to_long_long(char *str)
{
    long long number = 0;
    for (size_t i = strlen(str) - 1;; i--)
    {
        number += (str[i] - '0') * pow(10, strlen(str) - 1 - i);
        if(!i){break;}
    }
    return number;
}

bool is_number(char *str)
{
    bool isNumber = true;
    size_t start = 0;
    if(str[0] == '-')
    {
        start++;
    }
    for (size_t i = start; i < strlen(str); i++)
    {
        if(str[i] < '0' || str[i] > '9')
        {
            isNumber = false;
            break;
        }
    }
    return isNumber;
}

DataType get_data_type(char *str)
{
    if(is_number(str))
    {
        return DT_INT;
    }
    if((str[0] == '\'' && str[strlen(str) - 1] == '\''))
    {
        escape_str_or_char(str);
        if(strlen(str) != 3)
        {
            ERROR("char literal must have one character");
        }
        return DT_CHAR;
    }
    if(str[0] == '\"' && str[strlen(str) - 1] == '\"')
    {
        escape_str_or_char(str);
        return DT_STRING;
    }
    HERE("unknown data type");
    fprintf(stderr, " %s\n", str);
    exit(1);
}

KeyWord get_keyword(char *str)
{
    for (size_t i = 0; i < KEY_WORD_COUNT; i++)
    {
        if(!strcmp(key_word[i], str))
        {
            return (KeyWord)i;
        }
    }
    HERE("unkenown keyword");
    fprintf(stderr, " %s", str);
    exit(1);
}

WordType get_word_type(char *str)
{
    if(strlen(str) == 4 && str[0] == '\'' && str[3] == '\'')
    {
        escape_str_or_char(str);
        if(strlen(str) == 4)
        {
            ERROR("char must have one character");
        }
        return WT_DATA_TYPE;
    }
    if(is_number(str) || ((strlen(str) == 3) && (str[0] == '\'' && str[2] == '\'')) || (str[0] == '\"' && str[strlen(str) - 1] == '\"'))
    {
        return WT_DATA_TYPE;
    }
    if(strlen(str) == 1)
    {
        switch (str[0])
        {
        case OP_PLUS:
        case OP_MINUS:
        case OP_DIVISION:
        case OP_MULTIPLY:
        case OP_PRINT:
        case OP_EQUAL:
        case OP_PRINT_STACK:
            return WT_OP;
        case WT_NONE:
            return WT_NONE;
        default:
            break;
        }
    }
    for (size_t i = 0; i < KEY_WORD_COUNT; i++)
    {
        if(!strcmp(key_word[i], str))
        {
            return WT_KEY_WORD;
        }
    }
    HERE("unknown word");
    fprintf(stderr, " %s", str);
    exit(1);
}

void take_next_string(FILE *file, Word *word)
{
    char str[1024] = {0};
    int str_len = 0;
    char c;
    word->type = WT_DATA_TYPE;
    word->data_type = DT_STRING;
    word->size = 0;
    word->value = NULL;
    while (true)
    {
        c = getc(file);
        if(c == EOF)
        {
            ERROR("EOF");
        }
        if(c == '\"' && str_len == 0)
        {
            word->value = malloc(0);
            return;
        }
        if((c == '\"' && str[str_len - 1] != '\\') || c == '\0')
        {
            escape_str_or_char(str);
            word->size = (strlen(str) + 1) * sizeof(char);
            word->value = malloc(word->size);
            memcpy(word->value, str, word->size);
            return;
        }
        str[str_len] = c;
        str_len++;
    }
    
}

void take_next_char(FILE *file, Word *word)
{
    word->type = WT_DATA_TYPE;
    word->data_type = DT_CHAR;
    word->size = sizeof(char);
    char c[2] = {0};
    c[0] = getc(file);
    if(c[0] == EOF)
    {
        ERROR("EOF");
    }
    if(c[0] == '\\')
    {
        c[1] = getc(file);
        escape_str_or_char(c);
    }
    c[1] = getc(file);
    if(c[1] != '\'')
    {
        ERROR("closing char liteal error");
    }
    word->value = malloc(word->size);
    memcpy(word->value, &c[0], word->size);
}

int take_next_word(FILE *file, Word *word)
{
    char str[1024] = {0};
    int str_len = 0;
    char c;
    word->value = NULL;
    word->type = WT_NONE;
    while (true)
    {
        c = getc(file);
        switch (c)
        {
        case '\"':
            take_next_string(file, word);
            return 0;
        case '\'':
            take_next_char(file, word);
            return 0;
        default:
            break;
        }

        if(c != ' ' && c != '\n' && c != EOF)
        {
            str[str_len] = c;
            str_len++;
            continue;
        }

        if(str_len > 0)
        {
            word->type = get_word_type(str);
            switch (word->type)
            {
            case WT_DATA_TYPE:
                word->data_type = get_data_type(str);
                switch (word->data_type)
                {
                case DT_INT:
                    word->size = sizeof(long long);
                    word->value = malloc(word->size);
                    memcpy(word->value, &(long long){str_to_long_long(str)}, word->size);
                    break;
                default:
                    ERROR("unknown data type");
                    break;
                }
                break;
            case WT_KEY_WORD:
                word->key_word = get_keyword(str);
                break;
            case WT_OP:
                word->op = (Op)str[0];
                break;
            case WT_NONE:
                break;
            default:
                ERROR("unknown word type");
                break;
            }
            return c == EOF ? (str_len == 0 ? EOF : 0) : 0;
        }
        if(c == EOF)
        {
            return EOF;
        }
    }
}

void start(char *file_path)
{
    FILE *source_file = fopen(file_path, "r");
    Word word = {0};
    Stack stack = {0};
    while (take_next_word(source_file, &word) != EOF)
    {

        if(word.type == WT_NONE)
        {
            continue;
        }
        if(word.type == WT_DATA_TYPE)
        {
            stack_push(&stack, &word);
            continue;
        }
        if(word.type == WT_OP)
        {
            if(word.op == OP_PRINT)
            {
                if(stack.size == 0)
                {
                    ERROR("stack is empty");
                }
                if(stack.item[stack.size - 1]->type != WT_DATA_TYPE)
                {
                    ERROR("print only accept DATA_TYPE");
                }
                if(stack.item[stack.size - 1]->data_type == DT_INT)
                {
                    Word wrd = stack_pop(&stack);
                    printf("%lld", *(long long *)wrd.value);
                    free(wrd.value);
                    continue;
                }
                if(stack.item[stack.size - 1]->data_type == DT_CHAR)
                {
                    Word wrd = stack_pop(&stack);
                    printf("%c", *(char *)wrd.value);
                    free(wrd.value);
                    continue;   
                }
                if(stack.item[stack.size - 1]->data_type == DT_STRING)
                {
                    Word wrd = stack_pop(&stack);
                    printf("%s", (char *)wrd.value);
                    free(wrd.value);
                    continue;
                }
                if(stack.item[stack.size - 1]->data_type == DT_BOOL)
                {
                    Word wrd = stack_pop(&stack);
                    *(bool *)wrd.value ? printf("true") : printf("false");
                    free(wrd.value);
                    continue;
                }
                ERROR("unknown data type");
            }
            if(word.op == OP_PLUS)
            {
                if(stack.size < 2)
                {
                    ERROR("neet 2 values to PLUS");
                }
                if(stack.item[stack.size - 1]->type == WT_DATA_TYPE && stack.item[stack.size - 2]->type == WT_DATA_TYPE)
                {
                    if(stack.item[stack.size - 1]->data_type == DT_INT)
                    {
                        Word wrd1, wrd2;
                        wrd1 = stack_pop(&stack);
                        wrd2 = stack_pop(&stack);
                        *(long long *)wrd1.value = *(long long *)wrd1.value + *(long long*)wrd2.value;
                        stack_push(&stack, &wrd1);
                        free(wrd2.value);
                        continue;
                    }
                    ERROR("error");
                }
            }
            if(word.op == OP_EQUAL)
            {
                Word wrd1, wrd2;
                wrd1 = stack_pop(&stack);
                wrd2 = stack_pop(&stack);
                
                Word wrdbool;
                wrdbool.type = WT_DATA_TYPE;
                wrdbool.data_type = DT_BOOL;
                wrdbool.size = sizeof(bool);
                wrdbool.value = malloc(wrdbool.size);
                
                if(wrd1.size != wrd2.size)
                {
                    memcpy(wrdbool.value, &(bool){false}, wrdbool.size);
                }
                else if(!memcmp(wrd1.value, wrd2.value, wrd1.size))
                {
                    memcpy(wrdbool.value, &(bool){true}, wrdbool.size);
                }
                else
                {
                    memcpy(wrdbool.value, &(bool){false}, wrdbool.size);
                }
                stack_push(&stack, &wrdbool);
                continue;
            }
            if(word.op == OP_PRINT_STACK)
            {
                for (size_t i = stack.size - 1;; i--)
                {
                    printf("(");
                    switch (stack.item[i]->type)
                    {
                    case WT_NONE:
                        printf("NONE");
                        break;
                    case WT_DATA_TYPE:
                        printf("DATA TYPE -> ");
                        switch (stack.item[i]->data_type)
                        {
                        case DT_BOOL:
                            printf("BOOL");
                            break;
                        case DT_CHAR:
                            printf("CHAR");
                            break;
                        case DT_STRING:
                            printf("STRING");
                            break;
                        case DT_INT:
                            printf("INT");
                            break;
                        default:
                            break;
                        }
                        break;
                    case WT_KEY_WORD:
                        printf("KEYWORD -> ");
                        switch (stack.item[i]->key_word)
                        {
                        case KW_DROP:
                            printf("DROP");
                            break;
                        case KW_DUP:
                            printf("DUP");
                            break;
                        case KW_OVER:
                            printf("OVER");
                            break;
                        case KW_SWAP:
                            printf("SWAP");
                            break;
                        default:
                            break;
                        }
                        break;
                    case WT_OP:
                        printf("OP -> ");
                        switch (stack.item[i]->op)
                        {
                        case OP_DIVISION:
                            printf("DIVISION");
                            break;
                        case OP_MULTIPLY:
                            printf("MULTIPLY");
                            break;
                        case OP_MINUS:
                            printf("MINUS");
                            break;
                        case OP_PLUS:
                            printf("PLUS");
                            break;
                        case OP_EQUAL:
                            printf("EQUL");
                            break;
                        case OP_PRINT:
                            printf("PRINT");
                            break;
                        default:
                            break;
                        }
                        break;
                    default:
                        break;
                    }
                    printf(")\n");
                    if(!i){break;}
                }
                continue;
            }
            ERROR("unknown op");
        }
        if(word.type == WT_KEY_WORD)
        {
            if(word.key_word == KW_DROP)
            {
                if(stack.size == 0)
                {
                    ERROR("stack is empty");
                }
                Word wrd = stack_pop(&stack);
                free(word.value);
                continue;
            }
            if(word.key_word == KW_DUP)
            {
                if(stack.size == 0)
                {
                    ERROR("stack is empty");
                }
                stack_push(&stack, stack.item[stack.size - 1]);
                continue;
            }
            if(word.key_word == KW_SWAP)
            {
                if(stack.size < 2)
                {
                    ERROR("swap require 2 elements in stack");
                }
                Word wrd1 = stack_pop(&stack);
                Word wrd2 = stack_pop(&stack);

                stack_push(&stack, &wrd1);
                stack_push(&stack, &wrd2);
            
                free(wrd1.value);
                free(wrd2.value);
                continue;
            }
            if(word.key_word == KW_OVER)
            {
                if(stack.size < 2)
                {
                    ERROR("over require 2 elementes on stack");
                }
                stack_push(&stack, stack.item[stack.size - 2]);
                continue;
            }
            ERROR("unknown keyword");
        }
        HERE("unknown type");
        fprintf(stderr, " typeid: %d\n", (int)word.type);
        exit(1);
    }
    printf("\n");
    if(stack.size != 0)
    {
        ERROR("unhandle data on stack");
    }
    fclose(source_file);
}