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

typedef struct s_needed t_needed;
typedef struct s_stack t_stack;
typedef struct s_ast t_ast;
typedef struct s_command t_command;
typedef struct s_redir t_redir;
typedef struct s_token t_token;
typedef struct s_parser t_parser;
typedef struct s_stack_cmd t_stack_cmd;

struct s_token
{
	t_token_type    type;
	struct s_token  *next;
};

struct s_stack_cmd
{
	t_ast *node;
	struct s_stack *next;
};

struct s_parser
{
	t_token	*current;
	t_stack	*stack;
	t_stack_cmd	*stack_cmd;
};

struct s_stack
{
	t_ast *node;
	struct s_stack *next;
};

struct s_redir
{
	int	type; // Type of redirection
	char		*file; // Filename for redirection
	struct s_redir	*next; // Next redirection
};

struct s_command
{
	char	**args; // Command and arguments
	t_redir		*redirs;
	int		arg_count;
};

struct s_needed
{
	int between_parens;
};

struct s_ast
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
	t_stack		*stack;
	int           is_subshell;
	t_redir		*redirs;
	struct s_ast *left;
	struct s_ast *right;
	struct s_ast *node;
} ;

// Tokenizer
t_token     *tokenize(char *input);
// void        free_tokens(t_token *tokens);
void        print_tokens(t_token *tokens); // For debug

// Parser
void print_tokens(t_token *tokens);
int check_syntax_errors(t_token *tokens);
// void        free_ast(t_ast *node);
t_ast		*pop(t_stack **stack);
void		push(t_stack **stack, t_token *token);
void		free_stack(t_stack **stack);
t_ast		*peek(t_stack *stack);
// Syntax Error Handling
// int         has_syntax_error(t_token *tokens);
// void        print_syntax_error(t_token *token);

// Utils (if needed)
char        *ft_strdup(const char *s1);
t_ast *parse(t_parser *parser);
void print_ast(t_ast *node, int level);

#endif
