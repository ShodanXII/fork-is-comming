#include "../../minishell.h"

int check_syntax_errors(t_token *tokens)
{
    t_token *tmp = tokens;
    
    if (!tmp)
        return 1;
    
    // Check for pipe at start
    if (tmp->type == TOKEN_PIPE)
        return error("Syntax error near unexpected token '|'");
    
    while (tmp)
    {
        // Check for consecutive operators
        if ((tmp->type >= TOKEN_REDIR_IN && tmp->type <= TOKEN_PIPE) &&
            tmp->next && (tmp->next->type >= TOKEN_REDIR_IN && tmp->next->type <= TOKEN_PIPE))
            return error("Syntax error near unexpected token");
        
        // Check for operator at end
        if (!tmp->next && (tmp->type >= TOKEN_REDIR_IN && tmp->type <= TOKEN_PIPE))
            return error("Syntax error near unexpected token");
        
        tmp = tmp->next;
    }
    return 0;
}