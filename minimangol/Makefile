NAME = minishell

GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
RESET = \033[0m

SOURCES = \
	parsing/tokenize/lexer.c parsing/error/error.c \
	parsing/tree/tree_wdakxi.c parsing/tokenize/main.c \

OBJECTS = $(SOURCES:.c=.o)

CC = gcc

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a

READLINE_INCLUDE = -I/usr/include/readline
READLINE_LIB = -lreadline

all: $(NAME)

$(LIBFT):
	@echo "$(BLUE)Building libft library...$(RESET)"
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJECTS)
	@echo "$(GREEN)Linking executable...$(RESET)"
	@$(CC) -o $(NAME) $(OBJECTS) -L$(LIBFT_DIR) -lft $(READLINE_LIB)
	@echo "$(GREEN)✓ $(NAME) successfully built!$(RESET)"

%.o: %.c minishell.h
	@printf "$(YELLOW)▶ Compiling... [%s]\r$(RESET)" $
	@$(CC) -I$(LIBFT_DIR) $(READLINE_INCLUDE) -c $< -o $@
	@printf "$(GREEN)✓ Compiled   $(RESET)\n"

clean:
	@echo "$(BLUE)Cleaning project files...$(RESET)"
	@rm -f $(OBJECTS)
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "$(BLUE)Removing executable...$(RESET)"
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re