#include "../../minishell.h"

void flag_error(char *str)
{
    ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
    ft_putstr_fd(str, 2);
    ft_putstr_fd("'\n", 1);
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

t_token *tokenize(char *input)
{
    t_token *head = NULL;
    t_token *curr = NULL;
    int i = 0;
    
    while (input[i])
    {
        if (isspace(input[i]))
        {
            i++;
            continue;
        }
        
        if (input[i] == '|' || input[i] == '&' || 
            input[i] == '<' || input[i] == '>' || 
            input[i] == '(' || input[i] == ')')
        {
            handle_operator(&head, &curr, input, &i);
            continue;
        }
        
        if (input[i] == '"' || input[i] == '\'')
        {
            handle_quotes(&head, &curr, input, &i);
            continue;
        }
        
        handle_word(&head, &curr, input, &i);
    }
    
    return head;
}

void handle_operator(t_token **head, t_token **curr, char *input, int *i)
{
    if (input[*i] == '&' && input[*i + 1] == '&')
    {
        add_token(head, curr, "&&", TOKEN_AND);
        (*i) += 2;
    }
    else if (input[*i] == '|' && input[*i + 1] == '|')
    {
        add_token(head, curr, "||", TOKEN_OR);
        (*i) += 2;
    }
    else if (input[*i] == '>' && input[*i + 1] == '>')
    {
        add_token(head, curr, ">>", TOKEN_APPEND);
        (*i) += 2;
    }
    else if (input[*i] == '<' && input[*i + 1] == '<')
    {
        add_token(head, curr, "<<", TOKEN_HEREDOC);
        (*i) += 2;
    }
    else if (input[*i] == '|')
    {
        add_token(head, curr, "|", TOKEN_PIPE);
        (*i)++;
    }
    else if (input[*i] == '>')
    {
        add_token(head, curr, ">", TOKEN_REDIR_OUT);
        (*i)++;
    }
    else if (input[*i] == '<')
    {
        add_token(head, curr, "<", TOKEN_REDIR_IN);
        (*i)++;
    }
    else if (input[*i] == '(')
    {
        add_token(head, curr, "(", TOKEN_LPAREN);
        (*i)++;
    }
    else if (input[*i] == ')')
    {
        add_token(head, curr, ")", TOKEN_RPAREN);
        (*i)++;
    }
}

void handle_quotes(t_token **head, t_token **curr, char *input, int *i)
{
    char quote = input[*i];
    int start = ++(*i);
    
    while (input[*i] && input[*i] != quote)
    {
        if (quote == '"' && input[*i] == '\\' && input[*i + 1])
            (*i)++;
        (*i)++;
    }
    
    int len = *i - start;
    char *content = ft_substr(input, start, len);
    add_token(head, curr, content, TOKEN_WORD);
    free(content);
    
    if (input[*i])
        (*i)++;
}

void handle_word(t_token **head, t_token **curr, char *input, int *i)
{
    int start = *i;
    
    while (input[*i] && !isspace(input[*i]) && 
           input[*i] != '|' && input[*i] != '&' && 
           input[*i] != '<' && input[*i] != '>' && 
           input[*i] != '(' && input[*i] != ')' && 
           input[*i] != '\'' && input[*i] != '"')
    {
        if (input[*i] == '\\' && input[*i + 1])
            (*i) += 2;
        else
            (*i)++;
    }
    
    int len = *i - start;
    char *word = ft_substr(input, start, len);
    add_token(head, curr, word, TOKEN_WORD);
    free(word);
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

int check_syntax_errors(t_token *tokens)
{
    t_token *current = tokens;
    int paren_count = 0;
    
    while (current)
    {
        if (current->type == TOKEN_LPAREN)
            paren_count++;
        else if (current->type == TOKEN_RPAREN)
            paren_count--;
        
        if (paren_count < 0)
        {
            flag_error(")");
            return 1;
        }
        
        if (!current->next && 
            (current->type == TOKEN_PIPE || 
             current->type == TOKEN_AND || 
             current->type == TOKEN_OR || 
             current->type == TOKEN_REDIR_IN || 
             current->type == TOKEN_REDIR_OUT ||
             current->type == TOKEN_APPEND ||
             current->type == TOKEN_HEREDOC))
        {
            flag_error("newline");
            return 1;
        }
        
        current = current->next;
    }
    
    if (paren_count > 0)
    {
        flag_error("(");
        return 1;
    }
    
    return 0;
}

static void parse_redirection(t_parser *parser, t_command *cmd)
{
    t_redir *new;
    
    new = malloc(sizeof(t_redir));
    if (!new)
        return;
    new->type = parser->current->type;
    parser->current = parser->current->next;
    if (!parser->current || parser->current->type != TOKEN_WORD)
    {
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
           parser->current->type >= TOKEN_REDIR_IN))
    {
        if (parser->current->type != TOKEN_WORD)
        {
            parse_redirection(parser, cmd);
            continue;
        }
        if (arg_count >= capacity-1)
        {
            capacity *= 2;
            cmd->args = realloc(cmd->args, capacity * sizeof(char *));
        }
        cmd->args[arg_count++] = ft_strdup(parser->current->value);
        parser->current = parser->current->next;
    }
    cmd->args[arg_count] = NULL;
}
