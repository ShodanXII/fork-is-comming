#include "../../minishell.h"

int main(int ac, char **av, char **env)
{
    int i = 0;
    char *input;
    char *args;
    extern char **environ;
    t_ast *head = NULL;
    t_ast *ast = NULL;
    bool flag = 1;
    // while(flag)
    // {
        // input = readline("minishell> ");
        // if (input == NULL)
        //     // break;
        // if (ft_strlen(input) == 0)
        // {
        //     free(input);
        //     // continue;
        // }
        // add_history(input);

        t_token *tokens = lexer("ls -la");
        // printf("gg\n");

    //     if (tokens == NULL)
    //     {
    //         free(input);
    //   //      continue;
    //     }
    //     if (args == NULL)
    //     {
    //         free(input);
    //         // free_tokens(tokens);
    //         // continue;
    //     }
        // head = function_lmli7a(tokens, NULL);
        // printf("------>>>>>>>> %s: \n", head->left->args[0]);
            // free_tokens(tokens);
            // continue;
        execute_tree(head, 0, 1, -1, environ);
        printf("exited \n");
        // free(input);
        // free_tokens(tokens);
    // }

}