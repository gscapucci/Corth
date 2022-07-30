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
    my_free(macroVec->macros);
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
            macro->words = (Word *)my_realloc(macro->words, (macro->size + 5) * sizeof(Word));
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

void check_valid_name(char *str)
{
    if(is_float(str) || is_number(str))
    {
        ERROR("can not use an number to define a name");
    }
    for (uint64_t i = 0; i < KW_COUNT; i++)
    {
        if(!strcmp(str, key_word[i]))
        {
            ERROR("can not used a reserved word to define a name");
        }
    }
    for (uint64_t i = 0; i < OP_COUNT; i++)
    {
        if(!strcmp(str, ops[i]))
        {
            ERROR("can not use an operator to define a name");
        }
    }
    for (uint64_t i = 0; i < macro_vec.size; i++)
    {
        if(!strcmp(str, macro_vec.macros[i]->name))
        {
            ERROR("can not use a macro to define a name");
        }
    }
    for (uint64_t i = 0; i < func_vec.size; i++)
    {
        if(!strcmp(str, func_vec.funcs[i]->name))
        {
            ERROR("can not use a function do defien a name");
        }
    }
    
    for (uint64_t i = 0; i < DT_COUNT; i++)
    {
        if(!strcmp(str, type_names[i]))
        {
            ERROR("can not use a type name to define a name");
        }
    }
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
    check_valid_name(str);
    if(macro_vec.size >= macro_vec.capacity)
    {
        macro_vec.capacity += 1024 * 10 * sizeof(Macro *);
        macro_vec.macros = (Macro **)my_realloc(macro_vec.macros, macro_vec.capacity);
    }
    macro_vec.macros[macro_vec.size] = (Macro *)my_malloc(sizeof(Macro));
    macro_vec.macros[macro_vec.size]->name = (char *)my_malloc((str_len + 1) * sizeof(char));
    macro_vec.macros[macro_vec.size]->size = 0;
    memcpy(macro_vec.macros[macro_vec.size]->name, str, str_len + 1);
    for (uint64_t i = 0; str[i] != 0; i++)
    {
        str[i] = 0;
    }
    str_len = 0;
    for (c = getc(file);;c = getc(file))
    {
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
        if(c != ' ' && c != '\n' && c != EOF)
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
        if(c == EOF)
        {
            ERROR("not expect end of file");
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
                while (!(c == '\"' && str[str_len - 1] != '\\'))
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
            if(word.type == WT_KEY_WORD)
            {
                if(word.key_word == KW_MACRO)
                {
                    create_macro(file, &id);
                    for (uint64_t i = 0; str[i] != 0; i++)
                    {
                        str[i] = 0;
                    }
                    str_len = 0;
                    continue;
                }
                else if(word.key_word == KW_FUNC)
                {
                    create_func(file, &id);
                    for (uint64_t i = 0; str[i] != 0; i++)
                    {
                        str[i] = 0;
                    }
                    str_len = 0;
                    continue;
                }
            }
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
        if(c == EOF) return EOF;
    }
}

void word_vec_push(WordVec *word_vec, Word* word)
{
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
        if(word->type == WT_DATA_TYPE && word->data_type == DT_STRING)
        {
            word_vec->words[word_vec->size]->size = word->size - 2;
            word_vec->words[word_vec->size]->value = my_malloc(word->size - 2);
            memcpy(word_vec->words[word_vec->size]->value, word->value + 1, word_vec->words[word_vec->size]->size);
            ((char *)word_vec->words[word_vec->size]->value)[word_vec->words[word_vec->size]->size - 1] = 0;
        }
        else
        {
            word_vec->words[word_vec->size]->size = word->size;
            word_vec->words[word_vec->size]->value = my_malloc(word->size);
            memcpy(word_vec->words[word_vec->size]->value, word->value, word_vec->words[word_vec->size]->size);
        }
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
    case WT_COMMENT:
    case WT_MACRO:
    case WT_NONE:
    case WT_FUNC:
        break;
    default:
        ERROR("unknown word type");
        break;
    }
    word_vec->size++;
}

