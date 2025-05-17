#include "../../minishell.h"

void print_redirs(t_redir *redirs)
{
    t_redir *current = redirs;
    printf("--- Redirection List ---\n");
    while (current)
    {
        printf("Type: %d, File: '%s'\n", current->type, current->file);
        current = current->next;
    }
    printf("-----------------------\n");
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

t_redir *extract_redirs_from_command(char **args)
{
    t_redir *head = NULL;
    t_redir *current = NULL;
    int i = 0;
    
    while (args[i])
    {
        if (args[i+1] && (ft_strcmp(args[i], ">") == 0 || 
                          ft_strcmp(args[i], ">>") == 0 ||
                          ft_strcmp(args[i], "<") == 0 || 
                          ft_strcmp(args[i], "<<") == 0))
        {
            t_token_type type;
            if (ft_strcmp(args[i], ">") == 0)
                type = TOKEN_REDIR_OUT;
            else if (ft_strcmp(args[i], ">>") == 0)
                type = TOKEN_APPEND;
            else if (ft_strcmp(args[i], "<") == 0)
                type = TOKEN_REDIR_IN;
            else
                type = TOKEN_HEREDOC;
                
            t_redir *new = create_redir_node(type, args[i+1]);
            if (!head)
                head = new;
            else
                current->next = new;
            current = new;
            i++; // Skip the file name
        }
        i++;
    }
    
    return head;
}
