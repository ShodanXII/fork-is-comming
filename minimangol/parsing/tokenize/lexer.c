#include "../../minishell.h"


static void add_heredoc(t_redir **head, char *value, int type);
// static void parse_redirection(t_parser *parser, t_command *cmd);
// static void parse_command(t_parser *parser, t_command *cmd);

// static void 




t_redir *handle_redir(t_token *tokens)
{
	t_redir *redir_head = NULL;
	t_redir *redir_current = NULL;
	t_token *current = tokens;
	
	while(current && current->next)
	{
		if((current->type == TOKEN_REDIR_IN || 
			current->type == TOKEN_REDIR_OUT || 
			current->type == TOKEN_HEREDOC || 
			current->type == TOKEN_APPEND) && 
		   current->next->value)
		{
			add_herdoc(&redir_head, &current);
			current = current->next->next;
		}
		else
		{
			current = current->next;
		}
	}
	
	return redir_head;
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


static t_token *handle_word(t_token *tokens)
{
	char *params;
	t_token *next_token;
	t_token *current = tokens;
	int i = 0;
	while(current && current->next)
	{
		// printf("%s\n", current->value);
		next_token = current->next;
		if(current->type == TOKEN_WORD && next_token->type == TOKEN_WORD)
		{
			params = malloc(ft_strlen(current->value) + ft_strlen(next_token->value) + 1);
			if(!params)
				error("malloc mangoli");
			ft_strcpy(params, current->value);
			ft_strcat(params, " ");
			ft_strcat(params, next_token->value);
			free(current->value);
			current->value = params;
			printf("%s\n", current->value);
			current->next = next_token->next;
			free(next_token->value);
			free(next_token);
		}
		else
			current = current->next;
	}
	return (current);
}

static void type_define(char **args, t_token **tokens)
{
	int j = 0;
	t_token *current = NULL;
	t_redir *redir = NULL;
	int redi = 0;
	while(args[j])
	{
		if(ft_strcmp(args[j], ">>") == 0 && args[1])
		{
			add_heredoc(&redir, args[j], TOKEN_APPEND);
			redi = 1;
		}
		else if(ft_strcmp(args[j], "<<") == 0 && args[j])
		{
			add_heredoc(&redir, args[j], TOKEN_HEREDOC);
			redi = 1;
		}
		else if(ft_strcmp(args[j], ">") == 0 && args[j])
		{
			add_heredoc(&redir, args[j], TOKEN_REDIR_OUT);  // Fixed
			redi = 1;
		}
		else if(ft_strcmp(args[j], "<") == 0 && args[j+1])
		{
			add_heredoc(&redir, args[j], TOKEN_REDIR_IN);   // Fixed
			redi = 1;
		}
		else if(ft_strcmp(args[j], "&&") == 0)
			add_token(tokens, &current, args[j], TOKEN_AND);
		else if(ft_strcmp(args[j], "||") == 0)
			add_token(tokens, &current, args[j], TOKEN_OR);
		else if(ft_strcmp(args[j], "|") == 0)
			add_token(tokens, &current, args[j], TOKEN_PIPE);
		else if(redi)
			add_red_file(args[j], &redi);
		else
			add_token(tokens, &current, args[j], TOKEN_WORD);        
		j++;
	}
}

static void add_token(t_token **head, t_token **curr, char *value, int type)
{
	t_token *new;

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
static void add_heredoc(t_redir **head, char *value, int type)
{

	t_redir *new;
	t_redir *curr;
	new = malloc(sizeof(t_redir));
	if (!new)
		return;
	new->file = ft_strdup(value);
	new->type = type;
	new->next = NULL;    
	if (*head == NULL)
	{
		*head = new;
		return;
	}
	curr = *head;
	while (curr->next)
	curr = curr->next;
	curr->next = new;
	printf("%s\n", new->file);
}

// void handle_redir(char **args, int *j, t_token **tokens, t_token **current)
// {
// 	t_redir *redir = NULL;
// 	redir = malloc(sizeof(t_redir));
// 	if (!redir)
// 		return;
// 	redir->file = NULL;
// 	redir->type = 0;
// 	redir->next = NULL;
// 	char *value;    
// 	if (args[*j][0] == '>')
// 	{
// 		if (args[*j][1] && args[*j][1] == '>')
// 		{
// 			redir->type = TOKEN_APPEND;
// 			value = ">>";
// 		} 
// 		else 
// 		{
// 			redir->type = TOKEN_REDIR_OUT; 
// 			value = ">";
// 		}
// 	} 
// 	else if (args[*j][0] && args[*j][0] == '<')
// 	{
// 		if (args[*j][1] == '<') 
// 		{
// 			redir->type = TOKEN_HEREDOC;
// 			value = "<<";
// 		} 
// 		else 
// 		{
// 			redir->type = TOKEN_REDIR_IN;
// 			value = "<";
// 		}
// 	}
// 	add_token(tokens, current, value, redir->type);
// 	(*j)++;
// 	if (args[*j] != NULL && !ft_strchr(args[*j], '|') && !ft_strchr(args[*j], '&') && 
// 	!ft_strchr(args[*j], '(') && !ft_strchr(args[*j], ')') && 
// 	!ft_strchr(args[*j], '>') && !ft_strchr(args[*j], '<'))
// 	{
// 		redir->file = ft_strdup(args[*j]);
// 		(*j)++;
// 	} 
// 	else 
// 	{
// 		// makaynx file
// 		// Handle error to do
// 		(*j)--; // Stay at current position for next iteration
// 	}
// }



void print_tokens(t_token *tokens)
{
	t_token *current = tokens;
	printf("--- Token List ---\n");
	while (current)
	{
		printf("Type: %d, Value: '%s'\n", current->type, current->value);
		current = current->next;
	}
	printf("-----------------\n");
}


int count_word(char *line)
{
	int count;

	if (!line)
		return (0);
	count = 0;
	while (!ft_strchr("|><&()", line[count]) && line[count] && !ft_isspace(line[count]))
		count++;
	return (count);
}

static void determine_token_type(char *token_value, int *token_type)
{
	t_redir *redir;
	if (ft_strcmp(token_value, "|") == 0)
		*token_type = TOKEN_PIPE;
	else if (ft_strcmp(token_value, "||") == 0)
		*token_type = TOKEN_OR;
	else if (ft_strcmp(token_value, "&") == 0)
		*token_type = TOKEN_WORD;
	else if (ft_strcmp(token_value, "&&") == 0)
		*token_type = TOKEN_AND;
	else if (ft_strcmp(token_value, ">") == 0)
		*token_type = TOKEN_REDIR_OUT;
	else if (ft_strcmp(token_value, ">>") == 0)
		*token_type = TOKEN_APPEND;
	else if (ft_strcmp(token_value, "<") == 0)
		*token_type = TOKEN_REDIR_IN;
	else if (ft_strcmp(token_value, "<<") == 0)
		*token_type = TOKEN_HEREDOC;
	else if (ft_strcmp(token_value, "(") == 0)
		*token_type = TOKEN_LPAREN;
	else if (ft_strcmp(token_value, ")") == 0)
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


t_token *lexer(char *input)
{
	t_redir *redir = NULL
	t_token *tokens = NULL;
	t_token *current = NULL;
	char **args = ft_split(input, ' ');
	int j = 0;
	type_define(args , &tokens);
	redir = handle_redir(&tokens);
	tokens = handle_word(tokens);
	// printf("---->>>>%d\n", tokens->type);
	
	// printf("------->%d\n", tokens->type);
	// free_args(args); to do
	return tokens;
}
// int main()
// {
// 	char *input = "ls | cat";
// 	t_token *tokens = tokenize_compat(input);
	
// 	if (tokens == NULL)
// 	{
// 		printf("Tokenization failed.\n");
// 		return 1;
// 	}
	
// 	print_tokens(tokens);

	
// 	// Free the tokens after use
// 	free_tokens(tokens);
	
// 	return 0;
// }


// {
// 	char *input = "ls | grep 'test' && echo 'hello world' > output.txt";
// 	t_token *tokens = tokenize(input);
	
// 	if (check_syntax_errors(tokens))
// 	{
// 		free_tokens(tokens);
// 		return 1;
// 	}
	
// 	print_tokens(tokens);
	
// 	// Free the tokens after use
// 	free_tokens(tokens);
	
// 	return 0;
// }

// static void parse_redirection(t_parser *parser, t_command *cmd)
// {
// 	t_redir *new;
	
// 	new = malloc(sizeof(t_redir));
// 	if (!new)
// 		return;
// 	new->type = parser->current->type;
// 	parser->current = parser->current->next;
// 	if (!parser->current || parser->current->type != TOKEN_WORD)
// 	{
// 		ft_putstr_fd("minishell: syntax error near unexpected token\n", 2);
// 		free(new);
// 		return;
// 	}
// 	new->file = ft_strdup(parser->current->value);    
// 	new->next = cmd->redirs;
// 	cmd->redirs = new;    
// 	parser->current = parser->current->next;
// }

// static void parse_command(t_parser *parser, t_command *cmd)
// {
// 	int arg_count = 0;
// 	int capacity = 16;
// 	cmd->args = malloc(capacity * sizeof(char *));
	
// 	while (parser->current && 
// 		  (parser->current->type == TOKEN_WORD ||
// 		   parser->current->type >= TOKEN_REDIR_IN))
// 	{
// 		if (parser->current->type != TOKEN_WORD)
// 		{
// 			parse_redirection(parser, cmd);
// 			continue;
// 		}
// 		if (arg_count >= capacity-1)
// 		{
// 			capacity *= 2;
// 			cmd->args = realloc(cmd->args, capacity * sizeof(char *));
// 		}
// 		cmd->args[arg_count++] = ft_strdup(parser->current->value);
// 		parser->current = parser->current->next;
// 	}
// 	cmd->args[arg_count] = NULL;
// }




