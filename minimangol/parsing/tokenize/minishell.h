#ifndef minishell_h
# define minishell_h


# include <stdbool.h> // for bool
# include <stdlib.h>  // for malloc, free
# include <stdio.h>   // for fprintf
# include <string.h>  // for strdup

// You may need to include token types from minishell.h or define them here


// Stack functions
void        push(t_stack **stack, t_ast_node *node);
t_ast_node *pop(t_stack **stack);
t_ast_node *peek(t_stack *stack);
bool        is_empty(t_stack *stack);

// Output queue functions
void        add_to_queue(t_ast_node **queue, t_ast_node *node);
t_ast_node *pop_from_queue(t_ast_node **queue);

// AST node utilities
t_ast_node *init_node(t_ast_node *token, char **args);
char      **copy_args(t_ast_node **token);
void        add_node(t_ast_node **head, t_ast_node *new_node);
void        free_ast(t_ast_node *node);

// AST construction
t_ast_node *build_ast_from_postfix(t_ast_node **postfix_queue);
t_ast_node *create_ast_from_tokens(t_ast_node *tokens);

#endif
