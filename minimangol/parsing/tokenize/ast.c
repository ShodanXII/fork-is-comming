#include "../../minishell.h"

t_ast_node *parse_command(t_parser *parser);
t_ast_node *parse_pipe(t_parser *parser);

// Main entry point for parsing
t_ast_node *parse(t_parser *parser)
{
    return parse_pipe(parser);
}

// Handle pipe sequences
t_ast_node *parse_pipe(t_parser *parser)
{
    t_ast_node *left = parse_command(parser);
    
    if (parser->current && parser->current->type == TOKEN_PIPE)
    {
        t_ast_node *node = malloc(sizeof(t_ast_node));
        node->e_type = AST_PIPE;
        node->left = left;
        parser->current = parser->current->next;
        node->right = parse_pipe(parser);
        return node;
    }
    return left;
}

// Handle commands with arguments and redirections
t_ast_node *parse_command(t_parser *parser)
{
    t_ast_node *node = malloc(sizeof(t_ast_node));
    node->e_type = AST_COMMAND;
    node->args = NULL;
    node->redirs = NULL;
    
    t_command cmd = {0};
    int capacity = 16;
    cmd.args = malloc(capacity * sizeof(char *));

    while (parser->current && parser->current->type != TOKEN_PIPE 
           && parser->current->type != TOKEN_AND 
           && parser->current->type != TOKEN_OR)
    {
        if (parser->current->type >= TOKEN_REDIR_IN)
        {
            // Handle redirection
            t_redir *new = malloc(sizeof(t_redir));
            new->type = parser->current->type;
            parser->current = parser->current->next;
            
            if (parser->current && parser->current->type == TOKEN_WORD)
            {
                new->file = ft_strdup(parser->current->value);
                new->next = cmd.redirs;
                cmd.redirs = new;
                parser->current = parser->current->next;
            }
            else
            {
                ft_putstr_fd("minishell: syntax error\n", 2);
                free(new);
            }
        }
        else
        {
            // Handle command arguments
            if (cmd.arg_count >= capacity)
            {
                capacity *= 2;
                cmd.args = realloc(cmd.args, capacity * sizeof(char *));
            }
            cmd.args[cmd.arg_count++] = ft_strdup(parser->current->value);
            parser->current = parser->current->next;
        }
    }
    
    cmd.args[cmd.arg_count] = NULL;
    node->args = cmd.args;
    node->redirs = cmd.redirs;
    return node;
}

// Simple print function
void print_ast(t_ast_node *node, int level)
{
    if (!node) return;
    
    // Indentation based on depth
    for (int i = 0; i < level; i++) 
        printf("  ");
    
    if (node->e_type == AST_COMMAND)
    {
        // Print command arguments
        printf("COMMAND:");
        char **args = node->args;
        while (args && *args)
            printf(" %s", *args++);
        
        // Print redirections
        t_redir *r = node->redirs;
        while (r)
        {
            printf(" [");
            switch(r->type)
            {
                case TOKEN_REDIR_IN:   printf("INPUT"); break;
                case TOKEN_REDIR_OUT:  printf("OUTPUT"); break;
                case TOKEN_APPEND:     printf("APPEND"); break;
                case TOKEN_HEREDOC:    printf("HEREDOC"); break;
                default:               printf("UNKNOWN_REDIR");
            }
            printf(" -> %s]", r->file);
            r = r->next;
        }
        printf("\n");
    }
    else if (node->e_type == AST_PIPE)
    {
        printf("PIPE:\n");
        print_ast(node->left, level + 1);
        print_ast(node->right, level + 1);
    }
    else
    {
        printf("UNKNOWN NODE TYPE %d\n", node->e_type);
    }
}

