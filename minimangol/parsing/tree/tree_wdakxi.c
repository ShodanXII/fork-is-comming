#include "../../minishell.h"

static int get_token_precedence(t_token *token);
int is_operator(t_token *token);
// const char *get_token_type_name(t_token_type type)
// {
// 	switch (type)
// 	{
// 		case TOKEN_WORD:       return "WORD";
// 		case TOKEN_RPAREN:     return "RPAREN";
// 		case TOKEN_REDIR_IN:   return "REDIR_IN";
// 		case TOKEN_REDIR_OUT:  return "REDIR_OUT";
// 		case TOKEN_APPEND:     return "APPEND";
// 		case TOKEN_HEREDOC:    return "HEREDOC";
// 		case TOKEN_LPAREN:     return "LPAREN";
// 		case TOKEN_PIPE:       return "PIPE";
// 		case TOKEN_OR:         return "OR";
// 		case TOKEN_AND:        return "AND";
// 		default:               return "UNKNOWN";
// 	}
// }

static void free_ast(t_ast *node)
{
	if (node == NULL)
		return;
	free_ast(node->left);
	free_ast(node->right);
	free(node->cmd);
	
	// Free the args array
	if (node->args)
	{
		int i = 0;
		while (node->args[i])
		{
			free(node->args[i]);
			i++;
		}
		free(node->args);
	}
	
	free(node);
}



int get_precedence(int token_type)
{
	if (token_type == TOKEN_REDIR_IN || token_type == TOKEN_REDIR_OUT || 
		token_type == TOKEN_APPEND || token_type == TOKEN_HEREDOC)
		return 3;  // Highest precedence
	else if (token_type == TOKEN_PIPE)
		return 2;
	else if (token_type == TOKEN_AND)
		return 1;
	else if (token_type == TOKEN_OR)
		return 0;
	return -1;
}

t_ast *function_lmli7a(t_token *tokens, t_token *fin_t7bs)
{
	if (!tokens)
		return NULL;

	t_token *current = tokens;
	t_token *highest_ptr = current;
	t_ast *head = NULL;
	while (current && current != fin_t7bs)
	{
		if (get_token_precedence(current) > get_token_precedence(highest_ptr))
			highest_ptr = current;
		current = current->next;
	}
	if (!highest_ptr)
		return NULL;
	head = malloc(sizeof(t_ast));
	if (!head)
		return NULL;
	head->e_token_type = highest_ptr->type;
	head->cmd = NULL;
	head->args = NULL;
	head->redirs = NULL;
	head->left = NULL;
	head->right = NULL;
	head->is_wait = 0;
	head->ar_pipe = NULL;
	if (highest_ptr->type == TOKEN_WORD)
	{
		head->args = ft_split(highest_ptr->value, ' ');
		if (!head->args)
		{
			free(head);
			return NULL;
		}		
		head->cmd = ft_strdup(head->args[0]);
		if (!head->cmd)
		{
			free(head->args);
			free(head);
			return NULL;
		}
		head->redirs = handle_redir(&tokens);
	}
	else
	{
		head->cmd = ft_strdup(highest_ptr->value);
		if (!head->cmd)
		{
			free(head);
			return NULL;
		}
		head->left = function_lmli7a(tokens, highest_ptr);
		head->right = function_lmli7a(highest_ptr->next, fin_t7bs);
	}
	return head;
}


int is_operator(t_token *token)
{
	if (!token)
		return 0;
	if (token->type == TOKEN_AND || token->type == TOKEN_OR
		|| token->type == TOKEN_PIPE || token->type == TOKEN_REDIR_IN
		|| token->type == TOKEN_REDIR_OUT || token->type == TOKEN_APPEND
		|| token->type == TOKEN_HEREDOC)
		return 1;
	return 0;
}

static int get_token_precedence(t_token *token)
{
	if (!token)
		return -1;
	if (token->type == TOKEN_AND || token->type == TOKEN_OR)
		return 4;
	if (token->type == TOKEN_PIPE)
		return 3;
	if (token->type == TOKEN_WORD)
		return 2; 
	if (token->type == TOKEN_REDIR_IN || 
		token->type == TOKEN_REDIR_OUT ||
		token->type == TOKEN_APPEND || 
		token->type == TOKEN_HEREDOC)
		return 1;
	return 0;
}

// int	ft_strcmp(char *str1, char *str2)
// {
// 	int	i;

// 	i = 0;
// 	while (str1[i] || str2[i])
// 	{
// 		if (str1[i] > str2[i])
// 			return (1);
// 		else if (str1[i] < str2[i])
// 			return (-1);
// 		i++;
// 	}
// 	return (0);
// }

// void	open_herdoc(t_redirection *node)
// {
// 	int		new_in_fd;
// 	char	*line;

