#include "lexer/lexer.h"

#include <stdlib.h>
#include <string.h>

#include "lexer/get_token_type.h"
#include "lexer/handle_special_cases.h"
#include "utils/vector.h"

struct lexer *create_lexer(FILE *input)
{
    struct lexer *result = calloc(1, sizeof(struct lexer));
    // Check for NULL
    if (result == NULL)
    {
        return NULL;
    }
    result->input = input;
    result->pos = 0;
    result->prev = CHAR_NONE;
    result->ls.curr_tok = TOKEN_NONE;
    result->ls_next.curr_tok = TOKEN_NONE;
    // result->stack = create_stack();
    return result;
}

void free_lexer(struct lexer *lexer)
{
    if (lexer != NULL)
    {
        if (lexer->ls.curr_tok != TOKEN_NONE)
        {
            free(lexer->ls.tok_str);
        }
        // destroy_stack(lexer->stack);
        // lexer->stack = NULL;
        free(lexer);
    }
}

/*
 * Returns true if the current character belongs in the word
 * False if not
 */
static bool is_continuous_word(char character)
{
    return character != ' ' && character != ';' && character != '\n'
        && character != '\t' && character != '|' && character != '&'
        && character != '<' && character != '>' && character != EOF
        && character != '\0' && character != '(' && character != ')';
}

static void ignore_line(struct lexer *lexer)
{
    char c = fgetc(lexer->input);
    while (c != '\n' && c != EOF && c != '\0')
    {
        c = fgetc(lexer->input);
    }
}

static struct lexer_token_save get_part_one(struct lexer *lexer)
{
    char c = lexer->prev;
    if (c == CHAR_NONE)
    {
        c = fgetc(lexer->input);
    }
    if (c == '\\')
    {
        return handle_escape(lexer);
    }
    // Remove all spaces before the word
    c = skip_blanks(lexer, c);
    if (c == '\\')
    {
        return handle_escape(lexer);
    }
    while (c == '#')
    {
        ignore_line(lexer);
        c = fgetc(lexer->input);
    }
    struct lexer_token_save out = get_special_character(lexer, c);
    return out;
}

struct lexer_token_save main_loop(struct lexer *lexer, struct vector *vec)
{
    struct lexer_token_save out;
    out.curr_tok = TOKEN_NONE;
    char c = lexer->prev;
    while (is_continuous_word(c))
    {
        if (c == '\\')
        {
            c = fgetc(lexer->input);
            if (c == '\n')
            {
                c = fgetc(lexer->input);
                continue;
            }
            // This is to differentiate if and i\f
            // Yes, they are treated differently
            out.curr_tok = TOKEN_ESCAPED_WORD;
            vector_append(vec, '\\');
            vector_append(vec, c);
        }
        else if (c == '\'')
        {
            out = handle_single_quote(lexer, vec);
            if (out.curr_tok == TOKEN_STDIN)
            {
                vector_destroy(vec);
                return out;
            }
        }
        else if (c == '"')
        {
            out = handle_double_quote(lexer, vec);
            if (out.curr_tok == TOKEN_STDIN)
            {
                vector_destroy(vec);
                return out;
            }
        }
        else if (c == '=' && out.curr_tok != TOKEN_WORD)
        {
            return handle_assignment(lexer, vec);
        }
        else if (c == '$')
        {
            vector_append(vec, c);
            c = fgetc(lexer->input);
            if (c == '{')
            {
                out = handle_brackets(lexer, vec);
                if (out.curr_tok == TOKEN_STDIN)
                {
                    return out;
                }
                c = fgetc(lexer->input);
            }
            continue;
        }
        else
        {
            vector_append(vec, c);
        }
        c = fgetc(lexer->input);
    }
    lexer->prev = c;
    return out;
}

static struct lexer_token_save get_next_token(struct lexer *lexer)
{
    struct lexer_token_save out = get_part_one(lexer);
    if (out.curr_tok != TOKEN_NONE)
    {
        return out;
    }
    struct vector *vec = vector_create(100);
    out = main_loop(lexer, vec);
    if (out.curr_tok != TOKEN_NONE && out.curr_tok != TOKEN_WORD
        && out.curr_tok != TOKEN_ESCAPED_WORD)
    {
        return out;
    }
    vector_append(vec, '\0');
    if (out.curr_tok == TOKEN_NONE)
    {
        out = match_word(lexer, vec->data);
    }
    out.tok_str = strdup(vec->data);
    vector_destroy(vec);
    return out;
}

struct lexer_token_save lexer_peek(struct lexer *lexer)
{
    if (lexer->ls.curr_tok == TOKEN_NONE)
    {
        lexer->ls = get_next_token(lexer);
    }
    // printf("%i\n", lexer->ls.curr_tok);
    return lexer->ls;
}

struct lexer_token_save lexer_next_peek(struct lexer *lexer)
{
    if (lexer->ls.curr_tok == TOKEN_NONE)
    {
        lexer->ls = get_next_token(lexer);
    }
    if (lexer->ls_next.curr_tok == TOKEN_NONE)
    {
        lexer->ls_next = get_next_token(lexer);
    }
    // printf("%i\n", lexer->ls.curr_tok);
    return lexer->ls_next;
}
void lexer_pop(struct lexer *lexer, bool to_free)
{
    if (lexer->ls.curr_tok == TOKEN_NONE)
    {
        get_next_token(lexer);
    }
    if (to_free)
    {
        free(lexer->ls.tok_str);
    }
    if (lexer->ls_next.curr_tok != TOKEN_NONE)
    {
        lexer->ls = lexer->ls_next;
        lexer->ls_next.curr_tok = TOKEN_NONE;
    }
    else
    {
        lexer->ls.curr_tok = TOKEN_NONE;
    }
}

/*void lexer_savestate(struct lexer *lexer)
{
    long value = ftell(lexer->input);
    push_stack(lexer->stack, value);
}

bool lexer_loadstate(struct lexer *lexer)
{
    if (lexer->stack == NULL)
    {
        return false;
    }
    //long value = pop_stack(&(lexer->stack));
    fseek(lexer->input, value, SEEK_SET);
    return true;
}*/
