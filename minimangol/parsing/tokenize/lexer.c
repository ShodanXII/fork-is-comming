#include "../../minishell.h"

// int is_operator(char *token);

void flag_error(char *str)
{
    ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
    ft_putstr_fd(str, 2);
    ft_putstr_fd("'\n", 1);
    // prompt();// to do
}

void ft_free_split(char **tokens)
{
    int i = 0;
    while (tokens[i])
    {
        free(tokens[i]);
        i++;
    }
    free(tokens);
}
void check_s_char(char **token_value, int *token_type)
{
    if (ft_strcmp(*token_value, "(") == 0)
        *token_type = TOKEN_LPAREN;
    else if (ft_strcmp(*token_value, ")") == 0)
        *token_type = TOKEN_RPAREN;
    else
        *token_type = TOKEN_WORD;
}
void free_tokens(t_token *tokens)
{
    t_token *current = tokens;
    while (current)
    {
        t_token *next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
}

void create_token(t_token **head, t_token **current, char *value, int type)
{
    t_token *new_token = malloc(sizeof(t_token));
    if (!new_token)
        return;
    new_token->value = ft_strdup(value);
    new_token->type = type;
    new_token->next = NULL;
    if (*head == NULL)
        *head = new_token;
    else
        (*current)->next = new_token;
    *current = new_token;
}

// int	is_operator(char *token)
// {
// 	return (ft_strcmp(token, "|") == 0
// 		|| ft_strcmp(token, "&&") == 0
// 		|| ft_strcmp(token, "||") == 0
// 		|| ft_strcmp(token, ">") == 0
// 		|| ft_strcmp(token, "<") == 0
// 		|| ft_strcmp(token, ">>") == 0
// 		|| ft_strcmp(token, "<<") == 0
// 		|| ft_strcmp(token, "(") == 0
// 		|| ft_strcmp(token, ")") == 0);
// }

static void	add_token(t_token **head, t_token **curr, char *value, int type)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	new->value = ft_strdup(value);
	new->type = type;
	new->next = NULL;

	if (!*head)
		*head = new;
	else
		(*curr)->next = new;
	*curr = new;
}

// static void	process_word(char **tokens, t_token **head, t_token **curr, int *i)
// {
// 	char	*cmd;
// 	char	*tmp;

// 	cmd = ft_strdup(tokens[*i]);
// 	while (tokens[++(*i)] && !is_operator(tokens[*i]))
// 	{
// 		tmp = ft_strjoin(cmd, " ");
// 		free(cmd);
// 		cmd = ft_strjoin(tmp, tokens[*i]);
// 		free(tmp);
// 	}
// 	(*i)--;
// 	add_token(head, curr, cmd, TOKEN_COMMAND);
// 	free(cmd);
// }

t_token	*tokenize(char *input)
{
    t_token	*head = NULL;
    t_token	*curr = NULL;
    char	**tokens = ft_split(input, ' ');
    int		i = 0;

    while (tokens && tokens[i]) {
        if (ft_strcmp(tokens[i], "|") == 0)
            add_token(&head, &curr, tokens[i], TOKEN_PIPE);
        else if (ft_strcmp(tokens[i], "&&") == 0)
            add_token(&head, &curr, tokens[i], TOKEN_AND);
        else if (ft_strcmp(tokens[i], "||") == 0)
            add_token(&head, &curr, tokens[i], TOKEN_OR);
        else if (ft_strcmp(tokens[i], ">") == 0)
            add_token(&head, &curr, tokens[i], TOKEN_REDIR_OUT);
        else if (ft_strcmp(tokens[i], "<") == 0)
            add_token(&head, &curr, tokens[i], TOKEN_REDIR_IN);
        else if (ft_strcmp(tokens[i], ">>") == 0)
            add_token(&head, &curr, tokens[i], TOKEN_APPEND);
        else if (ft_strcmp(tokens[i], "<<") == 0)
            add_token(&head, &curr, tokens[i], TOKEN_HEREDOC);
        else if (ft_strcmp(tokens[i], "(") == 0)
            add_token(&head, &curr, tokens[i], TOKEN_LPAREN);
        else if (ft_strcmp(tokens[i], ")") == 0)
            add_token(&head, &curr, tokens[i], TOKEN_RPAREN);
        else
            add_token(&head, &curr, tokens[i], TOKEN_WORD);  // Key change here
        i++;
    }
    ft_free_split(tokens);
    return head;
}

