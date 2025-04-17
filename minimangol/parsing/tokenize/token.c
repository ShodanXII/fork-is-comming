#include "../../minishell.h"

// void prompt(bool running)
// {
// 	char *line;
// 	char *colored_prompt;
// 	char *temp_prompt;
// 	rl_catch_signals = 0;

// 	temp_prompt = ft_strjoin(COLOR_RED, "Minihell𒉭 > ");
// 	colored_prompt = ft_strjoin(temp_prompt, COLOR_RESET);
// 	free(temp_prompt);
// 	while (running)
// 	{
// 		line = readline(colored_prompt);
// 		char *processed_input = preprocess_input(line);
// 		if (line == NULL)
// 		{
// 			printf("exit\n");
// 			break; // Exit on Ctrl+D
// 		}
// 		free(line);
// 	}

// 	free(colored_prompt);
// }


// int main(int ac, char **av, char **env)
// {

// 	if (syntax_error(av))
// 	{
// 		printf("santax error\n");
// 		return 0;
// 	}
// 	setup_signals();
// 	prompt(true);
// }