// 	new_in_fd = open("here_doc", O_CREAT | O_WRONLY | O_TRUNC, 0644);
// 	// if (new_in_fd == -1)
// 	// 	t_error("pipex: input", data, 1);
// 	line = get_next_line(0);
// 	line[ft_strlen(line) - 1] = 0;
// 	while (ft_strcmp(node->file, line))
// 	{
// 		line[ft_strlen(line)] = '\n';
// 		write(new_in_fd, line, ft_strlen(line));
// 		free(line);
// 		line = get_next_line(0);
// 		line[ft_strlen(line) - 1] = 0;
// 	}
// 	free(line);
// 	close(new_in_fd);
// }

void	handle_redirection(t_ast *node, int *infd, int *outfd)
{
	t_redir	*lst;

	lst = node->redirs;
	while (lst)
	{
		if (lst->type == TOKEN_REDIR_IN)
		{
			*infd = open(lst->file, O_RDONLY);
			if (*infd == -1)
			{
				perror("open");
				return ;
			}
		}
		else if (lst->type == TOKEN_APPEND || lst->type == TOKEN_REDIR_OUT)
		{
			if (lst->type == TOKEN_APPEND)
				*outfd = open(lst->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
			else
				*outfd = open(lst->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			if (*outfd == -1)
			{
				perror("open");
				return ;
			}
		}
		// else if (lst->type == TOKEN_HEREDOC)
		// {
		// 	open_herdoc(lst);
		// 	*infd = open("here_doc", O_RDONLY);
		// 	if (*outfd == -1)
		// 	{
		// 		perror("open");
		// 		return ;
		// 	}
		// }
		lst = lst->next;
	}
}

// int my_execve(t_ast *node)
// {
// 	if (!ft_strcmp(node->cmd_args[0], "echo"))
// 		return (ft_echo(node->cmd_args));
// 	else if (!ft_strcmp(node->cmd_args[0], "cd"))
// 		return (ft_cd(node->env_var, node->cmd_args[1]));
// 	else if (!ft_strcmp(node->cmd_args[0], "pwd"))
// 		return (ft_pwd());
// 	else if (!ft_strcmp(node->cmd_args[0], "export"))
// 		return (ft_export(node->env_var, node->cmd_args[1]));
// 	else if (!ft_strcmp(node->cmd_args[0], "unset"))
// 		return (ft_unset(node->env_var, node->cmd_args));
// 	else if (!ft_strcmp(node->cmd_args[0], "env"))
// 		return (ft_env(*(node->env_var)));
// 	// else if (!ft_strcmp(node->cmd_args[0], "exit")) to do
// 	// 	return (1);
// }

// int execute_builtin(t_ast *node, int infd, int outfd)
// {
// 	handle_redirection(node, &infd, &outfd);
// 	if (infd)
// 	{
// 		dup2(infd, 0);
// 		close(infd);
// 	}
// 	if (outfd != 1)
// 	{
// 		dup2(outfd, 1);
// 		close(outfd);
// 	}
// 	my_execve(node);
// }

// int	is_builtin(char *cmd)
// {
// 	if (!ft_strcmp(cmd, "echo"))
// 		return (1);
// 	else if (!ft_strcmp(cmd, "cd"))
// 		return (1);
// 	else if (!ft_strcmp(cmd, "pwd"))
// 		return (1);
// 	else if (!ft_strcmp(cmd, "export"))
// 		return (1);
// 	else if (!ft_strcmp(cmd, "unset"))
// 		return (1);
// 	else if (!ft_strcmp(cmd, "env"))
// 		return (1);
// 	else if (!ft_strcmp(cmd, "exit"))
// 		return (1);
// 	return (0);
// }

char	*helper_path_cmd(char **commands_path, char *cmd)
{
	int		i;
	char	*command_path;

	i = 0;
	while (commands_path[i])
	{
		command_path = ft_strjoin(commands_path[i], cmd);
		if (!command_path)
		{
			// free_args(commands_path);
			free(cmd);
			return (NULL);
		}
		if (!access(command_path, X_OK))
		{
			// free_args(commands_path);
			free(cmd);
			return (command_path);
		}
		free(command_path);
		i++;
	}
	// free_args(commands_path);
	free(cmd);
	return (NULL);
}

char	*helper_path(char *str, char *cmd, int size)
{
	char	*complete_path;
	char	**commands_path;

	complete_path = ft_substr(str, size, ft_strlen(str) - size);
	if (!complete_path)
		return (NULL);
	commands_path = ft_split(complete_path, ':');
	if (!commands_path)
	{
		free(complete_path);
		return (NULL);
	}
	free(complete_path);
	return (helper_path_cmd(commands_path, cmd));
}

char	*debug_okda(char **env, char *cmd)
{
	char	*str;
	char	*cmdr;
	int		i;

	if (!cmd)
		return (NULL);
	if (!access(cmd, X_OK))
		return (ft_strdup(cmd));
	cmdr = ft_strjoin("/", cmd);
	if (!cmdr)
		return (NULL);
	i = 0;
	while (env[i])
	{
		str = ft_strnstr(env[i], "PATH=", ft_strlen("PATH="));
		if (str)
			return (helper_path(str, cmdr, ft_strlen("PATH=")));
		i++;
	}
	return (NULL);
}

int execute_command(t_ast *node, int infd, int outfd, int cs, char **env)
{
	int status;
	
	node->pid = fork();
	if (!node->pid)
	{
		close(cs);

		handle_redirection(node, &infd, &outfd);
		if (infd)
		{
			dup2(infd, 0);
			close(infd);
		}
		if (outfd != 1)
		{
			dup2(outfd, 1);
			close(outfd);
		}
		
		// Use the command path
		char *d = debug_okda(env, node->cmd);
		if (!d)
		{
			fprintf(stderr, "Command not found: %s\n", node->cmd);
			exit(127);
		}        
		execve(d, node->args, env);
		exit(1);
	}    
	if (node->is_wait == 1)
	{
		if (node->ar_pipe)
		{
			close(node->ar_pipe[0]);
			close(node->ar_pipe[1]);
			free(node->ar_pipe);
			node->ar_pipe = NULL;
		}
		waitpid(node->pid, &status, 0);
		return (WEXITSTATUS(status));
	}
	return (0);
}

void	ana_m9wd(t_ast *node)
{
	if (!node)
		return ;
	//ma kan7tajxe parting m9wd parting ki7tajni
	if (!node->right)
	{
		node->is_wait = 1;
		return ;
	}
	ana_m9wd(node->right);
}

int execute_tree(t_ast *node, int fd, int outfd, int cs, char **env)
{
	int status = 1;

	if (!node)
		return (1);
	if (node->e_token_type == TOKEN_PIPE)
	{
		if (!node->right)// Check
			return status;
		node->right->ar_pipe = malloc(2 * sizeof(int));
		// TO DO
		pipe(node->right->ar_pipe);
		execute_tree(node->left, fd, node->right->ar_pipe[1], node->right->ar_pipe[0], env);
		status = execute_tree(node->right, node->right->ar_pipe[0], outfd, node->right->ar_pipe[1], env);
		if (node->right->ar_pipe)
		{
			close(node->right->ar_pipe[0]);
			close(node->right->ar_pipe[1]);
			free(node->right->ar_pipe);
			node->right->ar_pipe = NULL;
		}
	}
	else if (node->e_token_type == TOKEN_AND)
	{
		ana_m9wd(node->left);
		status = execute_tree(node->left, fd, outfd, cs, env);
		if (!status)
			status = execute_tree(node->right, fd, outfd, cs, env);
	}
	else if (node->e_token_type == TOKEN_OR)
	{
		ana_m9wd(node->left);
		status = execute_tree(node->left, fd, outfd, cs, env);
		if (status)
			status = execute_tree(node->right, fd, outfd, cs, env);
		printf("1337\n");
	}
	else if (node->e_token_type == TOKEN_WORD)
		return (execute_command(node, fd, outfd, cs, env));
	return (status);
}

// void init_data(t_ast *ast, int flag)
// {
//     if (!ast)
//         return ;
//     if (ast->type == AST_PIPE)
//         flag = 1;
//     if (flag && ast->type == AST_CMD)
//     {
//         ast->is_wait = 1;
//         flag = 0;
//     }
//     init_data(ast->left, flag);
//     init_data(ast->right, 0);
// }

// int waiting(t_ast *ast)
// {
// 	int	status;
	
// 	while (wait(NULL) > 0);
// 	// if (!ast->right)
// 	// {
// 	// 	// waitpid(ast->pid, &status, 0);
// 	// 	while (wait(NULL) > 0);
// 	// 	return (0);
// 	// 	// return (WEXITSTATUS(status));
// 	// }
// 	// waiting(ast->right);
// 	// return (0);
// 	// wait(NULL);
// 	// if (!ast)
// 	//     return ;
// 	// if(ast->next)
// 	// {    if (ast->type == AST_CMD && ast->next->token->type == TOKEN_PIPE)
// 	//     	wait(NULL);
// 	// }
// 	// // wait(NULL);
// 	// waiting(ast->left, counter);
// 	// waiting(ast->right, counter);
// }

// int main()
// {
//     extern char **environ;  // Use system environment
//     t_ast *ast;
//     t_token *tokens = tokenize_compat("ls -la | grep minishell.h");
// 	// printf("tokens %s: \n", tokens->value);
//     if (!tokens)
//         return 1;
// 	// printf("tokens %s: \n", tokens->value);
//     t_ast *head = function_lmli7a(tokens, NULL);
// 	// printf("------>>>>>>>> %s: \n", head->left->cmd); 
//     //   // Uncomment for debugging
//     execute_tree(head, 0, 1, -1, environ);
//     free_ast(head);
//     // Free tokens here
//     return 0;
// }


// && || | cmd redirection

//           &&
//          /  \
//         /    \
//        /      \
//       /        \
//      &&         echo hi
//     /   \
//    /     \
// ls -l    |
//          / \
//         /   \
//      cat j   la