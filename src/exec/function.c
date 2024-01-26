#include "ast/ast.h"
#include "utils/hash_map.h"
#include "variables/variables.h"

void insert_function(struct ast *ast)
{
    struct hash_map *hm_funcs = get_functions();
    hash_map_insert(hm_funcs, ast->value[0], ast->children[0]);
}
