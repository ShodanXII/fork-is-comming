#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <stdbool.h>
# include "libft/libft.h"

typedef enum e_token_type
{
    TOKEN_WORD,
    TOKEN_REDIR_IN,   // <
    TOKEN_REDIR_OUT,  // >
    TOKEN_APPEND,     // >>
    TOKEN_OR,
    TOKEN_AND,
    TOKEN_HEREDOC,    // <<
    TOKEN_PIPE,
} t_token_type;

typedef struct s_redir t_redir;
typedef struct s_token t_token;
typedef struct s_ast t_ast;

struct s_token
{
    char            *value;
    t_token_type    type;
    struct s_token  *next;
};

struct s_redir
{
    t_token_type    type;
    char            *file;
    struct s_redir  *next;
};

struct s_ast
{
    t_token_type  e_token_type;
    int           e_precedence;
    char          *cmd;
    char          **args;
    int           arg_count;
    int           *ar_pipe;
    t_redir       *redirs;
    pid_t         pid;
    int           is_wait;
    struct s_ast  *left;
    struct s_ast  *right;
    struct s_ast  *node;
};

/* Tokenization functions */
t_token     *lexer(char *input);
t_token     *merge_consecutive_words(t_token *tokens);
void        print_tokens(t_token *tokens);
void        free_tokens(t_token *tokens);

/* AST functions */
t_ast       *create_ast_node(t_token_type type);
t_ast       *build_ast(t_token *tokens);
void        free_ast(t_ast *ast);
void        add_arg_to_command(t_ast *cmd_node, char *arg);
t_ast       *parse_simple_command(t_token **tokens);
t_ast       *build_pipeline_tree(t_token **tokens);

/* Redirection functions */
/* Redirections */
t_redir     *handle_redir(t_token **tokens);
t_redir     *create_redir(t_token_type type, char *file);
void        add_redir(t_redir **list, t_redir *new_redir);

/* Tokenization */

/* Syntax checking */
int         check_syntax_errors(t_token *tokens);

/* Execution functions */
void        handle_redirection(t_ast *node, int *infd, int *outfd);
t_redir *handle_redir(t_token **tokens);
char        *debug_okda(char **env, char *cmd);
int         execute_tree(t_ast *node, int fd, int outfd, int cs, char **env);
int         execute_command(t_ast *node, int infd, int outfd, int cs, char **env);

/* Helper functions */
void        ana_m9wd(t_ast *node);
int         check_syntax_errors(t_token *tokens);
void        error(char *str);
void        print_redirs(t_redir *redirs);

#endif