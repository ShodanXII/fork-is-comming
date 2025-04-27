#include "../../minishell.h"

void handle_quotes(t_token **head, t_token **curr, char *input, int *i);
void handle_word(t_token **head, t_token **curr, char *input, int *i);
// static void parse_redirection(t_parser *parser, t_command *cmd);
// static void parse_command(t_parser *parser, t_command *cmd);
void handle_operator(t_token **head, t_token **curr, char *input, int *i);


// void flag_error(char *str)
// {
// 	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
// 	ft_putstr_fd(str, 2);
// 	ft_putstr_fd("'\n", 1);
// }

// void ft_free_split(char **tokens)
// {
// 	int i = 0;
// 	while (tokens[i])
// 	{
// 		free(tokens[i]);
// 		i++;
// 	}
// 	free(tokens);
// }

// void check_s_char(char **token_value, int *token_type)
// {
// 	if (ft_strcmp(*token_value, "(") == 0)
// 		*token_type = TOKEN_LPAREN;
// 	else if (ft_strcmp(*token_value, ")") == 0)
// 		*token_type = TOKEN_RPAREN;
// 	else
// 		*token_type = TOKEN_WORD;
// }

// void free_tokens(t_token *tokens)
// {
// 	t_token *current = tokens;
// 	while (current)
// 	{
// 		t_token *next = current->next;
// 		free(current->value);
// 		free(current);
// 		current = next;
// 	}
// }

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

// int ft_isspace(char c)
// {
//     if (c == ' ' || c == '\t' || c == '\n')
//         return (1);
//     return (0);
// }

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
    if (ft_strcmp(token_value, "|") == 0)
        *token_type = TOKEN_PIPE;
    else if (ft_strcmp(token_value, "||") == 0)
        *token_type = TOKEN_OR;
    else if (ft_strcmp(token_value, "&") == 0)
        *token_type = TOKEN_WORD; // Regular & is not a special token
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

t_token *tokenize_compat(char *line)
{
    t_token *head = NULL;
    t_token *current = NULL;
    t_token *new_token = NULL;
    int word_len;
    int i = 0;
    int token_type;
    char token_value[1024]; // Increased buffer size for combining words
    char temp_value[256];   // Temporary buffer for individual words
    int is_first_word = 1;  // Flag to track first word in a command
    
    if (!line)
        return NULL;
    
    while (line[i])
    {
        // Skip whitespace
        while (ft_isspace(line[i]))
            i++;
            
        if (!line[i])
            break;
            
        // Handle operators
        if (ft_strchr("|>&<()", line[i]))
        {
            // When we hit an operator, finish current command if any
            if (new_token && new_token->type == TOKEN_WORD)
            {
                is_first_word = 1;  // Reset for next command
                
                // Add the token to the list
                if (!head)
                    head = new_token;
                else
                    current->next = new_token;
                
                current = new_token;
                new_token = NULL;
            }
            
            if ((line[i] == '&' && line[i + 1] == '&') ||
                (line[i] == '|' && line[i + 1] == '|') ||
                (line[i] == '>' && line[i + 1] == '>') ||
                (line[i] == '<' && line[i + 1] == '<'))
            {
                // Double-character operators
                temp_value[0] = line[i];
                temp_value[1] = line[i + 1];
                temp_value[2] = '\0';
                i += 2;
            }
            else
            {
                // Single-character operators
                temp_value[0] = line[i];
                temp_value[1] = '\0';
                i++;
            }
            determine_token_type(temp_value, &token_type);
            
            // Create operator token
            new_token = malloc(sizeof(t_token));
            if (!new_token)
                return NULL;
            
            new_token->value = ft_strdup(temp_value);
            new_token->type = token_type;
            new_token->next = NULL;
            
            // Add the token to the list
            if (!head)
                head = new_token;
            else
                current->next = new_token;
            
            current = new_token;
            new_token = NULL;
        }
        else
        {
            // Handle words
            word_len = count_word(&line[i]);
            int j = 0;
            while (j < word_len)
                temp_value[j++] = line[i++];
            temp_value[j] = '\0';
            
            // If this is the first word or there's no existing token, create new one
            if (is_first_word)
            {
                new_token = malloc(sizeof(t_token));
                if (!new_token)
                    return NULL;
                
                strcpy(token_value, temp_value);
                new_token->value = ft_strdup(token_value);
                new_token->type = TOKEN_WORD;
                new_token->next = NULL;
                is_first_word = 0;
            }
            else 
            {
                // Append the word to the existing command
                strcat(token_value, " ");
                strcat(token_value, temp_value);
                free(new_token->value);
                new_token->value = ft_strdup(token_value);
            }
        }
    }
    
    // Add the final token if there is one
    if (new_token)
    {
        if (!head)
            head = new_token;
        else
            current->next = new_token;
    }
    
    return head;
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
