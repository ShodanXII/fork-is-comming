// #include "minishell.h"
#include <string.h>

// #include "../../includes/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <ctype.h>

typedef enum e_token_type
{
    WORD ,
    PIPE,
    AND,
    OR,
    REDIR_OUT,
    REDIR_APPEND,
    REDIR_IN,
    HEREDOC,
    LPAREN,
    RPAREN
}   t_token_type;

typedef struct s_ast_node {
    enum e_token_type  e_token_type;
    int                e_precedence;
    char              *cmd;
    char             **args;
    struct s_ast_node *left;
    struct s_ast_node *right;
    struct s_ast_node *next;
    int                fd[2];
} t_ast_node;

typedef struct s_stack {
    t_ast_node       *token;
    struct s_stack   *next;
} t_stack;

// Helper to skip whitespace
static void skip_whitespace(const char **str)
{
    while (**str && isspace(**str))
        (*str)++;
}

// Helper to create a new token node
static t_ast_node *new_token_node(t_token_type type, const char *value)
{
    t_ast_node *node = malloc(sizeof(t_ast_node));
    if (!node)
        return NULL;
    node->e_token_type = type;
    node->e_precedence = 0;
    node->cmd = value ? strdup(value) : NULL;
    node->args = NULL;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    node->fd[0] = -1;
    node->fd[1] = -1;
    return node;
}

// Get the next token from input string
static t_ast_node *get_token(const char **input)
{
    skip_whitespace(input);

    if (!**input)
        return NULL;

    const char *start = *input;

    // Operators
    if (**input == '|')
    {
        (*input)++;
        if (**input == '|') {
            (*input)++;
            return new_token_node(OR, "||");
        }
        return new_token_node(PIPE, "|");
    }
    else if (**input == '&')
    {
        (*input)++;
        if (**input == '&') {
            (*input)++;
            return new_token_node(AND, "&&");
        }
        return NULL; // Invalid single &
    }
    else if (**input == '>')
    {
        (*input)++;
        if (**input == '>') {
            (*input)++;
            return new_token_node(REDIR_APPEND, ">>");
        }
        return new_token_node(REDIR_OUT, ">");
    }
    else if (**input == '<')
    {
        (*input)++;
        if (**input == '<') {
            (*input)++;
            return new_token_node(HEREDOC, "<<");
        }
        return new_token_node(REDIR_IN, "<");
    }
    else if (**input == '(')
    {
        (*input)++;
        return new_token_node(LPAREN, "(");
    }
    else if (**input == ')')
    {
        (*input)++;
        return new_token_node(RPAREN, ")");
    }

    // Word (command or argument)
    while (**input && !isspace(**input) && !strchr("|&><()",(int)**input))
        (*input)++;

    size_t len = *input - start;
    char *word = strndup(start, len);
    if (!word)
        return NULL;

    return new_token_node(WORD, word);
}

// Main lexer function
t_ast_node *lexer(const char *input)
{
    t_ast_node *head = NULL;
    t_ast_node *tail = NULL;

    while (*input)
    {
        t_ast_node *token = get_token(&input);
        if (!token)
            continue;

        if (!head)
            head = token;
        else
            tail->next = token;

        tail = token;
    }

    return head;
}

// Helper functions for the algorithm
void push(t_stack **stack, t_ast_node *node)
{
    t_stack *new_node = malloc(sizeof(t_stack));
    if (!new_node)
        return;
    
    new_node->token = node;
    new_node->next = *stack;
    *stack = new_node;
}

t_ast_node *pop(t_stack **stack)
{
    if (!*stack)
        return NULL;
    
    t_stack *temp = *stack;
    t_ast_node *node = temp->token;
    *stack = temp->next;
    free(temp);
    
    return node;
}

t_ast_node *peek(t_stack *stack)
{
    if (!stack)
        return NULL;
    
    return stack->token;
}

bool is_empty(t_stack *stack)
{
    return stack == NULL;
}

// Function to add node to the output queue
void add_to_queue(t_ast_node **queue, t_ast_node *node)
{
    if (!*queue)
    {
        *queue = node;
        return;
    }

    t_ast_node *current = *queue;
    while (current->next)
        current = current->next;
    current->next = node;
}

// Function to pop node from the output queue
t_ast_node *pop_from_queue(t_ast_node **queue)
{
    if (!*queue)
        return NULL;
    
    t_ast_node *node = *queue;
    *queue = node->next;
    node->next = NULL;
    
    return node;
}

// Initialize a new AST node from a token
t_ast_node *init_node(t_ast_node *token, char **args)
{
    t_ast_node *node = malloc(sizeof(t_ast_node));
    if (!node)
        return NULL;
    
    node->e_token_type = token->e_token_type;
    node->e_precedence = token->e_precedence;
    if (token->cmd)
        node->cmd = strdup(token->cmd);
    else
        node->cmd = NULL;
    node->args = args;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    node->fd[0] = -1;
    node->fd[1] = -1;
    
    return node;
}

// Function to copy arguments (simplified version for test)
char **copy_args(t_ast_node **token)
{
    // In a real implementation, this would parse and copy the arguments
    // For testing, we'll just create a dummy array with the command
    char **args = malloc(sizeof(char *) * 2);
    if (!args)
        return NULL;
    
    args[0] = strdup((*token)->cmd);
    args[1] = NULL;
    
    return args;
}

