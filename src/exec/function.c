#include "ast/ast.h"
#include "utils/hash_map.h"
#include "variables/variables.h"

void insert_function(struct ast *ast)
{
    struct hash_map *hm_funcs = get_functions();
    struct ast *copy = ast_deep_copy(ast->children[0]);
    hash_map_insert(hm_funcs, strdup(ast->value[0]), copy);
}
