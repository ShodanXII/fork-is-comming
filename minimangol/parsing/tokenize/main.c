#include "../../minishell.h"

void free_tokens(t_token *tokens)
{
    t_token *tmp;
    
    while (tokens)
    {
        tmp = tokens;
        tokens = tokens->next;
        free(tmp->value);
        free(tmp);
    }
}

static const char *get_token_type_name(t_token_type type)
{
    switch (type)
    {
        case TOKEN_WORD:       return "WORD";
        case TOKEN_REDIR_IN:   return "REDIR_IN";
        case TOKEN_REDIR_OUT:  return "REDIR_OUT";
        case TOKEN_APPEND:     return "APPEND";
        case TOKEN_HEREDOC:    return "HEREDOC";
        case TOKEN_PIPE:       return "PIPE";
        case TOKEN_AND:       return "AND";
        case TOKEN_OR:        return "OR";
        default:               return "UNKNOWN";
    }
}

static void print_ast_horizontal(t_ast *node, int level)
{
    if (!node) return;
    print_ast_horizontal(node->right, level + 1);
    for (int i = 0; i < level; i++)
        printf("    ");
    printf("[%s]", get_token_type_name(node->e_token_type));
    if (node->cmd)
        printf(" -> %s", node->cmd);
    printf("\n");
    print_ast_horizontal(node->left, level + 1);
}

static void cleanup(t_token *tokens, t_redir *redirs, t_ast *head, char *input)
{
    if (tokens) free_tokens(tokens);
    // if (redirs) free_redirs(redirs);
    // if (head) free_ast(head);
    if (input) free(input);
}

static void print_debug_info(t_token *tokens, t_redir *redirs, t_ast *head)
{
    printf("\n=== DEBUG OUTPUT ===\n");
    printf("\n[TOKENS]\n");
    print_tokens(tokens);
    printf("\n[REDIRECTIONS]\n");
    print_redirs(redirs);
    printf("\n[ABSTRACT SYNTAX TREE]\n");
    print_ast_horizontal(head, 0);
    printf("\n====================\n");
}

static int execute_command_sequence(char *input)
{
    extern char **environ;
    t_ast *head = NULL;
    t_redir *redirs = NULL;
    t_token *tokens = NULL;

    tokens = lexer(input);
    if (!tokens)
    {
        printf("Lexer error in input: %s\n", input);
        free(input);
        return 1;
    }

    // Handle redirections (ensure this function exists in your project)
    redirs = handle_redir(&tokens);
    
    // Syntax checking
    // if (check_syntax_errors(tokens))
    // {
    //     cleanup(tokens, redirs, head, input);
    //     return 1;
    // }

    // tokens = merge_consecutive_words(tokens);
    head = build_ast(tokens);
    
    if (!head)
    {
        printf("Parser error!\n");
        cleanup(tokens, redirs, head, input);
        return 1;
    }

    #ifdef DEBUG
    print_debug_info(tokens, redirs, head);
    #endif

    int status = execute_tree(head, 0, 1, -1, environ);
    cleanup(tokens, redirs, head, input);
    return status;
}

int main(int ac, char **av, char **env)
{
    char *input;
    (void)ac;
    (void)av;
    (void)env;

    while (1)
    {
        const char *prompt = "\001\033[0;31m\002MINISHELL𒉭 > \001\033[0m\002";
        input = readline(prompt);
        
        if (!input)
        {
            printf("\thala!\n");
            break;
        }
        
        if (ft_strlen(input) == 0)
        {
            free(input);
            continue;
        }
        
        add_history(input);
        execute_command_sequence(input);
    }
    
    rl_clear_history();
    return 0;
}