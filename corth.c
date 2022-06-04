#include "corth.h"

void escape_str(char *str)
{
    for (uint64_t i = 0; i < strlen(str); i++)
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
            for (uint64_t j = i + 1; j < strlen(str); j++)
            {
                str[j] = str[j + 1];
            }
        }
    }    
}

void clear_macro_vec(MacroVec *macroVec)
{
    for (uint64_t i = 0; i < macroVec->size; i++)
    {
        for (uint64_t j = 0; j < macroVec->macros[i]->size; j++)
        {
            if(macroVec->macros[i]->words[j].size > 0)
            {
                my_free(macroVec->macros[i]->words[j].value);
            }
        }
        my_free(macroVec->macros[i]->words);
        my_free(macroVec->macros[i]->name);
        my_free(macroVec->macros[i]);
    }
}

void macro_vec_push(Macro* macro, Word *word)
{
    if(macro->size == 0)
    {
        macro->words = (Word *)my_malloc(5 * sizeof(Word));
    }
    else
    {
        if(macro->size % 5 == 0)
        {
            macro->words = my_realloc(macro->words, (macro->size + 5) * sizeof(Word));
        }
    }
    macro->words[macro->size].size = word->size;
    if(word->size > 0)
    {
        macro->words[macro->size].value = my_malloc(word->size);
        memcpy(macro->words[macro->size].value, word->value, word->size);
    }
    macro->words[macro->size].type = word->type;
    switch (word->type)
    {
    case WT_COMMENT:
        ERROR("not expect comment");
        break;
    case WT_DATA_TYPE:
        macro->words[macro->size].data_type = word->data_type;
        break;
    case WT_KEY_WORD:
        macro->words[macro->size].key_word = word->key_word;
        break;
    case WT_OP:
        macro->words[macro->size].op = word->op;
        break;
    case WT_SYSCALL:
        macro->words[macro->size].sys_call = word->sys_call;
        break;
    case WT_MACRO:
    case WT_NONE:
        break;
    default:
        ERROR("unknown word type");
        break;
    }
    macro->size++;
    return;
}

void create_macro(FILE *file, uint64_t *id)
{
    char str[1024] = {0};
    uint64_t str_len = 0;
    uint64_t block_deepth = 0;
    char c;
    Word word = {0};
    for (c = getc(file); !(c == '\n' || c == ' '); c = getc(file))
    {
        if(c == EOF)
        {
            ERROR("not expect end of file");
        }    
        str[str_len++] = c;
    }
    if(is_float(str) || is_number(str))
    {
        ERROR("can not use an namer as a macro name");
    }
    for (uint64_t i = 0; i < KW_COUNT; i++)
    {
        if(!strcmp(str, key_word[i]))
        {
            ERROR("can not used a reserved word as a macro name");
        }
    }
    for (uint64_t i = 0; i < OP_COUNT; i++)
    {
        if(!strcmp(str, ops[i]))
        {
            ERROR("can not use an operator as a macro name");
        }
    }
    for (uint64_t i = 0; i < macro_vec.size; i++)
    {
        if(!strcmp(str, macro_vec.macros[i]->name))
        {
            ERROR("macro already defined");
        }
    }
    macro_vec.macros[macro_vec.size] = my_malloc(sizeof(Macro));
    macro_vec.macros[macro_vec.size]->name = my_malloc((str_len + 1) * sizeof(char));
    macro_vec.macros[macro_vec.size]->size = 0;
    memcpy(macro_vec.macros[macro_vec.size]->name, str, str_len + 1);
    for (uint64_t i = 0; str[i] != 0; i++)
    {
        str[i] = 0;
    }
    str_len = 0;
    for (c = getc(file);;c = getc(file))
    {
        if(c == EOF)
        {
            ERROR("not expect end of file");
        }
        if(str_len == 0)
        {
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
                word.id = *id;
                (*id)++;
                macro_vec_push(macro_vec.macros[macro_vec.size], &word);
                if(word.size > 0)
                {
                    my_free(word.value);
                }
                for (uint64_t i = 0; str[i] != 0; i++)
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
                word.id = *id;
                (*id)++;
                macro_vec_push(macro_vec.macros[macro_vec.size], &word);
                if(word.size > 0)
                {
                    my_free(word.value);
                }
                for (uint64_t i = 0; str[i] != 0; i++)
                {
                    str[i] = 0;
                }
                str_len = 0;
                continue;
            }
        }
        if(c == '#')
        {
            while ((c = getc(file)) != '\n' && c != EOF);
        }
        if(c != ' ' && c != '\n')
        {
            str[str_len++] = c;
            continue;
        }
        if(str_len > 0)
        {
            word = get_word(str);   
            word.id = *id;
            (*id)++;
            if(word.type == WT_KEY_WORD)
            {
                if(word.key_word == KW_WHILE || word.key_word == KW_IF)
                {
                    block_deepth++;
                }
                else if(word.key_word == KW_END)
                {
                    if(block_deepth == 0)
                    {
                        macro_vec.size++;
                        return;
                    }
                    block_deepth--;
                }
            }
            macro_vec_push(macro_vec.macros[macro_vec.size], &word);
            if(word.type == WT_KEY_WORD && word.key_word == KW_MACRO)
            {
                ERROR("not expect key word macro inside a macro");
            }
            if(word.size > 0)
            {
                my_free(word.value);
            }
            for (uint64_t i = 0; str[i] != 0; i++)
            {
                str[i] = 0;
            }
            str_len = 0;
            continue;
        }
    }
}

