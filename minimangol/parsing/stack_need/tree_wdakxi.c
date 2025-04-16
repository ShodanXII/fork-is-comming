#include "../../minishell.h"

void sub_tree(t_parser *stacks, t_token *tokens)
{
	// to do later requre understanding kifax ktkhdm shunting yard
}

t_parser *stack_checker(t_parser *stacks, t_token *tokens)
{
	if(!stacks || !tokens)
		return;
	t_stack *stack = stacks->stack;
	if(tokens->type < tokens->next->type)
		swap(&stack);
	return (stacks);
}

void function_lmli7a(t_parser *stacks, t_token *tokens)
{
	t_stack *stack = stacks->stack;
	t_stack *stack_cmd = stacks->stack_cmd;
	if(!tokens)
		return;
	while(tokens)
	{
		if(is_operator(tokens))
		{
			push(&stack, tokens);
			satck_checker(stacks, tokens);		
		}
		else if(tokens->type == TOKEN_LPAREN)
			sub_tree(stacks, tokens);
		else
			push(&stack_cmd, tokens);
		tokens = tokens->next;
	}
}

static int is_operator(t_token *token)
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
    if (token->type == TOKEN_WORD)
        return -1;
    if (token->type == TOKEN_AND || token->type == TOKEN_OR)
        return 1;
    if (token->type == TOKEN_PIPE)
        return 2;
    if (token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT
        || token->type == TOKEN_APPEND || token->type == TOKEN_HEREDOC)
        return 4;
    return 0;
}


void push(t_stack **stack, t_token *tokens)
{
	t_stack *new_node = malloc(sizeof(t_stack));
	new_node->node = tokens;
	new_node->next = *stack;
	*stack = new_node;
}

t_ast *pop(t_stack **stack)
{
    if (!*stack)
        return NULL;
    t_stack *top = *stack;
    t_ast *node = top->node;
    *stack = top->next;
    free(top);
    return node;
}

void free_stack(t_stack **stack)
{
	while (*stack)
	{
		t_stack *temp = *stack;
		*stack = (*stack)->next;
		free(temp);
	}
}

void swap(t_stack **stack)
{
	t_stack *first = *stack;
	t_stack *second = first->next;
	if (!first || !second)
		return;
	first->next = second->next;
	second->next = first;
	*stack = second;
}

t_ast *peek(t_stack *stack)
{
	if (stack == NULL)
		return;
	return stack->node;
}
t_ast *tree_core(t_token *postfix, t_parser *stacks)
{
	
}

// && || | cmd redirection