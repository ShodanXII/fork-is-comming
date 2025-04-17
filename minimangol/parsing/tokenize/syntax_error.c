#include "../../minishell.h"

/* Check if string is a command (not an operator) */
static int ft_is_cmd(char *str)
{
    int i;

    i = 0;
    while (str[i])
    {
        if (str[i] == '|' || str[i] == '&' || str[i] == ';' 
            || str[i] == '<' || str[i] == '>')
            return (0);
        i++;
    }
    return (1);
}

/* Syntax error check before parsing */
int check_syntax_errors(t_token *tokens)
{
    t_token *current;
    t_token *prev;
    
    if (tokens == NULL)
        return (0);
    
    /* Check if first token is a pipe, redirect, or logical operator */
    if (tokens->type == TOKEN_PIPE || tokens->type == TOKEN_APPEND 
        || tokens->type == TOKEN_OR)
    {
        ft_putstr_fd("Error: syntax error near unexpected token `", 2);
        ft_putstr_fd(tokens->value, 2);
        ft_putstr_fd("'\n", 2);
        return (1);
    }
    
    current = tokens;
    prev = NULL;
    
    while (current)
    {
        /* Check redirect errors */
        if (current->type == TOKEN_REDIR_IN || current->type == TOKEN_REDIR_OUT 
            || current->type == TOKEN_APPEND || current->type == TOKEN_HEREDOC)
        {
            if (current->next == NULL || is_operator(current->next->type))
            {
                ft_putstr_fd("Error: syntax error near unexpected token `", 2);
                ft_putstr_fd(current->value, 2);
                ft_putstr_fd("'\n", 2);
                return (1);
            }
        }
        /* Check pipe/logical operator errors */
        else if (current->type == TOKEN_PIPE || current->type == TOKEN_AND 
                || current->type == TOKEN_OR)
        {
            if (current->next == NULL || is_operator_token(current->next->type) 
                || prev == NULL || is_operator_token(prev->type))
            {
                ft_putstr_fd("Error: syntax error near unexpected token `", 2);
                ft_putstr_fd(current->value, 2);
                ft_putstr_fd("'\n", 2);
                return (1);
            }
        }
        /* Check parentheses errors */
        else if (current->type == TOKEN_LPAREN)
        {
            if (current->next == NULL || is_operator_token(current->next->type))
            {
                ft_putstr_fd("Error: syntax error near unexpected token `", 2);
                ft_putstr_fd(current->value, 2);
                ft_putstr_fd("'\n", 2);
                return (1);
            }
        }
        else if (current->type == TOKEN_RPAREN)
        {
            if (prev == NULL || is_operator_token(prev->type))
            {
                ft_putstr_fd("Error: syntax error near unexpected token `", 2);
                ft_putstr_fd(current->value, 2);
                ft_putstr_fd("'\n", 2);
                return (1);
            }
        }
        
        prev = current;
        current = current->next;
    }
    write(1, "Syntax check passed\n", 20);
    return (0);
}

/* Modified build_ast function to include syntax check */