// Function to build AST from postfix notation
t_ast_node *build_ast_from_postfix(t_ast_node **postfix_queue)
{
    t_stack *node_stack = NULL;
    t_ast_node *current;
    t_ast_node *root = NULL;

    // Create a copy of the queue to iterate through
    t_ast_node *queue_copy = *postfix_queue;
    *postfix_queue = NULL; // Clear the original queue
    
    while (queue_copy)
    {
        current = queue_copy;
        queue_copy = queue_copy->next;
        current->next = NULL;
        
        if (current->e_token_type == WORD)
        {
            push(&node_stack, current);
        }

		
        else if (current->e_token_type == PIPE || current->e_token_type == AND || 
                 current->e_token_type == OR || current->e_token_type == REDIR_OUT || 
                 current->e_token_type == REDIR_APPEND || current->e_token_type == REDIR_IN ||
                 current->e_token_type == HEREDOC)
        {
            // For binary operators
            if (is_empty(node_stack))
            {
                fprintf(stderr, "Error: Not enough operands for operator\n");
                free(current);
                continue;
            }
            
            t_ast_node *right = pop(&node_stack);
            
            if (is_empty(node_stack))
            {
                fprintf(stderr, "Error: Not enough operands for operator\n");
                push(&node_stack, right); // Put it back
                free(current);
                continue;
            }
            
            t_ast_node *left = pop(&node_stack);
            
            current->left = left;
            current->right = right;
            
            push(&node_stack, current);
        }
    }
    
    // The root of the AST should be the only node left on the stack
    if (!is_empty(node_stack))
    {
        root = pop(&node_stack);
        
        // Check for leftover nodes, which would indicate an error
        if (!is_empty(node_stack))
        {
            fprintf(stderr, "Warning: Extra nodes in stack, AST may be malformed\n");
            while (!is_empty(node_stack))
                free(pop(&node_stack));
        }
    }
    
    return root;
}

// Main AST building function
t_ast_node *create_ast_from_tokens(t_ast_node *tokens)
{
    t_stack *operator_stack = NULL;
    t_ast_node *output_queue = NULL;
    t_ast_node *current = tokens;
    t_ast_node *ast = NULL;
    
    // First pass: Convert infix to postfix using shunting yard algorithm
    while (current)
    {
        if (current->e_token_type == WORD)
        {
            t_ast_node *cmd_node = init_node(current, copy_args(&current));
            add_to_queue(&output_queue, cmd_node);
        }
        else if (current->e_token_type == PIPE || current->e_token_type == AND || 
                 current->e_token_type == OR || current->e_token_type == REDIR_OUT || 
                 current->e_token_type == REDIR_APPEND || current->e_token_type == REDIR_IN ||
                 current->e_token_type == HEREDOC)
        {
            while (!is_empty(operator_stack) && 
                   peek(operator_stack)->e_token_type != LPAREN &&
                   peek(operator_stack)->e_precedence <= current->e_precedence)
            {
                t_ast_node *op = pop(&operator_stack);
                add_to_queue(&output_queue, init_node(op, NULL));
            }
            
            push(&operator_stack, current);
        }
        else if (current->e_token_type == LPAREN)
        {
            push(&operator_stack, current);
        }
        else if (current->e_token_type == RPAREN)
        {
            while (!is_empty(operator_stack) && 
                   peek(operator_stack)->e_token_type != LPAREN)
            {
                t_ast_node *op = pop(&operator_stack);
                add_to_queue(&output_queue, init_node(op, NULL));
            }
            
            if (!is_empty(operator_stack) && peek(operator_stack)->e_token_type == LPAREN)
                pop(&operator_stack);
        }
        
        current = current->next;
    }
    
    // Pop any remaining operators to the output queue
    while (!is_empty(operator_stack))
    {
        t_ast_node *op = pop(&operator_stack);
        if (op->e_token_type == LPAREN || op->e_token_type == RPAREN)
        {
            fprintf(stderr, "Error: Mismatched parentheses\n");
            continue;
        }
        add_to_queue(&output_queue, init_node(op, NULL));
    }
    
    // Second pass: Build the AST from the postfix expression
    ast = build_ast_from_postfix(&output_queue);
    
    return ast;
}

void add_node(t_ast_node **head, t_ast_node *new_node)
{
    if (!*head)
    {
        *head = new_node;
        return;
    }
    
    t_ast_node *current = *head;
    while (current->next)
        current = current->next;
    
    current->next = new_node;
}

void free_ast(t_ast_node *node)
{
    if (!node)
        return;
    
    free_ast(node->left);
    free_ast(node->right);
    
    if (node->cmd)
        free(node->cmd);
    
    if (node->args)
    {
        for (int i = 0; node->args[i]; i++)
            free(node->args[i]);
        free(node->args);
    }
    
    free(node);
}




int main()
{
	
    t_ast_node *tokens = lexer("ls -la >> file | (cd || clear) && exit");
    if (tokens == NULL) {
        fprintf(stderr, "Error: No tokens to parse\n");
        return 1;
    }

    t_ast_node *ast = create_ast_from_tokens(tokens);
    if (ast == NULL) {
        fprintf(stderr, "Error: Failed to create AST\n");
        return 1;
    }

    printf("Abstract Syntax Tree:\n");
				printf("	R--}%s\n", ast->right->cmd);

	printf("[%s] \n", ast->cmd);

	printf("						R--}%s\n", ast->left->right->right->right->cmd);
	printf("				R--}%s\n", ast->left->right->right->cmd);
	printf("						L--}%s\n", ast->left->right->right->left->cmd);
	printf("		R--}%s\n", ast->left->right->cmd);
	printf("				R--}%s\n", ast->left->right->left->cmd);
				printf("	L--}%s\n", ast->left->cmd);
						printf("		L--}%s\n", ast->left->left->cmd);
    //print_ast(ast, 0);

    return 0;
}