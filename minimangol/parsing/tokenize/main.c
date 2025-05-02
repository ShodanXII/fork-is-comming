#include "../../minishell.h"

int main(int ac, char **av, char **env)
{
    extern char **environ;
    t_ast *head = NULL;
    t_redir *redirs = NULL;
    t_token *tokens = NULL;    
    char *input = "ls -la > minishell.h";    
    tokens = lexer(input);
    if (!tokens)
    {
        printf("Lexer error!\n");
        return 1;
    }
    tokens = merge_consecutive_words(tokens); //jma3 token w params
    printf("\n--- Merged Tokens ---\n");
    print_tokens(tokens);
    redirs = handle_redir(&tokens); // handli redir
    printf("\n--- After Redirection Processing ---\n");
    print_tokens(tokens);
    print_redirs(redirs);

    // 4. Parse tokens into AST
    head = function_lmli7a(tokens, NULL);
    if (!head)
    {
        printf("Parser error!\n");
        // free_tokens(tokens);
        // free_redirs(redirs);
        return 1;
    }

    // 5. Execute the AST
    printf("\n--- Execution ---\n");
    execute_tree(head, 0, 1, -1, environ);
    printf("exited \n");

    // 6. Cleanup
    // free_ast(head);
    // free_tokens(tokens);
    // free_redirs(redirs);

    return 0;
}