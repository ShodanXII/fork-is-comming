NAME = minishell

SOURCES = \
	parsing/tokenize/token.c \
	parsing/error/error.c \
	parsing/tokenize/santax_error.c

OBJECTS = $(SOURCES:.c=.o)

CC = cc
CFLAGS = -Wall -I$(BREW_PREFIX)/opt/readline/include
LDFLAGS = -L$(BREW_PREFIX)/opt/readline/lib
LIBS = -lreadline
LIBFT = libft/libft.a

BREW_PREFIX := $(shell brew --prefix)

all: $(NAME)

$(NAME): $(OBJECTS) $(LIBFT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME)

re: fclean all
