NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -lreadline
INCLUDE = inc
BUILDDIR = build

LIBFT = ft_split.c \
	ft_strcmp.c \
	ft_strncmp.c \
	ft_strjoin.c \
	ft_strlen.c \
	ft_strdup.c \
	ft_strstr.c \
	ft_strncpy.c \
	ft_strcpy.c \
	ft_isalnum.c \
	ft_memmove.c \

SRC = $(LIBFT) \
	main.c

SRC := $(addprefix src/,$(SRC))
OBJS = $(SRC:src/%.c=$(BUILDDIR)/%.o)

all: $(BUILDDIR) $(NAME)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -I$(INCLUDE) -o $(NAME)

clean:
	rm -rf $(BUILDDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