void clear_word_vec(WordVec *word_vec)
{
    for (uint64_t i = 0; i < word_vec->size; i++)
    {
        if(word_vec->words[i]->size > 0)
        {
            my_free(word_vec->words[i]->value);
        }
        my_free(word_vec->words[i]);
    }
    my_free(word_vec->words);
    word_vec->size = 0;
}

void data_type_stack_push(DataTypeStack *dt_stack, DataType *data_type)
{
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
    for (uint64_t i = 0; i < DT_COUNT; i++)
    {
        if(!strcmp(str, type_names[i]))
        {
            word.type = WT_TYPE_NAME;
            word.data_type = (DataType)i;
            word.size = 0;
            word.value = NULL;
            return word;
        }
    }
    
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
    for (uint64_t i = 0; i < func_vec.size; i++)
    {
        if(!strcmp(str, func_vec.funcs[i]->name))
        {
            word.type = WT_FUNC;
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
    parsed_file->words[while_index]->size = sizeof(uint64_t);
    parsed_file->words[while_index]->value = my_malloc(sizeof(uint64_t));
    memcpy(parsed_file->words[while_index]->value, &while_index, sizeof(uint64_t));
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
                            memcpy(parsed_file->words[end_index]->value + sizeof(uint64_t), i, sizeof(uint64_t));
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


void create_func_if_block(Func *func, uint64_t *i, uint64_t func_vec_index, uint64_t parsed_file_index)
{
    uint64_t if_index = *i;
    func->func_body.words[func_vec_index]->size = sizeof(uint64_t);
    func->func_body.words[func_vec_index]->value = my_malloc(func->func_body.words[func_vec_index]->size);
    for (uint64_t index = func_vec_index;true;index++)
    {
        (*i)++;
        // if((*i) >= parsed_file_index)
        // {
        //     ERROR("unclosed if");
        // }
        if(func->func_body.words[index]->type == WT_KEY_WORD && func->func_body.words[index]->key_word == KW_IF)
        {
            create_func_if_block(func, i, index, parsed_file_index);
        }
        if(func->func_body.words[index]->type == WT_KEY_WORD && (func->func_body.words[index]->key_word == KW_ELSE || func->func_body.words[index]->key_word == KW_END))
        {
            memcpy(func->func_body.words[func_vec_index]->value, i, sizeof(uint64_t));
            if(func->func_body.words[index]->key_word == KW_ELSE)
            {
                uint64_t else_index = index;
                func->func_body.words[index]->size = 2 * sizeof(uint64_t);
                func->func_body.words[index]->value = my_malloc(func->func_body.words[index]->size);
                while (true)
                {
                    (*i)++;
                    if(func->func_body.words[index]->type == WT_KEY_WORD && func->func_body.words[index]->key_word == KW_IF)
                    {
                        create_func_if_block(func, i, func_vec_index, parsed_file_index);
                    }
                    if(func->func_body.words[index]->type == WT_KEY_WORD && func->func_body.words[index]->key_word == KW_END)
                    {
                        func->func_body.words[index]->size = 2 * sizeof(uint64_t);
                        func->func_body.words[index]->value = my_malloc(func->func_body.words[index]->size);
                        memcpy(func->func_body.words[index]->value, i, sizeof(uint64_t));
                        memcpy(func->func_body.words[index]->value + sizeof(uint64_t), i, sizeof(uint64_t));
                        memcpy(func->func_body.words[else_index]->value, i, sizeof(uint64_t));
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
void create_func_while_block(Func *func, uint64_t *i, uint64_t func_vec_index, uint64_t parsed_file_index)
{
    uint64_t while_index = *i;
    func->func_body.words[func_vec_index]->size = sizeof(uint64_t);
    func->func_body.words[func_vec_index]->value = my_malloc(func->func_body.words[func_vec_index]->size);
    memcpy(func->func_body.words[func_vec_index]->value, &while_index, sizeof(uint64_t));
    for(uint64_t index = func_vec_index;true;index++)
    {
        (*i)++;
        // if((*i) >= parsed_file_size)
        // {
        //     printf("\n%zu %zu\n", *i, parsed_file_size);
        //     printf("\n%d\n", macro->words[index].type);
        //     printf("\n%d\n", macro->words[index].key_word);
        //     ERROR("unclosed while");
        // }
        if(func->func_body.words[index]->type == WT_KEY_WORD)
        {
            if(func->func_body.words[index]->key_word == KW_DO)
            {
                uint64_t do_index = index;
                func->func_body.words[do_index]->size = sizeof(uint64_t);
                func->func_body.words[do_index]->value = my_malloc(func->func_body.words[do_index]->size);
                while (true)
                {
                    index++;
                    (*i)++;
                    // if(*i >= macro->size)
                    // {
                    //     ERROR("unclosed do");
                    // }
                    if(func->func_body.words[index]->type == WT_KEY_WORD)
                    {
                        if(func->func_body.words[index]->key_word == KW_END)
                        {
                            uint64_t end_index = index;
                            memcpy(func->func_body.words[do_index]->value, i, sizeof(uint64_t));
                            func->func_body.words[end_index]->size = 2 * sizeof(uint64_t);
                            func->func_body.words[end_index]->value = my_malloc(func->func_body.words[end_index]->size);
                            memcpy(func->func_body.words[end_index]->value, &while_index, sizeof(uint64_t));
                            memcpy(func->func_body.words[end_index]->value + sizeof(uint64_t), i, sizeof(uint64_t));
                            (*i)++;
                            return;
                        }
                        if(func->func_body.words[index]->key_word == KW_IF)
                        {
                            create_func_if_block(func, i, index, parsed_file_index);
                        }
                    }
                }
            }
            continue;
        }
    }
}

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

    for (uint64_t j = 0; j < func_vec.size; j++)
    {
        for (uint64_t k = 0; k < func_vec.funcs[j]->func_body.size; k++)
        {
            if(func_vec.funcs[j]->func_body.words[k]->type == WT_KEY_WORD)
            {
                if(func_vec.funcs[j]->func_body.words[k]->key_word == KW_IF)
                {
                    create_func_if_block(func_vec.funcs[j], i, k, parsed_file->size);
                    continue;
                }
                if(func_vec.funcs[j]->func_body.words[k]->key_word == KW_WHILE)
                {
                    create_func_while_block(func_vec.funcs[j], i, k, parsed_file->size);
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
                // uint64_t start = *i;
                create_if_block(parsed_file, &j);
                // j += *i - start;
                continue;
            }
            if(parsed_file->words[j]->key_word == KW_WHILE)
            {
                // uint64_t start = *i;
                create_while_block(parsed_file, &j);
                // j += *i - start;
                continue;
            }
        }
    }
}

void write_fasm_file(FILE *fasm_file, Word *word, DataTypeStack *data_type_stack, uint64_t *stack_size, uint64_t index)
{
    static uint16_t *str_len;
    static int32_t str_index = 0;
    DataType aux[3] = {0};
    switch (word->type)
    {
    case WT_COMMENT:
        break;
    case WT_DATA_TYPE:
        switch (word->data_type)
        {
        case DT_BOOL:
            UNIMPLEMENTED("DT_BOOL");
            break;
        case DT_CHAR:
            data_type_stack_push(data_type_stack, &(DataType){DT_CHAR});
            fprintf(fasm_file, ";;--PUSH_CHAR ascii(%zu)--;;\n", (uint64_t)*(char *)word->value);
            fprintf(fasm_file, "    push %zu\n", (uint64_t)*(char *)word->value);
            break;
        case DT_FLOAT:
            UNIMPLEMENTED("DT_FLOAT");
            break;
        case DT_INT:
            data_type_stack_push(data_type_stack, &(DataType){DT_INT});
            fprintf(fasm_file, ";;--PUSH_INT %zu--;;\n", *(uint64_t *)word->value);
            fprintf(fasm_file, "    mov rax, %zu\n", *(uint64_t *)word->value);
            fprintf(fasm_file, "    push rax\n");
            break;
        case DT_STRING:
            data_type_stack_push(data_type_stack, &(DataType){DT_STRING});
            string_vec_push(&str_vec, word);
            str_len[str_index] = str_vec.strings[str_vec.size - 1]->size;
            fprintf(fasm_file, ";;--PUSH_STRING--;;\n");
            fprintf(fasm_file, "    push str_%zu\n", str_vec.size - 1);
            break;
        default:
            ERROR("unknown DataType");
            break;
        }
        break;
    case WT_KEY_WORD:
        switch (word->key_word)
        {
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
                case DT_PTR:
                    fprintf(fasm_file, ";;--KW_CAST_INT--;;\n");
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
                    fprintf(fasm_file, ";;--KW_CAST_CHAR--;;\n");
                    break;
                default:
                    ERROR("invalid cast to char");
                    break;
            }
            data_type_stack_push(data_type_stack, &(DataType){DT_CHAR});
            break;
        case KW_CAST_PTR:
            switch (data_type_stack_pop(data_type_stack))
            {
            case DT_INT:
            case DT_PTR:
                fprintf(fasm_file, ";;--KW_CAST_PTR--;;\n");
                break;
            default:
                ERROR("invalid cast to ptr");
                break;
            }
            data_type_stack_push(data_type_stack, &(DataType){DT_PTR});
            break;
        case KW_FALSE:
            fprintf(fasm_file, ";;--KW_FALSE--;;\n");
            fprintf(fasm_file, "    mov rax, 0\n");
            fprintf(fasm_file, "    push rax\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_BOOL});
            break;
        case KW_TRUE:
            fprintf(fasm_file, ";;--KW_TRUE--;;\n");
            fprintf(fasm_file, "    mov rax, 1\n");
            fprintf(fasm_file, "    push rax\n");
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
        case KW_MEM:
            fprintf(fasm_file, ";;KW_MEM--;;\n");
            fprintf(fasm_file, "    push mem\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_PTR});
            break;
        case KW_WRITE8:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[1] != DT_PTR)
            {
                ERROR("expect ptr");
            }
            fprintf(fasm_file, ";;--KW_WRITE8--;;\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    mov [rax], bl\n");
            break;
        case KW_WRITE16:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[1] != DT_PTR)
            {
                ERROR("expect ptr");
            }
            fprintf(fasm_file, ";;--KW_WRITE16--;;\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    mov [rax], bx\n");
            break;
        case KW_WRITE32:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[1] != DT_PTR)
            {
                ERROR("expect ptr");
            }
            fprintf(fasm_file, ";;--KW_WRITE32--;;\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    mov [rax], ebx\n");
            break;
        case KW_WRITE64:
            aux[0] = data_type_stack_pop(data_type_stack);
            aux[1] = data_type_stack_pop(data_type_stack);
            if(aux[1] != DT_PTR)
            {
                ERROR("expect ptr");
            }
            fprintf(fasm_file, ";;--KW_WRITE64--;;\n");
            fprintf(fasm_file, "    pop rbx\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    mov [rax], rbx\n");
            break;
        case KW_READ8:
            aux[0] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_PTR)
            {
                ERROR("expect ptr");
            }
            fprintf(fasm_file, ";;--KW_READ8--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    xor rbx, rbx\n");
            fprintf(fasm_file, "    mov bl, [rax]\n");
            fprintf(fasm_file, "    push rbx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_INT});
            break;
        case KW_READ16:
            aux[0] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_PTR)
            {
                ERROR("expect ptr");
            }
            fprintf(fasm_file, ";;--KW_READ8--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    xor rbx, rbx\n");
            fprintf(fasm_file, "    mov bx, [rax]\n");
            fprintf(fasm_file, "    push rbx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_INT});
            break;
        case KW_READ32:
            aux[0] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_PTR)
            {
                ERROR("expect ptr");
            }
            fprintf(fasm_file, ";;--KW_READ32--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    xor rbx, rbx\n");
            fprintf(fasm_file, "    mov ebx, [rax]\n");
            fprintf(fasm_file, "    push rbx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_INT});
            break;
        case KW_READ64:
            aux[0] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_PTR)
            {
                ERROR("expect ptr");
            }
            fprintf(fasm_file, ";;--KW_READ64--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    xor rbx, rbx\n");
            fprintf(fasm_file, "    mov rbx, [rax]\n");
            fprintf(fasm_file, "    push rbx\n");
            data_type_stack_push(data_type_stack, &(DataType){DT_INT});
            break;
        case KW_SHR:
            aux[0] = data_type_stack_pop(data_type_stack);
            if(aux[0] != DT_INT && aux[0] != DT_CHAR)
            {
                ERROR("invalid data type");
            }
            fprintf(fasm_file, ";;--KW_SHR--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    shr rax, 1\n");
            fprintf(fasm_file, "    push rax\n");
            data_type_stack_push(data_type_stack, &aux[0]);
            break;
        case KW_SHL:
            aux[0] = data_type_stack_pop(data_type_stack);
            fprintf(fasm_file, ";;--KW_SHL--;;\n");
            fprintf(fasm_file, "    pop rax\n");
            fprintf(fasm_file, "    shl rax, 1\n");
            fprintf(fasm_file, "    push rax\n");
            data_type_stack_push(data_type_stack, &aux[0]);
            break;
        default:
            fprintf(stderr, "%s ", key_word[word->key_word]);
            ERROR("unknown Key Word");
            break;
        }
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
    case WT_FUNC:
        for (uint64_t j = 0; j < func_vec.size; j++)
        {
            if(!strcmp((char *)word->value, func_vec.funcs[j]->name))
            {
                for (uint64_t k = 0; k < func_vec.funcs[j]->args->size; k++)
                {
                    aux[0] = data_type_stack_pop(data_type_stack);
                    if(aux[0] != func_vec.funcs[j]->args->types[(func_vec.funcs[j]->args->size - 1) - k])
                    {
                        ERROR("invalid function args");
                    }
                }
                fprintf(fasm_file, ";;--FUNCTION_CALL--;;\n");
                fprintf(fasm_file, "    mov rax, rsp\n");
                fprintf(fasm_file, "    mov rsp, [ret_stack_rsp]\n");
                fprintf(fasm_file, "    call addr_%s\n", func_vec.funcs[j]->name);
                fprintf(fasm_file, "    mov [ret_stack_rsp], rsp\n");
                fprintf(fasm_file, "    mov rsp, rax\n");
                for (uint64_t k = 0; k < func_vec.funcs[j]->ret->size; k++)
                {
                    data_type_stack_push(data_type_stack, &func_vec.funcs[j]->ret->types[k]);
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
            case DT_PTR:
                fprintf(fasm_file, ";;--PRINT_PTR--;;\n");
                fprintf(fasm_file, "    pop rdi\n");
                fprintf(fasm_file, "    call dump\n");
                break;
            case DT_INT:
                fprintf(fasm_file, ";;--PRINT_INT--;;\n");
                fprintf(fasm_file, "    pop rdi\n");
                fprintf(fasm_file, "    call dump\n");
                break;
            case DT_STRING:
                aux[0] = data_type_stack_pop(data_type_stack);
                if(aux[0] != DT_INT)
                {
                    ERROR("expect string size(int)");
                }
                fprintf(fasm_file, ";;--PRINT_STR--;;\n");
                fprintf(fasm_file, "    pop rax\n");
                fprintf(fasm_file, "    pop rbx\n");
                fprintf(fasm_file, "    lea rsi, [rax]\n");
                fprintf(fasm_file, "    mov rdi, 1\n");
                fprintf(fasm_file, "    mov rdx, rbx\n");
                fprintf(fasm_file, "    mov rax, 1\n");
                fprintf(fasm_file, "    syscall\n");
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
        case OP_PRINT_STACK:
            print_data_type_stack(data_type_stack);
            break;
        default:
            ERROR("unknown op");
            break;
        }
        break;
    default:
        ERROR("unknown word type");
        break;
    }
}

void compile(char *path)
{
    WordVec parsed_file = {0};

    parsed_file.capacity = MAX_NUMBER_OF_WORDS;
    parsed_file.words = (Word **)my_malloc(parsed_file.capacity * sizeof(Word*));

    macro_vec.capacity = MAX_MACRO_NAMES;
    macro_vec.macros = (Macro **)my_malloc(macro_vec.capacity * sizeof(Macro*));

    str_vec.capacity = MAX_NUMBER_OF_STRINGS;
    str_vec.strings = (String **)my_malloc(str_vec.capacity * sizeof(String*));

    func_vec.capacity = MAX_NUMBER_OF_FUNCTIONS;
    func_vec.funcs = (Func **)my_malloc(func_vec.capacity * sizeof(Func*));

    DataTypeStack data_type_stack = {0};
    data_type_stack.types = (DataType *)my_malloc(MAX_STACK_CAP * sizeof(DataType));

    char *out_file = (char *)my_malloc(strlen(path) * sizeof(char));
    memcpy(out_file, path, strlen(path) - 5);
    memcpy(out_file + strlen(path) - 5, "fasm", 5);
    FILE *fasm_file = fopen(out_file, "w");
    FILE *corth_file = fopen(path, "r");

    my_free(out_file);

    parse_file(&parsed_file, corth_file);
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
    
    write_functions(fasm_file);

    fprintf(fasm_file, "entry main\n");
    fprintf(fasm_file, "main:\n");
    fprintf(fasm_file, "    mov rax, ret_stack_end\n");
    fprintf(fasm_file, "    mov [ret_stack_rsp], rax\n");
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

    for (uint64_t i = 0; i < str_vec.size; i++)
    {
        fprintf(fasm_file, "str_%zu db ", i);
        for (uint64_t j = 0; j < str_vec.strings[i]->size - 1; j++)
        {
            fprintf(fasm_file, "%d,", str_vec.strings[i]->str[j]);
        }
        fprintf(fasm_file, "0\n");
    }
    fprintf(fasm_file, "ret_stack_rsp: rq 1\n");
    fprintf(fasm_file, "ret_stack: rb %d\n", RET_STACK_CAP);
    fprintf(fasm_file, "ret_stack_end:\n");
    fprintf(fasm_file, "mem_str: rb %d\n", MEM_STR_CAP);
    fprintf(fasm_file, "mem: rb %d\n", MEM_CAP);
    
    clear_word_vec(&parsed_file);
    clear_macro_vec(&macro_vec);
    clear_string_vec(&str_vec);
    clear_func_vec(&func_vec);
    my_free(data_type_stack.types);

    fclose(fasm_file);
    fclose(corth_file);
}

void string_vec_push(StringVec *string_vec, Word *word)
{
    string_vec->strings[string_vec->size] = my_malloc(sizeof(String));
    string_vec->strings[string_vec->size]->size = (strlen((char *)word->value) + 1) * sizeof(char);
    string_vec->strings[string_vec->size]->str = my_malloc(string_vec->strings[string_vec->size]->size * sizeof(char));
    memcpy(string_vec->strings[string_vec->size]->str, word->value, string_vec->strings[string_vec->size]->size);
    string_vec->size++;
}

void clear_string_vec(StringVec *string_vec)
{
    if(string_vec)
    {
        for (uint64_t i = 0; i < string_vec->size; i++)
        {
            if(string_vec->strings[i])
            {
                if(string_vec->strings[i]->str)
                {
                    my_free(string_vec->strings[i]->str); 
                }
            }
            my_free(string_vec->strings[i]);
        }
        my_free(string_vec->strings);
    }
}

void print_data_type_stack(DataTypeStack *data_type_stack)
{
    if(data_type_stack->size == 0) {return;}
    printf("-----------------\n");
    for (uint64_t i = data_type_stack->size - 1;; i--)
    {
        switch (data_type_stack->types[i])
        {
        case DT_BOOL:
            printf("[BOOL]\n");
            break;
        case DT_CHAR:
            printf("[CHAR]\n");
            break;
        case DT_FLOAT:
            printf("[FLOAT]\n");
            break;
        case DT_INT:
            printf("[INT]\n");
            break;
        case DT_PTR:
            printf("[PTR]\n");
            break;
        case DT_STRING:
            printf("[STRING]\n");
            break;
        default:
            ERROR("unknown data type");
            break;
        }
        if(i == 0){break;}
    }
    printf("-----------------\n");
}

void create_func(FILE *file, uint64_t *id)
{
    char str[1024] = {0};
    uint64_t str_len = 0;
    uint64_t block_deepth = 0;
    char c;
    Word word = {0};
    bool args = true;
    for (c = getc(file); !(c == '\n' || c == ' '); c = getc(file))
    {
        if(c == EOF)
        {
            ERROR("not expect end of file");
        }    
        str[str_len++] = c;
    }
    check_valid_name(str);
    func_vec.funcs[func_vec.size] = (Func *)my_malloc(sizeof(Func));
    func_vec.funcs[func_vec.size]->name = (char *)my_malloc((str_len + 1) * sizeof(char));
    
    func_vec.funcs[func_vec.size]->args = (DataTypeStack *)my_malloc(sizeof(DataTypeStack));
    func_vec.funcs[func_vec.size]->args->size = 0;
    func_vec.funcs[func_vec.size]->args->types = (DataType *)my_malloc(MAX_NUMBER_OF_FUNCTION_PARAMETERS * sizeof(DataType));
    
    func_vec.funcs[func_vec.size]->ret = (DataTypeStack *)my_malloc(sizeof(DataTypeStack));
    func_vec.funcs[func_vec.size]->ret->size = 0;
    func_vec.funcs[func_vec.size]->ret->types = (DataType *)my_malloc(MAX_NUMBER_OF_FUNCTION_PARAMETERS * sizeof(DataType));

    func_vec.funcs[func_vec.size]->func_body.capacity = MAX_STACK_CAP;
    func_vec.funcs[func_vec.size]->func_body.words = (Word **)my_malloc(func_vec.funcs[func_vec.size]->func_body.capacity * sizeof(Word*));
    memcpy(func_vec.funcs[func_vec.size]->name, str, str_len + 1);
    for (uint64_t i = 0; i < str_len; i++)
    {
        str[i] = 0;
    }
    str_len = 0;
    for(c = getc(file);;c = getc(file))
    {
        if(c == EOF)
        {
            ERROR("not expect end of file");
        }
        if(c != '\n' && c != ' ')
        {
            str[str_len++] = c;
            continue;
        }
        if(str_len > 0)
        {
            word = get_word(str);
            if(word.type == WT_KEY_WORD && word.key_word == KW_DO)
            {
                for (uint64_t i = 0; str[i] != 0; i++)
                {
                    str[i] = 0;
                }
                str_len = 0;
                break;
            }
            if(word.type == WT_KEY_WORD && word.key_word == KW_MINUS_MINUS)
            {
                args = false;
                str[0] = 0;
                str[1] = 0;
                str_len = 0;
                continue;
            }
            if(word.type != WT_TYPE_NAME)
            {
                ERROR("function definition only accept type names");
            }
            if(args)
            {
                data_type_stack_push(func_vec.funcs[func_vec.size]->args, &(DataType){word.data_type});
            }
            else
            {
                data_type_stack_push(func_vec.funcs[func_vec.size]->ret, &(DataType){word.data_type});
            }
            for (uint64_t i = 0; i < str_len; i++)
            {
                str[i] = 0;
            }
            str_len = 0;
            continue;
        }
    }
    for (char c = getc(file);;c = getc(file))
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
                word_vec_push(&func_vec.funcs[func_vec.size]->func_body, &word);
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
                word_vec_push(&func_vec.funcs[func_vec.size]->func_body, &word);
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
                        func_vec.size++;
                        return;
                    }
                    block_deepth--;
                }
            }
            word_vec_push(&func_vec.funcs[func_vec.size]->func_body, &word);
            if(word.type == WT_KEY_WORD && word.key_word == KW_MACRO)
            {
                ERROR("not expect key word macro inside a function");
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
        if(c == EOF)
        {
            ERROR("not expect end of file");
        }
    }
}

void clear_func_vec(FuncVec *function_vec)
{
    if(function_vec)
    {
        for (uint64_t i = 0; i < function_vec->size; i++)
        {
            if(function_vec->funcs[i])
            {
                if(function_vec->funcs[i]->args)
                {
                    if(function_vec->funcs[i]->args->types)
                    {
                        my_free(function_vec->funcs[i]->args->types);
                    }
                    my_free(function_vec->funcs[i]->args);
                }
                if(function_vec->funcs[i]->name)
                {
                    my_free(function_vec->funcs[i]->name);
                }
                if(function_vec->funcs[i]->ret)
                {
                    if(function_vec->funcs[i]->ret->types)
                    {
                        my_free(function_vec->funcs[i]->ret->types);
                    }
                    my_free(function_vec->funcs[i]->ret);
                }
                clear_word_vec(&function_vec->funcs[i]->func_body);
                my_free(function_vec->funcs[i]);
            }
        }
        my_free(function_vec->funcs);
    }
}

void write_function(FILE *file, Func *func)
{
    DataTypeStack dt_stack = {0};
    dt_stack.types = (DataType *)my_malloc(MAX_NUMBER_OF_FUNCTION_PARAMETERS * sizeof(DataType));
    uint64_t stack_size = 0;

    for (uint64_t i = 0; i < func->args->size; i++)
    {
        data_type_stack_push(&dt_stack, &func->args->types[i]);
    }
    fprintf(file, "addr_%s:\n", func->name);
    fprintf(file, "    mov [ret_stack_rsp], rsp\n");
    fprintf(file, "    mov rsp, rax\n");
    for (uint64_t i = 0; i < func->func_body.size; i++)
    {
        write_fasm_file(file, func->func_body.words[i], &dt_stack, &stack_size, i);
    }
    fprintf(file, "    mov rax, rsp\n");
    fprintf(file, "    mov rsp, [ret_stack_rsp]\n");
    fprintf(file, "    ret\n");
    if(dt_stack.size != func->ret->size)
    {
        fprintf(stderr, "%zu %zu\n", dt_stack.size, func->ret->size);
        ERROR("invalid return");
    }
    for (uint64_t i = 0; i < func->ret->size; i++)
    {
        if(dt_stack.types[(dt_stack.size - 1) - i] != func->ret->types[(func->ret->size - 1) - i])
        {
            ERROR("return not expect");
        }
    }
    my_free(dt_stack.types);
}


void write_functions(FILE *file)
{
    for (uint64_t i = 0; i < func_vec.size; i++)
    {
        write_function(file, func_vec.funcs[i]);
    }   
}