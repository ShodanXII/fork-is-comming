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
 TOKEN_WORD ,
 TOKEN_RPAREN ,   // )
 TOKEN_REDIR_IN,   // <
 TOKEN_REDIR_OUT ,  // >
 TOKEN_APPEND ,   // >>
 TOKEN_HEREDOC ,   // <<
 TOKEN_LPAREN ,   // (
 TOKEN_PIPE ,
 TOKEN_OR ,
 TOKEN_AND ,
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
	char		*value;
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
	t_token_type  e_token_type;
	int           e_precedence;
	char         *cmd;
	char        **args;
	int           arg_count;
	int           fd[2];
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
t_ast *function_lmli7a(t_token *tokens, t_token *fin_t7bs);
void		free_stack(t_stack **stack);
t_ast		*peek(t_stack *stack);
// Syntax Error Handling
// int         has_syntax_error(t_token *tokens);
// void        print_syntax_error(t_token *token);

// Utils (if needed)
char        *ft_strdup(const char *s1);
t_ast *parse(t_parser *parser);
void compare(t_parser *stacks, t_token *tokens);
int get_precedence(int token_type);
void print_ast(t_ast *node, int level);
int is_operator(t_token *token);

#endif
