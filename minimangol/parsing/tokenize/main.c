#include "../../minishell.h"

const char *get_token_type_name(t_token_type type)
{
	switch (type)
	{
		case TOKEN_WORD:       return "WORD";
		case TOKEN_RPAREN:     return "RPAREN";
		case TOKEN_REDIR_IN:   return "REDIR_IN";
		case TOKEN_REDIR_OUT:  return "REDIR_OUT";
		case TOKEN_APPEND:     return "APPEND";
		case TOKEN_HEREDOC:    return "HEREDOC";
		case TOKEN_LPAREN:     return "LPAREN";
		case TOKEN_PIPE:       return "PIPE";
		case TOKEN_OR:         return "OR";
		case TOKEN_AND:        return "AND";
		default:               return "UNKNOWN";
	}
}

void print_ast_horizontal(struct s_ast *node, int level)
{
	if (!node)
		return;

	// Print right subtree first (it goes on the right side)
	print_ast_horizontal(node->right, level + 1);

	// Indentation per level
	for (int i = 0; i < level; i++)
		printf("    ");

	// Print token type (and command if present)
	printf("[%s]", get_token_type_name(node->e_token_type));
	if (node->cmd)
		printf(" -> %s", node->cmd);
	printf("\n");

	// Print left subtree next (it goes on the left side)
	print_ast_horizontal(node->left, level + 1);
}

int main(int ac, char **av, char **env)
{
    extern char **environ;
    t_ast *head = NULL;
    t_redir *redirs = NULL;
    t_token *tokens = NULL;    
    char *input = "echo '\"hello world\"' > test.txt";    
    tokens = lexer(input);
    if (!tokens)
    {
        printf("Lexer error!\n");
        return 1;
    }
    redirs = handle_redir(&tokens); // handli redir
    tokens = merge_consecutive_words(tokens); //jma3 token w params
    printf("\n--- Merged Tokens ---\n");
    print_tokens(tokens);
    printf("\n--- After Redirection Processing ---\n");
    print_tokens(tokens);
    print_redirs(redirs);
    head = function_lmli7a(tokens, NULL); // a77777 ala tree dyali
	print_ast_horizontal(head, 0);
    if (!head)
    {
        printf("Parser error!\n");
        // free_tokens(tokens);
        // free_redirs(redirs);
        return 1;
    }
    printf("\n--- Execution ---\n");
    execute_tree(head, 0, 1, -1, environ);
    printf("exited \n");

    // Cleanup to do
    // free_ast(head);
    // free_tokens(tokens);
    // free_redirs(redirs);

    return 0;
}