/* Redirection Parser */
static void parse_redirection(t_parser *parser, t_command *cmd)
{
    t_redir *new = malloc(sizeof(t_redir));
    new->type = parser->current->type;
    parser->current = parser->current->next;

    if (!parser->current || parser->current->type != TOKEN_WORD) {
        ft_putstr_fd("minishell: syntax error near unexpected token\n", 2);
        free(new);
        return;
    }

    new->file = ft_strdup(parser->current->value);
    new->next = cmd->redirs;
    cmd->redirs = new;
    parser->current = parser->current->next;
}

static void parse_command(t_parser *parser, t_command *cmd)
{
    write(1, "arg_count: ", 12);
    int arg_count = 0;
    int capacity = 16;
    cmd->args = malloc(capacity * sizeof(char *));
    
    while (parser->current && 
          (parser->current->type == TOKEN_WORD ||
           parser->current->type >= TOKEN_REDIR_IN)) {
        
        if (parser->current->type != TOKEN_WORD) {
            parse_redirection(parser, cmd);
            continue;
        }
        if (arg_count >= capacity-1) {
            capacity *= 2;
            cmd->args = realloc(cmd->args, capacity * sizeof(char *));
        }
        cmd->args[arg_count++] = ft_strdup(parser->current->value);
        parser->current = parser->current->next;
    }
    cmd->args[arg_count] = NULL;
}

/* Test Function */
void print_command(t_command *cmd)
{
    printf("Command arguments:\n");
    for (int i = 0; cmd->args[i]; i++)
        printf("  [%d] %s\n", i, cmd->args[i]);
    
    printf("Redirections:\n");
    for (t_redir *r = cmd->redirs; r; r = r->next)
        printf("  Type %d -> %s\n", r->type, r->file);
}

void test_parser(char *input)
{
    t_token *tokens = tokenize(input);
    t_parser parser = {tokens};
    t_command cmd = {0};
    
    parse_command(&parser, &cmd);
    print_command(&cmd);
    free(cmd.args);
    free_tokens(tokens);
}

// int main()
// {
//     char *input = "ls -la > ";
//     t_token *tokens = tokenize(input);
//     check_syntax_errors(tokens);
//     t_parser parser = {tokens};
//     t_ast *ast = parse(&parser);
    
//     print_ast(ast, 0);
//     // cleaner(); // to do!!    
//     return 0;
// }
// void print_tokens(t_token *tokens)
// {
//     t_token *current = tokens;
//     while (current) {
//         printf("Type: %d, Value: %s\n", current->type, current->value);
//         current = current->next;
//     }
//     printf("\n");
// }

/* Parse pipe sequences */
// t_ast *parse_pipeline(t_token **tokens)
// {
//     t_ast *left;
//     t_ast *node;
    
//     left = parse_command(tokens);
    
//     if (*tokens != NULL && (*tokens)->type == NODE_PIPE)
//     {
//         *tokens = (*tokens)->next; // Consume the pipe
        
//         node = create_node(NODE_PIPE);
//         node->left = left;
//         node->right = parse_pipeline(tokens);
//         return node;
//     }
    
//     return left;
// }

/* Parse simple commands */
// t_ast *parse_command(t_token **tokens)
// {
//     t_ast *node;
//     int arg_count;
//     t_token *current;
//     int i;
    
//     if (*tokens == NULL || (*tokens)->type != NODE_COMMAND)
//         return NULL;
        
//     node = create_node(NODE_COMMAND);
    
//     // Count args
//     arg_count = 0;
//     current = *tokens;
//     while (current != NULL && current->type == NODE_COMMAND)
//     {
//         arg_count++;
//         current = current->next;
//     }
    
//     // Allocate args array
//     node->args = malloc(sizeof(char*) * (arg_count + 1));
//     if (!node->args)
//         return NULL;
    
//     // Fill args array
//     i = 0;
//     while (i < arg_count)
//     {
//         node->args[i] = ft_strdup((*tokens)->value);
//         *tokens = (*tokens)->next;
//         i++;
//     }
//     node->args[i] = NULL;
    
//     return node;
// }


// void print_tokens(t_token *tokens)
// {
//     t_token *current = tokens;
//     while (current) {
//         printf("Type: %d, Value: %s\n", current->type, current->value);
//         current = current->next;
//     }
//     printf("\n");
// }
