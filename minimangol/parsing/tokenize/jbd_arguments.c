#include "../../minishell.h"

static char **split_arg(char *line)
{
	char **tokens;

	tokens = ft_split(line, ' ');
	if (!tokens)
	{
		perror("Error splitting mangolian");
		exit(EXIT_FAILURE);
	}
	return (tokens);
}

// t_command *command_structer(char **tokens)
// {
// 	int i = 0;
// 	int j = 0;
// 	t_command *cmd = NULL;

// 	if(!tokens)
// 		return (NULL);
// 	while (tokens[i])
// 	{
// 		j = 0;
// 		while (tokens[i][j])
// 		{
// 			if (tokens[i][j] == '|' || tokens[i][j] == '&' || tokens[i][j] == ';' || tokens[i][j] == '<' || tokens[i][j] == '>')
// 			{
// 				cmd = malloc(sizeof(t_command));
// 				if (!cmd)
// 					return (NULL);
// 				cmd->name = ft_strdup(tokens[i]);
// 				cmd->args = NULL;
// 				cmd->redirections = NULL;
// 				cmd->pipe_out = 0;
// 				cmd->next = NULL;
// 				return (cmd);
// 			}
// 			j++;
// 		}
// 	}
// }