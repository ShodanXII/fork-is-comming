#include "../../minishell.h"

static void add_tokens(t_token **head, char *value, int type);

// static t_token handel_paren(char *input)
// {
// 	int i = 0;
// 	int start = 0;
// 	while(input[i])
// 	{
// 		if(input[i] == '(')
// 		{

// 		}
// 	}
// }

char *ft_strjoin_char(char *str, char c)
{
	char    *new;
	size_t  len;

	if (!str)
	{
		new = malloc(2);
		new[0] = c;
		new[1] = '\0';
		return new;
	}
	len = ft_strlen(str);
	new = malloc(len + 2);
	ft_strlcpy(new, str, len + 1);
	new[len] = c;
	new[len + 1] = '\0';
	free(str);
	return new;
}

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

static bool is_redirection(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || 
			type == TOKEN_REDIR_OUT ||
			type == TOKEN_APPEND ||
			type == TOKEN_HEREDOC);
}

t_redir *create_redir_node(t_token_type type, char *file)
{
	t_redir *new_redir = malloc(sizeof(t_redir));
	if (!new_redir)
		return NULL;
	
	new_redir->file = ft_strdup(file);
	if (!new_redir->file) {
		free(new_redir);
		return NULL;
	}
	
	new_redir->type = type;
	new_redir->next = NULL;
	return new_redir;
}

static void add_tokens(t_token **head, char *value, int type)
{
	t_token *new_token;
	t_token *last;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return;
	
	new_token->value = ft_strdup(value);
	new_token->type = type;
	new_token->next = NULL;

	if (!*head)
	{
		*head = new_token;
		return;
	}

	last = *head;
	while (last->next)
		last = last->next;
	last->next = new_token;
}

void remove_token(t_token **head, t_token *to_remove, t_token *prev)
{
	if (!*head || !to_remove)
		return;
	
	if (prev)
		prev->next = to_remove->next;
	else
		*head = to_remove->next;
	
	free(to_remove->value);
	free(to_remove);
}

static int handle_operator(char *input, int i, t_token **tokens)
{
	int type;
	int len;

	type = TOKEN_WORD;
	len = 1;
	if (input[i] == '>' && input[i + 1] == '>')
	{
		type = TOKEN_APPEND;
		len = 2;
	}
	else if (input[i] == '<' && input[i + 1] == '<')
	{
		type = TOKEN_HEREDOC;
		len = 2;
	}
	else if (input[i] == '>')
		type = TOKEN_REDIR_OUT;
	else if (input[i] == '<')
		type = TOKEN_REDIR_IN;
	else if (input[i] == '|' && input[i + 1] == '|')
	{
		type = TOKEN_OR;
		len = 2;
	}
	else if (input[i] == '&' && input[i + 1] == '&')
	{
		type = TOKEN_AND;
		len = 2;
	}
	else if (input[i] == '|')
		type = TOKEN_PIPE;

	char *value = ft_substr(input, i, len);
	add_tokens(tokens, value, type);
	return (i + len);
}

static int handle_quotes(char *input, int i, t_token **tokens, char quote)
{
	int start = i;
	i++;

	while (input[i] && input[i] != quote)
		i++;

	if (!input[i])
	{
		ft_putstr_fd("Error: Unclosed quotes\n", 2);
		return (-1);
	}

	char *value = ft_substr(input, start + 1, i - start - 1);
	add_tokens(tokens, value, TOKEN_WORD);
	return (i + 1);
}

static int handel_paren(char *input, int i, t_token **tokens)
{
	int start = i;
	int flager = 0;
	int beg = 0;
	while(input[i])
	{
		if(input[i] == '(' || input[i] == ')')
		{
			if(input[i] == '(')
			{
				flager ++;
				beg++;
			}	
		}
	}
		// to do
}

static int handle_word(char *input, int i, t_token **tokens)
{
	int start = i;

	while (input[i] && !ft_isspace(input[i]) && !ft_strchr("><|&", input[i]))
		i++;

	char *value = ft_substr(input, start, i - start);
	add_tokens(tokens, value, TOKEN_WORD);
	return (i);
}

t_token *lexer(char *input)
{
	t_token *tokens = NULL;
	int i = 0;
	int result;

	while (input[i])
	{
		if (ft_isspace(input[i]))
		{
			i++;
			continue;
		}

		if (ft_strchr("><|&", input[i]))
			result = handle_operator(input, i, &tokens);
		else if (input[i] == '\'' || input[i] == '"')
			result = handle_quotes(input, i, &tokens, input[i]);
		// else if (input[i] == '(' || input[i] == ')') to do
		// 	handle_paren(input, i, &tokens);
		else
			result = handle_word(input, i, &tokens);
		if (result == -1)
		{
			// free_tokens(tokens);
			return (NULL);
		}
		i = result;
	}
	return (tokens);
}

t_redir *handle_redir(t_token **tokens)
{
	t_token *curr = *tokens;
	t_redir *redirs = NULL;
	t_redir *redir_tail = NULL;

	while (curr && curr->next)
	{
		if (is_redirection(curr->type))
		{
			t_redir *new_redir = create_redir_node(curr->type, curr->next->value);            
			if (!redirs) {
				redirs = new_redir;
				redir_tail = new_redir;
			} else {
				redir_tail->next = new_redir;
				redir_tail = new_redir;
			}            
			curr = curr->next->next;
		}
		else
		{
			curr = curr->next;
		}
	}
	return redirs;
}

void print_redirs(t_redir *redirs)
{
	t_redir *current = redirs;
	printf("\n--- Redirections ---\n");
	while (current)
	{
		printf("Type: %d, File: '%s'\n", current->type, current->file);
		current = current->next;
	}
}

void free_redirs(t_redir *redirs)
{
	t_redir *current = redirs;
	while (current)
	{
		t_redir *next = current->next;
		free(current->file);
		free(current);
		current = next;
	}
}

t_token *merge_consecutive_words(t_token *tokens)
{
	t_token *current = tokens;
	
	while (current && current->next)
	{
		t_token *next = current->next;
		
		if (current->type == TOKEN_WORD && next->type == TOKEN_WORD)
		{
			char *merged = malloc(ft_strlen(current->value) + 
						  ft_strlen(next->value) + 2);
			if (!merged)
				return tokens;
			ft_strcpy(merged, current->value);
			ft_strcat(merged, " ");
			ft_strcat(merged, next->value);
			free(current->value);
			current->value = merged;			
			current->next = next->next;
			free(next->value);
			free(next);			
		}
		else
		{
			current = current->next;
		}
	}
	return tokens;
}