int32_t parse_file(WordVec *word_vec, FILE *file)
{
    char str[1024] = {0};
    uint16_t str_len = 0;
    uint64_t id = 0;
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
                word.id = id;
                id++;
                word_vec_push(word_vec, &word);
                if(word.size > 0)
                {
                    my_free(word.value);
                }
                for (uint64_t i = 0; str[i] != 0; i++)
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
                word.id = id;
                id++;
                word_vec_push(word_vec, &word);
                if(word.size > 0)
                {
                    my_free(word.value);
                }
                for (uint64_t i = 0; str[i] != 0; i++)
                {
                    str[i] = 0;
                }
                str_len = 0;
                continue;
            }
        }
        if(c == '#')
        {
            while ((c = getc(file)) != '\n' && c != EOF);
        }
        if(c != ' ' && c != '\n' && c != EOF)
        {
            str[str_len++] = c;
            continue;
        }

        if(str_len > 0)
        {
            word = get_word(str);   
            word.id = id;
            id++;
            if(word.type == WT_KEY_WORD && word.key_word == KW_MACRO)
            {
                create_macro(file, &id);
            }
            else
            {
                word_vec_push(word_vec, &word);
            }
            if(word.size > 0)
            {
                my_free(word.value);
            }
            for (uint64_t i = 0; str[i] != 0; i++)
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
            my_free(word_vec->words[word_vec->size]->value);
        }
        my_free(word_vec->words[word_vec->size]);
        word_vec->words[word_vec->size] = NULL;
    }
    word_vec->words[word_vec->size] = my_malloc(sizeof(Word));
    word_vec->words[word_vec->size]->type = word->type;
    word_vec->words[word_vec->size]->size = word->size;
    if(word->size > 0)
    {
        word_vec->words[word_vec->size]->size = word->size;
        word_vec->words[word_vec->size]->value = my_malloc(word->size);
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
    for (uint64_t i = 0; i < word_vec->size; i++)
    {
        if(word_vec->words[i]->size > 0)
        {
            my_free(word_vec->words[i]->value);
        }
        my_free(word_vec->words[i]);
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
    for (uint64_t i = 0; i < strlen(str); i++)
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
    for (uint64_t i = 0; i < strlen(str); i++)
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
    for (uint64_t i = 0; i < OP_COUNT; i++)
    {
        if(!strcmp(str, ops[i]))
        {
            word.type = WT_OP;
            word.op = (Op)i;
            word.size = 0;
            word.value = NULL;
            return word;
        }
    }
    for (uint64_t i = 0; i < KW_COUNT; i++)
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
    for (uint64_t i = 0; i < SYSCALL_COUNT; i++)
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
    for (uint64_t i = 0; i < macro_vec.size; i++)
    {
        if(!strcmp(str, macro_vec.macros[i]->name))
        {
            word.type = WT_MACRO;
            word.size = strlen(str) + 1;
            word.value = my_malloc(word.size * sizeof(char));
            memcpy(word.value, str, word.size);
            return word;
        }
    }
    if(is_number(str))
    {
        word.type = WT_DATA_TYPE;
        word.data_type = DT_INT;
        word.size = sizeof(uint64_t);
        word.value = my_malloc(word.size);
        memcpy(word.value, &(uint64_t){strtoul(str, NULL, 10)}, word.size);
        return word;
    }
    if(is_float(str))
    {
        word.type = WT_DATA_TYPE;
        word.data_type = DT_FLOAT;
        word.size = sizeof(long double);
        word.value = my_malloc(word.size);
        memcpy(word.value, &(long double){strtold(str, NULL)}, word.size);
        return word;
    }
    if(strlen(str) == 3 && (str[0] == '\'' && str[2] == '\''))
    {
        word.type = WT_DATA_TYPE;
        word.data_type = DT_CHAR;
        word.size = sizeof(char);
        word.value = (char *)my_malloc(word.size);
        memcpy(word.value, &str[1], word.size);
        return word;
    }
    if(str[0] == '\"' && str[strlen(str) - 1] == '\"')
    {
        word.type = WT_DATA_TYPE;
        word.data_type = DT_STRING;
        word.size = strlen(str) + 1;
        word.value = (char *)my_malloc(word.size);
        memcpy(word.value, str, word.size);
        return word;
    }
    fprintf(stderr, "%s ", str);
    ERROR("unknown word");
}

void create_if_block(WordVec *parsed_file, uint64_t *i)
{
    uint64_t if_index = *i;
    parsed_file->words[if_index]->size = sizeof(uint64_t);
    parsed_file->words[if_index]->value = my_malloc(parsed_file->words[if_index]->size);
    while (true)
    {
        (*i)++;
        if(*i >= parsed_file->size)
        {
            ERROR("unclosed if");
        }
        if(parsed_file->words[*i]->type == WT_KEY_WORD && parsed_file->words[*i]->key_word == KW_IF)
        {
            create_if_block(parsed_file, i);
        }
        if(parsed_file->words[*i]->type == WT_KEY_WORD && (parsed_file->words[*i]->key_word == KW_ELSE || parsed_file->words[*i]->key_word == KW_END))
        {
            memcpy(parsed_file->words[if_index]->value, i, sizeof(uint64_t));
            if(parsed_file->words[*i]->key_word == KW_ELSE)
            {
                uint64_t else_index = *i;
                parsed_file->words[else_index]->size = sizeof(uint64_t);
                parsed_file->words[else_index]->value = my_malloc(parsed_file->words[else_index]->size);
                while (true)
                {
                    (*i)++;
                    if(*i >= parsed_file->size)
                    {
                        ERROR("unclosed if");
                    }
                    if(parsed_file->words[*i]->type == WT_KEY_WORD && parsed_file->words[*i]->key_word == KW_IF)
                    {
                        create_if_block(parsed_file, i);
                    }
                    if(parsed_file->words[*i]->type == WT_KEY_WORD && parsed_file->words[*i]->key_word == KW_END)
                    {
                        parsed_file->words[*i]->size = 2 * sizeof(uint64_t);
                        parsed_file->words[*i]->value = my_malloc(parsed_file->words[*i]->size);
                        memcpy(parsed_file->words[*i]->value, i, sizeof(uint64_t));
                        memcpy(parsed_file->words[*i]->value + sizeof(uint64_t), i, sizeof(uint64_t));
                        memcpy(parsed_file->words[else_index]->value, i, sizeof(uint64_t));
                        (*i)++;
                        return;
                    }
                }
            }
            if(parsed_file->words[*i]->key_word == KW_END)
            {
                parsed_file->words[*i]->size = 2 * sizeof(uint64_t);
                parsed_file->words[*i]->value = my_malloc(parsed_file->words[*i]->size);
                memcpy(parsed_file->words[*i]->value, i, sizeof(uint64_t));
                memcpy(parsed_file->words[*i]->value + sizeof(uint64_t), i, sizeof(uint64_t));
                memcpy(parsed_file->words[if_index]->value, i, sizeof(uint64_t));
                (*i)++;
                return;
            }
            (*i)++;
            return;
        }
    }
}

void create_while_block(WordVec *parsed_file, uint64_t *i)
{
    uint64_t while_index = *i;
    while(true)
    {
        (*i)++;
        if(*i >= parsed_file->size)
        {
            ERROR("unclosed while");
        }
        if(parsed_file->words[*i]->type == WT_KEY_WORD)
        {
            if(parsed_file->words[*i]->key_word == KW_DO)
            {
                uint64_t do_index = *i;
                parsed_file->words[do_index]->size = sizeof(uint64_t);
                parsed_file->words[do_index]->value = my_malloc(parsed_file->words[do_index]->size);
                while (true)
                {
                    (*i)++;
                    if(*i >= parsed_file->size)
                    {
                        ERROR("unclosed do");
                    }
                    if(parsed_file->words[*i]->type == WT_KEY_WORD)
                    {
                        if(parsed_file->words[*i]->key_word == KW_END)
                        {
                            uint64_t end_index = *i;
                            memcpy(parsed_file->words[do_index]->value, &end_index, sizeof(uint64_t));
                            parsed_file->words[end_index]->size = 2 * sizeof(uint64_t);
                            parsed_file->words[end_index]->value = my_malloc(parsed_file->words[end_index]->size);
                            memcpy(parsed_file->words[end_index]->value, &while_index, sizeof(uint64_t));
                            memcpy(parsed_file->words[end_index]->value + sizeof(uint64_t), &i, sizeof(uint64_t));
                            (*i)++;
                            return;    
                        }
                        if(parsed_file->words[*i]->key_word == KW_IF)
                        {
                            create_if_block(parsed_file, i);
                        }
                    }
                }
            }
            continue;
        }
    }
}


void create_macro_if_block(Macro *macro, uint64_t *i, uint64_t macro_vec_index, uint64_t parsed_file_size)
{
    uint64_t if_index = *i;
    macro->words[macro_vec_index].size = sizeof(uint64_t);
    macro->words[macro_vec_index].value = my_malloc(macro->words[macro_vec_index].size);
    for (uint64_t index = macro_vec_index;true;index++)
    {
        (*i)++;
        if((*i) >= parsed_file_size)
        {
            ERROR("unclosed if");
        }
        if(macro->words[index].type == WT_KEY_WORD && macro->words[index].key_word == KW_IF)
        {
            create_macro_if_block(macro, i, index, parsed_file_size);
        }
        if(macro->words[index].type == WT_KEY_WORD && (macro->words[index].key_word == KW_ELSE || macro->words[index].key_word == KW_END))
        {
            memcpy(macro->words[macro_vec_index].value, i, sizeof(uint64_t));
            if(macro->words[index].key_word == KW_ELSE)
            {
                uint64_t else_index = index;
                macro->words[index].size = 2 * sizeof(uint64_t);
                macro->words[index].value = my_malloc(macro->words[index].size);
                while (true)
                {
                    (*i)++;
                    if(macro->words[index].type == WT_KEY_WORD && macro->words[index].key_word == KW_IF)
                    {
                        create_macro_if_block(macro, i, macro_vec_index, parsed_file_size);
                    }
                    if(macro->words[index].type == WT_KEY_WORD && macro->words[index].key_word == KW_END)
                    {
                        macro->words[index].size = 2 * sizeof(uint64_t);
                        macro->words[index].value = my_malloc(macro->words[index].size);
                        memcpy(macro->words[index].value, i, sizeof(uint64_t));
                        memcpy(macro->words[index].value + sizeof(uint64_t), i, sizeof(uint64_t));
                        memcpy(macro->words[else_index].value, i, sizeof(uint64_t));
                        (*i)++;
                        return;
                    }
                }
            }
            (*i)++;
            return;
        }
    }
}
void create_macro_while_block(Macro *macro, uint64_t *i, uint64_t macro_vec_index, uint64_t parsed_file_size)
{
    uint64_t while_index = *i;
    macro->words[macro_vec_index].size = sizeof(uint64_t);
    macro->words[macro_vec_index].value = my_malloc(macro->words[macro_vec_index].size);
    memcpy(macro->words[macro_vec_index].value, &while_index, sizeof(uint64_t));
    for(uint64_t index = macro_vec_index;true;index++)
    {
        (*i)++;
        // if((*i) >= parsed_file_size)
        // {
        //     printf("\n%zu %zu\n", *i, parsed_file_size);
        //     printf("\n%d\n", macro->words[index].type);
        //     printf("\n%d\n", macro->words[index].key_word);
        //     ERROR("unclosed while");
        // }
        if(macro->words[index].type == WT_KEY_WORD)
        {
            if(macro->words[index].key_word == KW_DO)
            {
                uint64_t do_index = index;
                macro->words[do_index].size = sizeof(uint64_t);
                macro->words[do_index].value = my_malloc(macro->words[do_index].size);
                while (true)
                {
                    index++;
                    (*i)++;
                    // if(*i >= macro->size)
                    // {
                    //     ERROR("unclosed do");
                    // }
                    if(macro->words[index].type == WT_KEY_WORD)
                    {
                        if(macro->words[index].key_word == KW_END)
                        {
                            uint64_t end_index = index;
                            memcpy(macro->words[do_index].value, i, sizeof(uint64_t));
                            macro->words[end_index].size = 2 * sizeof(uint64_t);
                            macro->words[end_index].value = my_malloc(macro->words[end_index].size);
                            memcpy(macro->words[end_index].value, &while_index, sizeof(uint64_t));
                            memcpy(macro->words[end_index].value + sizeof(uint64_t), i, sizeof(uint64_t));
                            (*i)++;
                            return;
                        }
                        if(macro->words[index].key_word == KW_IF)
                        {
                            create_macro_if_block(macro, i, index, parsed_file_size);
                        }
                    }
                }
            }
            continue;
        }
    }
}
//TODO: something wrong with index
void create_blocks(WordVec *parsed_file, uint64_t *i)
{

    for (uint64_t j = 0; j < macro_vec.size; j++)
    {
        for (uint64_t k = 0; k < macro_vec.macros[j]->size; k++, (*i)++)
        {
            if(macro_vec.macros[j]->words[k].type == WT_KEY_WORD)
            {
                if(macro_vec.macros[j]->words[k].key_word == KW_IF)
                {
                    create_macro_if_block(macro_vec.macros[j], i, k, parsed_file->size);
                    continue;
                }
                if(macro_vec.macros[j]->words[k].key_word == KW_WHILE)
                {
                    create_macro_while_block(macro_vec.macros[j], i, k, parsed_file->size);
                    continue;
                }
            }
        }
    }
    
    for (uint64_t j = 0; j < parsed_file->size; j++, (*i)++)
    {
        if(parsed_file->words[j]->type == WT_KEY_WORD)
        {
            if(parsed_file->words[j]->key_word == KW_IF)
            {
                uint64_t start = *i;
                create_if_block(parsed_file, i);
                j += *i - start;
                continue;
            }
            if(parsed_file->words[j]->key_word == KW_WHILE)
            {
                uint64_t start = *i;
                create_while_block(parsed_file, i);
                j += *i - start;
                continue;
            }
        }
    }
}

void write_fasm_file(FILE *fasm_file, Word *word, DataTypeStack *data_type_stack, uint64_t *stack_size, uint64_t index)
{
    DataType aux[3] = {0};
    switch (word->type)
    {
    case WT_COMMENT:
        break;
    case WT_DATA_TYPE:
        data_type_stack_push(data_type_stack, &word->data_type);
        switch (word->data_type)
        {
        case DT_BOOL:
            UNIMPLEMENTED("DT_BOOL");
            break;
        case DT_CHAR:
            fprintf(fasm_file, ";;--PUSH_CHAR ascii(%zu)--;;\n", (uint64_t)*(char *)word->value);
            fprintf(fasm_file, "    push %zu\n", (uint64_t)*(char *)word->value);
            break;
        case DT_FLOAT:
            UNIMPLEMENTED("DT_FLOAT");
            break;
        case DT_INT:
            fprintf(fasm_file, ";;--PUSH_INT %zu--;;\n", *(uint64_t *)word->value);
            fprintf(fasm_file, "    mov rax, %zu\n", *(uint64_t *)word->value);
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
        switch (word->key_word)
        {
        case KW_SYSCALL:
            aux[0] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_INT)
            {
                ERROR("expect int");
            }
            write_syscall(fasm_file, data_type_stack, &(Syscall){aux[0]});
            break;
        case KW_IF:
            aux[0] = data_type_stack_pop(data_type_stack);
            *stack_size = data_type_stack->size;
            if(aux[0] != DT_BOOL)
            {
                ERROR("expect bool");
            }
            fprintf(fasm_file, ";;--KW_IF--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    cmp rax, 0\n");
            fprintf(fasm_file, "    je addr_%zu\n", *(uint64_t *)word->value);
            break;
        case KW_END:
            if(*stack_size != data_type_stack->size)
            {
                ERROR("stack size after and before blocks must be equal");
            }
            fprintf(fasm_file, ";;--KW_END--;;\n");
            fprintf(fasm_file, "    jmp addr_%zu\n", ((uint64_t *)word->value)[0]);
            fprintf(fasm_file, "addr_%zu:\n", ((uint64_t *)word->value)[1]);
            break;
        case KW_ELSE:
            if(*stack_size != data_type_stack->size)
            {
                ERROR("stack size after and before if must be equal");
            }
            fprintf(fasm_file, ";;--KW_ELSE--;;\n");
            fprintf(fasm_file, "    jmp addr_%zu\n", *(uint64_t *)word->value);
            fprintf(fasm_file, "addr_%zu:\n", index);
            break;
        case KW_DROP:
            aux[0] = data_type_stack_pop(data_type_stack);
            fprintf(fasm_file, ";;--KW_DROP--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            break;
        case KW_DUP:
            aux[0] = data_type_stack_pop(data_type_stack);
            fprintf(fasm_file, ";;--KW_DUP--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    push rax\n");
            fprintf(fasm_file, "    push rax\n");
            data_type_stack_push(data_type_stack, &aux[0]);
            data_type_stack_push(data_type_stack, &aux[0]);
            break;
        case KW_SWAP:
            if(data_type_stack->size < 2)
            {
                ERROR("swap require 2 elements");
            }
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            fprintf(fasm_file, ";;--KW_SWAP--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    push rax\n");
            fprintf(fasm_file, "    push rbx\n");
            data_type_stack_push(data_type_stack, &aux[0]);
            data_type_stack_push(data_type_stack, &aux[1]);
            break;
        case KW_OVER:
            if(data_type_stack->size < 2)
            {
                ERROR("over require 2 elements");
            }
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            fprintf(fasm_file, ";;--KW_OVER--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    push rbx\n");
            fprintf(fasm_file, "    push rax\n");
            fprintf(fasm_file, "    push rbx\n");
            data_type_stack_push(data_type_stack, &aux[1]);
            data_type_stack_push(data_type_stack, &aux[0]);
            data_type_stack_push(data_type_stack, &aux[1]);
            break;
        case KW_ROT:
            if(data_type_stack->size < 3)
            {
                ERROR("rot require 3 elements");
            }
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            aux[2] = data_type_stack_pop(data_type_stack);
            fprintf(fasm_file, ";;--KW_ROT--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    pop rcx\n");
            fprintf(fasm_file, "    push rbx\n");
            fprintf(fasm_file, "    push rax\n");
            fprintf(fasm_file, "    push rcx\n");
            data_type_stack_push(data_type_stack, &aux[1]);
            data_type_stack_push(data_type_stack, &aux[0]);
            data_type_stack_push(data_type_stack, &aux[2]);
            break;
        case KW_CAST_INT:
            switch(data_type_stack_pop(data_type_stack))
            {
                case DT_INT:
                case DT_CHAR:
                case DT_BOOL:
                    break;
                default:
                    ERROR("invalid cast to int");
                    break;
            }
            data_type_stack_push(data_type_stack, &(DataType){DT_INT});
            break;
        case KW_CAST_CHAR:
            switch(data_type_stack_pop(data_type_stack))
            {
                case DT_INT:
                case DT_CHAR:
                case DT_BOOL:
                    break;
                default:
                    ERROR("invalid cast to char");
                    break;
            }
            data_type_stack_push(data_type_stack, &(DataType){DT_CHAR});
            break;
        case KW_FALSE:
            fprintf(fasm_file, ";;--KW_FALSE--;;\n");
            fprintf(fasm_file, "    push 0\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_BOOL});
            break;
        case KW_TRUE:
            fprintf(fasm_file, ";;--KW_TRUE--;;\n");
            fprintf(fasm_file, "    push 1\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_BOOL});
            break;
        case KW_WHILE:
            *stack_size = data_type_stack->size;
            fprintf(fasm_file, ";;--KW_WHILE--;;\n");
            fprintf(fasm_file, "addr_%zu:\n", *(uint64_t *)word->value);
            break;
        case KW_DO:
            aux[0] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_BOOL)
            {
                ERROR("expect bool");
            }
            fprintf(fasm_file, ";;--KW_DO--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    cmp rax, 0\n");
            fprintf(fasm_file, "    je addr_%zu\n", *(uint64_t *)word->value);
            break;
        default:
            fprintf(stderr, "%d ", word->key_word);
            ERROR("unknown Key Word");
            break;
        }
        break;
    case WT_SYSCALL:
        UNIMPLEMENTED("WT_SYSCALL");
        // write_syscall(fasm_file, &data_type_stack, &word.sys_call);
        break;
    case WT_MACRO:
        for (uint64_t j = 0; j < macro_vec.size; j++)
        {
            if(!strcmp((char *)word->value, macro_vec.macros[j]->name))
            {
                for (size_t k = 0; k < macro_vec.macros[j]->size; k++)
                {
                    write_fasm_file(fasm_file, &macro_vec.macros[j]->words[k], data_type_stack, stack_size, index);
                }
            }
        }
        break;
    case WT_NONE:
        UNIMPLEMENTED("WT_NONE");
        break;
    case WT_OP:
        switch (word->op)
        {
        case OP_PRINT:
            switch(data_type_stack_pop(data_type_stack))
            {
            case DT_BOOL:
                fprintf(fasm_file, ";;--PRINT_BOOL--;;\n");
                fprintf(fasm_file, "    pop rdi\n");
                fprintf(fasm_file, "    call dump\n");
                break;
            case DT_CHAR:
                fprintf(fasm_file, ";;--PRINT_CHAR--;;\n");
                fprintf(fasm_file, "    pop rax\n");
                fprintf(fasm_file, "    mov BYTE [rsp+31], al\n");
                fprintf(fasm_file, "    lea rsi, [rsp+31]\n");
                fprintf(fasm_file, "    mov rdi, 1\n");
                fprintf(fasm_file, "    mov rdx, 1\n");
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
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[0] != aux[1])
            {
                ERROR("add error");
            }
            switch (aux[0])
            {
            case DT_INT:
                fprintf(fasm_file, ";;--OP_ADD--;;\n");
                fprintf(fasm_file, "    pop rbx\n");
                fprintf(fasm_file, "    pop rax\n");
                fprintf(fasm_file, "    add rax, rbx\n");
                fprintf(fasm_file, "    push rax\n");
                data_type_stack_push(data_type_stack, &aux[0]);
                break;
            default:
                ERROR("unsuported data type");
                break;
            }
            break;
        case OP_SUB:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
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
                data_type_stack_push(data_type_stack, &aux[0]);
                break;
            default:
                ERROR("unsuported data type");
                break;
            }
            break;
        case OP_EQUAL:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_INT || aux[1] != DT_INT)
            {
                ERROR("can only compare integers");
            }
            fprintf(fasm_file, ";;--OP_EQUAL--;;\n");
            fprintf(fasm_file, "    mov rcx, 0\n");
            fprintf(fasm_file, "    mov rdx, 1\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    cmp rax, rbx\n");
            fprintf(fasm_file, "    cmove rcx, rdx\n");
            fprintf(fasm_file, "    push rcx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_BOOL});
            break;
        case OP_DIVMOD:
            UNIMPLEMENTED("OP_DIVMOD");
            break;
        case OP_MULTIPLY:
            UNIMPLEMENTED("OP_MULTIPLY");
            break;
        case OP_GREAT:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_INT || aux[1] != DT_INT)
            {
                ERROR("can only compare integers");
            }
            fprintf(fasm_file, ";;--OP_GREAT--;;\n");
            fprintf(fasm_file, "    mov rcx, 0\n");
            fprintf(fasm_file, "    mov rdx, 1\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    cmp rbx, rax\n");
            fprintf(fasm_file, "    cmovg rcx, rdx\n");
            fprintf(fasm_file, "    push rcx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_BOOL});
            break;
        case OP_GREAT_EQUAL:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_INT || aux[1] != DT_INT)
            {
                ERROR("can only compare integers");
            }
            fprintf(fasm_file, ";;--OP_GREAT--;;\n");
            fprintf(fasm_file, "    mov rcx, 0\n");
            fprintf(fasm_file, "    mov rdx, 1\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    cmp rbx, rax\n");
            fprintf(fasm_file, "    cmovge rcx, rdx\n");
            fprintf(fasm_file, "    push rcx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_BOOL});
            break;
        case OP_LESS:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_INT || aux[1] != DT_INT)
            {
                ERROR("can only compare integers");
            }
            fprintf(fasm_file, ";;--OP_LESS--;;\n");
            fprintf(fasm_file, "    mov rcx, 0\n");
            fprintf(fasm_file, "    mov rdx, 1\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    cmp rbx, rax\n");
            fprintf(fasm_file, "    cmovl rcx, rdx\n");
            fprintf(fasm_file, "    push rcx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_BOOL});
            break;
        case OP_LESS_EQUAL:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_INT || aux[1] != DT_INT)
            {
                ERROR("can only compare integers");
            }
            fprintf(fasm_file, ";;--OP_LESS--;;\n");
            fprintf(fasm_file, "    mov rcx, 0\n");
            fprintf(fasm_file, "    mov rdx, 1\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    cmp rbx, rax\n");
            fprintf(fasm_file, "    cmovle rcx, rdx\n");
            fprintf(fasm_file, "    push rcx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_BOOL});
            break;
        case OP_NOT:
            aux[0] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_BOOL)
            {
                ERROR("expect bool");
            }
            fprintf(fasm_file, ";;--OP_NOT--;;\n");
            fprintf(fasm_file, "    mov rcx, 0\n");
            fprintf(fasm_file, "    mov rdx, 1\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    cmp rax, rcx\n");
            fprintf(fasm_file, "    cmove rcx, rdx\n");
            fprintf(fasm_file, "    push rcx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_BOOL});
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

void compile(char *path)
{
    WordVec parsed_file = {0};
    char *out_file = (char *)my_malloc(strlen(path) * sizeof(char));
    memcpy(out_file, path, strlen(path) - 5);
    memcpy(out_file + strlen(path) - 5, "fasm", 5);
    FILE *fasm_file = fopen(out_file, "w");
    FILE *corth_file = fopen(path, "r");

    my_free(out_file);

    parse_file(&parsed_file, corth_file);
    // print_parsed_file(&parsed_file);
    uint64_t index = 0;
    create_blocks(&parsed_file, &index);
    // type_check();
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
    
    DataTypeStack data_type_stack = {0};
    uint64_t stack_size = 0;
    
    for (uint64_t i = 0; i < parsed_file.size; i++)
    {
        write_fasm_file(fasm_file, parsed_file.words[i], &data_type_stack, &stack_size, i);
    }
    if(data_type_stack.size > 0)
    {
        ERROR("unhandle data on stack");
    }
    fprintf(fasm_file, "    mov rax, 60\n");
    fprintf(fasm_file, "    mov rdi, 0\n");
    fprintf(fasm_file, "    syscall\n");
    fprintf(fasm_file, "    ret\n");
    fprintf(fasm_file, "segment readable writable\n");

    word_vec_clear(&parsed_file);
    clear_macro_vec(&macro_vec);

    fclose(fasm_file);
    fclose(corth_file);
}

Syscall get_syscall(char *str)
{
    for (uint64_t i = 0; i < SYSCALL_COUNT; i++)
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
        UNIMPLEMENTED("3 args syscall");
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

void print_parsed_file(WordVec *word_vec)
{
    for (uint64_t i = 0; i < word_vec->size; i++)
    {
        switch (word_vec->words[i]->type)
        {
        case WT_COMMENT:
            // printf("[%zu] = WT_COMMENT\n", i);
            break;
        case WT_DATA_TYPE:  
            switch (word_vec->words[i]->data_type)
            {
            case DT_INT:
                printf("[%zu] = DT_INT\n", i);
                break;
            case DT_CHAR:
                printf("[%zu] = DT_CHAR\n", i);
                break;
            case DT_BOOL:
                printf("[%zu] = DT_BOOL\n", i);
                break;
            default:
                // ERROR("unknow data type");
                break;
            }
            break;
        case WT_KEY_WORD:
            switch (word_vec->words[i]->key_word)
            {
            case KW_IF:
                printf("[%zu] = KW_IF\n", i);
                break;
            case KW_ELSE:
                printf("[%zu] = KW_ELSE\n", i);
                break;
            case KW_END:
                printf("[%zu] = KW_END\n", i);
                break;
            default:
                break;
            }
            break;
        case WT_MACRO:
            // UNIMPLEMENTED("WT_MACRO");
            break;
        case WT_NONE:
            // UNIMPLEMENTED("WT_NONE");
            break;
        case WT_OP:
            // UNIMPLEMENTED("WT_OP");
            break;
        case WT_SYSCALL:
            // UNIMPLEMENTED("WT_SYSCALL");
            break;
        default:
            // ERROR("unkown word type");
            break;
        }
    }
}