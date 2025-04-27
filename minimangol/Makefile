NAME = minishell

# Source files with proper handling
SOURCES = \
	parsing/tokenize/lexer.c \
	parsing/tree/tree_wdakxi.c

OBJECTS = $(SOURCES:.c=.o)

CC = gcc

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a

# Handle readline dependency
READLINE_INCLUDE = -I/usr/include/readline
READLINE_LIB = -lreadline

all: $(NAME)

# Rule to ensure libft is compiled first
$(LIBFT):
	@echo "Building libft library..."
	@$(MAKE) -C $(LIBFT_DIR)

# Main rule to build the executable - link with explicit -L path
$(NAME): $(LIBFT) $(OBJECTS)
	@echo "Linking executable..."
	$(CC)  -o $(NAME) $(OBJECTS) -L$(LIBFT_DIR) -lft $(READLINE_LIB)

# Rule to compile individual object files - fix include path
%.o: %.c minishell.h
	@echo "Compiling $<..."
	$(CC)  -I$(LIBFT_DIR) $(READLINE_INCLUDE) -c $< -o $@

clean:
	@echo "Cleaning object files..."
	@rm -f $(OBJECTS)
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "Removing executable..."
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
