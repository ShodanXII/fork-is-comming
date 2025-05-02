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
	print_ast_horizontal(node->right, level + 1);
	for (int i = 0; i < level; i++)
		printf("    ");
	printf("[%s]", get_token_type_name(node->e_token_type));
	if (node->cmd)
		printf(" -> %s", node->cmd);
	printf("\n");
	print_ast_horizontal(node->left, level + 1);
}

int execute_command_sequence(char *input)
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
    redirs = handle_redir(&tokens);    
    tokens = merge_consecutive_words(tokens);    
    head = function_lmli7a(tokens, NULL);
    if (!head)
    {
		printf("Parser error!\n");
        // cleanup(tokens, redirs, head, input);
        return 1;
    }	
    execute_tree(head, 0, 1, -1, environ);
    return 0;
}

void print_debug_info(t_token *tokens, t_redir *redirs, t_ast *head)
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

// void cleanup(t_token *tokens, t_redir *redirs, t_ast *head, char *input)
// {
// 	if (tokens) free_tokens(tokens);
//     if (redirs) free_redirs(redirs);
//     if (head) free_ast(head);
//     if (input) free(input);
// }

int main(int ac, char **av, char **env)
{
	char *input;
	while (1)
	{
		const char *prompt = "\001\033[0;31m\002MINISHELL𒉭 > \001\033[0m\002";
		input = readline(prompt);
		if (!input) // Handle Ctrl+D
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
	
	return 0;
}