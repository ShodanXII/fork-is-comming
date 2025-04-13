#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <readline/readline.h>
#include "libft/libft.h"
# include <readline/history.h>

typedef enum e_token_type
{
 TOKEN_WORD = 0,
 TOKEN_COMMAND ,
 TOKEN_PIPE ,
 TOKEN_AND ,
 TOKEN_OR ,
 TOKEN_REDIR_IN,   // <
 TOKEN_REDIR_OUT ,  // >
 TOKEN_APPEND ,   // >>
 TOKEN_HEREDOC ,   // <<
 TOKEN_LPAREN ,   // (
 TOKEN_RPAREN ,   // )
}   t_token_type;


typedef struct s_token
{
	t_token_type    type;
	char            *value;
	struct s_token  *next;
}   t_token;

typedef struct s_parser
{
	t_token	*current;
}   t_parser;


typedef struct s_redir
{
	int	type; // Type of redirection
	char		*file; // Filename for redirection
	struct s_redir	*next; // Next redirection
}	t_redir;

typedef struct s_command
{
	char	**args; // Command and arguments
	t_redir		*redirs;
	int		arg_count;
}	t_command;

typedef struct s_ast_node
{
	enum
	{
		AST_COMMAND,
		AST_PIPE,
		AST_AND,
		AST_OR,
		AST_SUBSHELL,
		AST_REDIR,
		AST_SQUOTE,
		AST_DQUOTE,
	}e_type ;
	t_token_type  e_token_type;
	int           e_precedence;
	char         *cmd;
	char        **args;
	int           arg_count;
	int           fd[2];
	int           is_subshell;
	t_redir		*redirs;
	struct s_ast_node *left;
	struct s_ast_node *right;
	struct s_ast_node *child;
} t_ast_node;

// Tokenizer
t_token     *tokenize(char *input);
// void        free_tokens(t_token *tokens);
void        print_tokens(t_token *tokens); // For debug

// Parser
void print_tokens(t_token *tokens);
int check_syntax_errors(t_token *tokens);
// void        free_ast(t_ast_node *node);

// Syntax Error Handling
int         has_syntax_error(t_token *tokens);
void        print_syntax_error(t_token *token);

// Utils (if needed)
char        *ft_strdup(const char *s1);
t_ast_node *parse(t_parser *parser);
void print_ast(t_ast_node *node, int level);

#endif
