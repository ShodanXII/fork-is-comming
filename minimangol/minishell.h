#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <readline/readline.h>
# include "libft/libft.h"
# include <readline/history.h>
# include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

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
	int			is_wait;
	pid_t		pid;
	int           arg_count;
	int           *ar_pipe;
	t_redir		*redirs;
	struct s_ast *left;
	struct s_ast *right;
	struct s_ast *node;
} ;

void        print_tokens(t_token *tokens); // For debug
void	ana_m9wd(t_ast *node);
void print_tokens(t_token *tokens);
int check_syntax_errors(t_token *tokens);
t_ast *function_lmli7a(t_token *tokens, t_token *fin_t7bs);
int	execute_tree(t_ast *node, int fd, int outfd, int cs, char **env);
t_redir *handle_redir(t_token **tokens);
void	error(char *str);
t_token *lexer(char *input);
t_token *merge_consecutive_words(t_token *tokens);
char        *ft_strdup(const char *s1);
int get_precedence(int token_type);
int is_operator(t_token *token);

void print_redirs(t_redir *redirs);
void free_redirs(t_redir *redirs);
t_redir *extract_redirs_from_command(char **args);

#endif
