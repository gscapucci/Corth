#include "corth.h"

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
    for (size_t i = 0; i < strlen(str); i++)
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
    if(strlen(str) == 3 && (str[0] == '\'' && str[2] == '\''))
    {
        return DT_CHAR;
    }
    if(str[0] == '\"' && str[strlen(str) - 1] == '\"')
    {
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
    if( is_number(str) || ((strlen(str) == 3) && (str[0] == '\'' && str[2] == '\'')) || (str[0] == '\"' && str[strlen(str) - 1] == '\"'))
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

int take_next_word(FILE *file, Word *word)
{
    char str[1024] = {0};
    size_t str_len = 0;
    char c;
    word->value = NULL;
    word->type = WT_NONE;
    bool is_str = false;
    bool escape = false;
    bool is_char = false;
    while (true)
    {
        c = getc(file);
        if(c == '\'')
        {
            is_char = true;
        }
        else if(c == '\"')
        {
            if(str_len != 0)
            {
                if(str[str_len - 1] != '\\')
                {
                    is_str = is_str ? false : true;
                }
            }
            else
            {
                is_str = is_str ? false : true;
            }
        }
        if(c == '\\' && is_str == true)
        {
            escape = true;
        }
        else if(c == '\\' && str[str_len - 1] == '\'');
        else if(escape)
        {
            switch (c)
            {
            case 'n':
                str[str_len - 1] = '\n';
                break;
            case 'a':
                str[str_len - 1] = '\a';
                break;
            case 'b':
                str[str_len - 1] = '\b';
                break;
            case 't':
                str[str_len - 1] = '\t';
                break;
            case 'v':
                str[str_len - 1] = '\v';
                break;
            case 'f':
                str[str_len - 1] = '\f';
                break;
            case 'r':
                str[str_len - 1] = '\r';
                break;
            case 'e':
                str[str_len - 1] = '\e';
                break;
            case '\"':
                str[str_len - 1] = '\"';
                break;
            default:
                ERROR("trying to escape nothing");
                break;
            }
            str[str_len] = '\0';
            str_len--;
            escape = false;
        }
        if((c ==  ' ' || c == '\n' || c == EOF) && !is_str)
        {
            if(str_len > 0)
            {
                word->type = get_word_type(str);
                switch (word->type)
                {
                case WT_NONE:
                    break;
                case WT_OP:
                    switch (str[0])
                    {
                    case OP_PLUS:
                        word->op = OP_PLUS;
                        break;
                    case OP_MINUS:
                        word->op = OP_MINUS;
                        break;
                    case OP_DIVISION:
                        word->op = OP_DIVISION;
                        break;
                    case OP_MULTIPLY:
                        word->op = OP_MULTIPLY;
                        break;
                    case OP_PRINT:
                        word->op = OP_PRINT;
                        break;
                    default:
                        HERE("unknown OP");
                        fprintf(stderr, " %s", str);
                        exit(1);
                        break;
                    }
                    break;
                case WT_DATA_TYPE:
                    switch (get_data_type(str))
                    {
                    case DT_INT:
                        word->size = sizeof(long long);
                        word->value = malloc(word->size);
                        word->data_type = DT_INT;
                        memcpy(word->value, &(long long){str_to_long_long(str)}, word->size);
                        break;
                    case DT_CHAR:
                        word->size = sizeof(char);
                        word->value = malloc(sizeof(char));
                        word->data_type = DT_CHAR;
                        memcpy(word->value, &str[1], word->size);
                        break;
                    case DT_STRING:
                        word->size = str_len - 1;
                        word->value = malloc(word->size * sizeof(char));
                        word->data_type = DT_STRING;
                        str[str_len - 1] = '\0';
                        memcpy(word->value, str + 1, word->size);
                        break;
                    default:
                        HERE("unknown data type");
                        fprintf(stderr, " %s\n", str);
                        exit(1);
                        break;
                    }
                    break;
                case WT_KEY_WORD:
                    switch (get_keyword(str))
                    {
                    case KW_DROP:
                        word->key_word = KW_DROP;
                        break;
                    case KW_DUP:
                        word->key_word = KW_DUP;
                        break;
                    case KW_SWAP:
                        word->key_word = KW_SWAP;
                        break;
                    case KW_OVER:
                        word->key_word = KW_OVER;
                        break;
                    default:
                        HERE("unknown keyword");
                        fprintf(stderr, " %s\n", str);
                        exit(1);
                        break;
                    }
                    break;
                default:
                    HERE("unknown type");
                    fprintf(stderr, " %s\n", str);
                    exit(1);
                    break;
                }
            }
            else if(c == '\n')
            {
                continue;
            }
            return c == EOF? (str_len != 0 ? 0 : EOF) : 0;
        }
        str[str_len] = c;
        str_len++;
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
        ERROR("unknown type");
    }
    printf("\n");
    if(stack.size != 0)
    {
        ERROR("unhandle data on stack");
    }
    fclose(source_file);
}