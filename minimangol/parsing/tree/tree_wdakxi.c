#include "../../minishell.h"

static int get_token_precedence(t_token *token);
int is_operator(t_token *token);
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

int get_precedence(int token_type)
{
	if (token_type == TOKEN_REDIR_IN || token_type == TOKEN_REDIR_OUT || 
		token_type == TOKEN_APPEND || token_type == TOKEN_HEREDOC)
		return 3;  // Highest precedence
	else if (token_type == TOKEN_PIPE)
		return 2;
	else if (token_type == TOKEN_AND)
		return 1;
	else if (token_type == TOKEN_OR)
		return 0;
	return -1;
}


t_ast *function_lmli7a(t_token *tokens, t_token *fin_t7bs)
{
	
	if (!tokens)
		return NULL;
	t_ast *head = NULL;
	t_ast *left = NULL;
	t_ast *right = NULL;
	t_token *current = tokens;
	t_token *highest_ptr = current;
	if(!current || current->next == fin_t7bs)
		return head;
	while(current && current != fin_t7bs)
	{
		if(is_operator(current))
		{
			if(!highest_ptr)
				highest_ptr = current;
			else if(get_token_precedence(current) >= get_token_precedence(highest_ptr))
				highest_ptr = current;
		}
		current = current->next;
	}
	if (highest_ptr)
	{
		head = malloc(sizeof(t_ast));
		head->e_token_type = highest_ptr->type;
		head->cmd = ft_strdup(highest_ptr->value);
		if(highest_ptr->type != TOKEN_WORD)
		{
			head->left = function_lmli7a(tokens, highest_ptr);
			head->right = function_lmli7a(highest_ptr->next, fin_t7bs);
		}
	}
	return head;
}

int is_operator(t_token *token)
{
	if (!token)
		return 0;
	if (token->type == TOKEN_AND || token->type == TOKEN_OR
		|| token->type == TOKEN_PIPE || token->type == TOKEN_REDIR_IN
		|| token->type == TOKEN_REDIR_OUT || token->type == TOKEN_APPEND
		|| token->type == TOKEN_HEREDOC)
		return 1;
	return 0;
}

static int get_token_precedence(t_token *token)
{
	if (token->type == TOKEN_WORD)
		return 1;
	if (token->type == TOKEN_AND || token->type == TOKEN_OR)
		return 4;
	if (token->type == TOKEN_PIPE)
		return 3;
	if (token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT
		|| token->type == TOKEN_APPEND || token->type == TOKEN_HEREDOC)
		return 2;
	return 0;
}

void free_ast(t_ast *node)
{
	if (node == NULL)
		return;
	free_ast(node->left);
	free_ast(node->right);
	free(node->cmd);
	free(node);
}

int main()
{
	t_token *tokens = tokenize("ls -l | cat j");
	t_token *fin_t7bs = NULL;
	t_ast *head = function_lmli7a(tokens, NULL);
    print_ast_horizontal(head, 0);
	free_ast(head);
	return 0;
}


// && || | cmd redirection

//           &&
//          /  \
//         /    \
//        /      \
//       /        \
//      &&         echo hi
//     /   \
//    /     \
// ls -l    |
//          / \
//         /   \
//      cat j   la