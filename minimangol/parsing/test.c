#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Token types
typedef enum {
    TOKEN_COMMAND = 0,
    TOKEN_PIPE = 1,        // |
    TOKEN_AND = 2,         // &&
    TOKEN_OR = 3,          // ||
    TOKEN_REDIRECT_IN = 4, // <
    TOKEN_REDIRECT_OUT = 5 // >
} TokenType;

// Token structure
typedef struct Token {
    TokenType type;
    char *value;
    struct Token *next;
} Token;

// Abstract Syntax Tree node
typedef struct ASTNode {
    TokenType type;
    char **args;           // For commands, store command and arguments
    int arg_count;
    struct ASTNode *left;  // Left child
    struct ASTNode *right; // Right child
} ASTNode;

// Stack for operators during parsing
typedef struct OpStack {
    ASTNode **items;
    int capacity;
    int top;
} OpStack;

// Queue for output during shunting yard algorithm
typedef struct OutputQueue {
    ASTNode **items;
    int capacity;
    int front;
    int rear;
    int size;
} OutputQueue;

// Initialize stack
OpStack *create_stack(int capacity) {
    OpStack *stack = (OpStack *)malloc(sizeof(OpStack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->items = (ASTNode **)malloc(capacity * sizeof(ASTNode *));
    return stack;
}

// Push to stack
void stack_push(OpStack *stack, ASTNode *node) {
    if (stack->top == stack->capacity - 1) {
        // Resize if needed
        stack->capacity *= 2;
        stack->items = (ASTNode **)realloc(stack->items, stack->capacity * sizeof(ASTNode *));
    }
    stack->items[++stack->top] = node;
}

// Pop from stack
ASTNode *stack_pop(OpStack *stack) {
    if (stack->top == -1) return NULL;
    return stack->items[stack->top--];
}

// Peek at stack top
ASTNode *stack_peek(OpStack *stack) {
    if (stack->top == -1) return NULL;
    return stack->items[stack->top];
}

// Check if stack is empty
bool stack_is_empty(OpStack *stack) {
    return stack->top == -1;
}

// Free stack
void free_stack(OpStack *stack) {
    free(stack->items);
    free(stack);
}

// Initialize queue
OutputQueue *create_queue(int capacity) {
    OutputQueue *queue = (OutputQueue *)malloc(sizeof(OutputQueue));
    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->items = (ASTNode **)malloc(capacity * sizeof(ASTNode *));
    return queue;
}

// Enqueue
void enqueue(OutputQueue *queue, ASTNode *node) {
    if (queue->size == queue->capacity) {
        // Resize if needed
        queue->capacity *= 2;
        queue->items = (ASTNode **)realloc(queue->items, queue->capacity * sizeof(ASTNode *));
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->items[queue->rear] = node;
    queue->size++;
}

// Dequeue
ASTNode *dequeue(OutputQueue *queue) {
    if (queue->size == 0) return NULL;
    ASTNode *node = queue->items[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return node;
}

// Free queue
void free_queue(OutputQueue *queue) {
    free(queue->items);
    free(queue);
}

// Create a new AST node
ASTNode *create_ast_node(TokenType type) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    node->args = NULL;
    node->arg_count = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Create a command node with arguments
ASTNode *create_command_node(char **args, int arg_count) {
    ASTNode *node = create_ast_node(TOKEN_COMMAND);
    node->args = args;
    node->arg_count = arg_count;
    return node;
}

// Create an operator node
ASTNode *create_operator_node(TokenType type, ASTNode *left, ASTNode *right) {
    ASTNode *node = create_ast_node(type);
    node->left = left;
    node->right = right;
    return node;
}

// Get operator precedence
int get_precedence(TokenType type) {
    switch (type) {
        case TOKEN_PIPE: return 3;
        case TOKEN_AND: return 2;
        case TOKEN_OR: return 1;
        default: return 0;
    }
}
void print_ast_horizontal(ASTNode *node, int depth) {
    if (!node) return;

    // Print right child first
    print_ast_horizontal(node->right, depth + 1);

    // Print current node
    for (int i = 0; i < depth; i++) {
        printf("        "); // 8 spaces per depth level
    }

    switch (node->type) {
        case TOKEN_COMMAND:
            printf("CMD: ");
            for (int i = 0; i < node->arg_count; i++) {
                printf("%s ", node->args[i]);
            }
            printf("\n");
            break;
        case TOKEN_PIPE:
            printf("PIPE |\n");
            break;
        case TOKEN_AND:
            printf("AND &&\n");
            break;
        case TOKEN_OR:
            printf("OR ||\n");
            break;
        case TOKEN_REDIRECT_IN:
            printf("REDIR <\n");
            break;
        case TOKEN_REDIRECT_OUT:
            printf("REDIR >\n");
            break;
        default:
            printf("UNKNOWN\n");
    }

    // Print left child
    print_ast_horizontal(node->left, depth + 1);
}

// Convert token list to AST using Shunting Yard algorithm
ASTNode *build_ast_from_tokens(Token *token_list) {
    if (!token_list) return NULL;
    
    OpStack *operator_stack = create_stack(10);
    OutputQueue *output_queue = create_queue(10);
    
    // Current command and arguments
    char **current_args = NULL;
    int arg_count = 0;
    
    Token *current = token_list;
    
    // First pass: group commands with their arguments
    while (current) {
        if (current->type == TOKEN_COMMAND) {
            // Add argument to current command
            arg_count++;
            current_args = (char **)realloc(current_args, arg_count * sizeof(char *));
            current_args[arg_count - 1] = strdup(current->value);
        } else {
            // Operator encountered, create command node from collected arguments
            if (arg_count > 0) {
                ASTNode *cmd_node = create_command_node(current_args, arg_count);
                enqueue(output_queue, cmd_node);
                
                // Reset for next command
                current_args = NULL;
                arg_count = 0;
            }
            
            // Process the operator using shunting yard
            TokenType current_op = current->type;
            int current_precedence = get_precedence(current_op);
            
            while (!stack_is_empty(operator_stack) && 
                   get_precedence(stack_peek(operator_stack)->type) >= current_precedence) {
                // Pop operators with higher precedence and process them
                ASTNode *op = stack_pop(operator_stack);
                
                // Create operator node with right and left operands from queue
                if (output_queue->size >= 2) {
                    ASTNode *right = dequeue(output_queue);
                    ASTNode *left = dequeue(output_queue);
                    op->right = right;
                    op->left = left;
                    enqueue(output_queue, op);
                } else {
                    // Syntax error: not enough operands
                    printf("Error: syntax error - not enough operands for operator\n");
                    free_stack(operator_stack);
                    free_queue(output_queue);
                    return NULL;
                }
            }
            
            // Push current operator to stack
            ASTNode *op_node = create_ast_node(current_op);
            stack_push(operator_stack, op_node);
        }
        
        current = current->next;
    }
    
    // Handle any remaining command arguments
    if (arg_count > 0) {
        ASTNode *cmd_node = create_command_node(current_args, arg_count);
        enqueue(output_queue, cmd_node);
    }
    
    // Process remaining operators on the stack
    while (!stack_is_empty(operator_stack)) {
        ASTNode *op = stack_pop(operator_stack);
        
        if (output_queue->size >= 2) {
            ASTNode *right = dequeue(output_queue);
            ASTNode *left = dequeue(output_queue);
            op->right = right;
            op->left = left;
            enqueue(output_queue, op);
        } else {
            // Syntax error: not enough operands
            printf("Error: syntax error - not enough operands for operator\n");
            free_stack(operator_stack);
            free_queue(output_queue);
            return NULL;
        }
    }
    
    // Final tree should have only one node in the queue
    ASTNode *root = NULL;
    if (output_queue->size == 1) {
        root = dequeue(output_queue);
    } else if (output_queue->size > 1) {
        printf("Error: syntax error - too many commands without operators\n");
    }
    
    free_stack(operator_stack);
    free_queue(output_queue);
    
    return root;
}

// Function to tokenize input string (simplified for example)
Token *tokenize(const char *input) {
    // This is a simplified tokenizer for demonstration
    // A real implementation would parse the input string properly
    char *input_copy = strdup(input);
    char *token_str = strtok(input_copy, " ");
    
    Token *head = NULL;
    Token *current = NULL;
    
    while (token_str) {
        Token *token = (Token *)malloc(sizeof(Token));
        
        if (strcmp(token_str, "|") == 0) {
            token->type = TOKEN_PIPE;
        } else if (strcmp(token_str, "&&") == 0) {
            token->type = TOKEN_AND;
        } else if (strcmp(token_str, "||") == 0) {
            token->type = TOKEN_OR;
        } else {
            token->type = TOKEN_COMMAND;
        }
        
        token->value = strdup(token_str);
        token->next = NULL;
        
        if (!head) {
            head = token;
        } else {
            current->next = token;
        }
        
        current = token;
        token_str = strtok(NULL, " ");
    }
    
    free(input_copy);
    return head;
}

// Function to print the AST (for debugging)
void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case TOKEN_COMMAND:
            printf("COMMAND: ");
            for (int i = 0; i < node->arg_count; i++) {
                printf("%s ", node->args[i]);
            }
            printf("\n");
            break;
        case TOKEN_PIPE:
            printf("PIPE\n");
            break;
        case TOKEN_AND:
            printf("AND\n");
            break;
        case TOKEN_OR:
            printf("OR\n");
            break;
        default:
            printf("UNKNOWN\n");
    }
    
    print_ast(node->left, indent + 1);
    print_ast(node->right, indent + 1);
}

// Free memory allocated for AST
void free_ast(ASTNode *node) {
    if (!node) return;
    
    free_ast(node->left);
    free_ast(node->right);
    
    if (node->type == TOKEN_COMMAND) {
        for (int i = 0; i < node->arg_count; i++) {
            free(node->args[i]);
        }
        free(node->args);
    }
    
    free(node);
}

// Free memory allocated for token list
void free_tokens(Token *head) {
    Token *current = head;
    while (current) {
        Token *next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
}

// Test function for parser
void test_parser(const char *input) {
    printf("\nParsing: \"%s\"\n", input);
    Token *token_list = tokenize(input);
    
    // Print tokens
    printf("TOKENS:\n");
    Token *current = token_list;
    while (current) {
        printf("Type: %d, Value: %s\n", current->type, current->value);
        current = current->next;
    }
    
    // Build AST
    ASTNode *root = build_ast_from_tokens(token_list);
    
    if (root) {
        printf("AST (horizontal view):\n");
        print_ast_horizontal(root, 0);
        free_ast(root);
    } else {
        printf("Failed to build AST\n");
    }
    
    free_tokens(token_list);
}

int main() {
    // Simple command
    test_parser("ls -la");
    
    // Pipe
    test_parser("ls -la | grep file");
    
    // Logical operators
    test_parser("ls -la && echo success");
    test_parser("ls -la || echo failed");
    
    // Complex combinations
    test_parser("ls -la | grep file && echo success || echo failed");
    
    // Syntax errors
    test_parser("|"); // Error: pipe at beginning
    test_parser("ls |"); // Error: nothing after pipe
    test_parser("ls && || echo"); // Error: consecutive operators
    
    return 0;
}