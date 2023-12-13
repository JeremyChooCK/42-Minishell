NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -Wshadow -g
LDFLAGS = -lreadline
INCLUDE = inc
BUILDDIR = build

LIBFT_SRC = ft_isalnum.c \
	ft_isdigit.c \
	ft_itoa.c \
	ft_atoi.c \
	ft_bzero.c \
	ft_putstr_fd.c \
	ft_realloc.c \
	ft_setenv.c \
	ft_strnew.c \
	ft_strchr.c \
	ft_strdup.c \
	ft_strncat.c \
	ft_strndup.c \
	ft_memcpy.c \
	ft_split.c \
	ft_strcmp.c \
	ft_strjoin.c \
	ft_strncmp.c \
	ft_strnlen.c \
	ft_memmove.c \
	ft_min.c \
	ft_strcat.c \
	ft_strcpy.c \
	ft_strlen.c \
	ft_strncpy.c \
	ft_strstr.c \
	ft_strtok.c \

LIBFT_SRC := $(addprefix libft/,$(LIBFT_SRC))
LIBFT_OBJS := $(LIBFT_SRC:libft/%.c=$(BUILDDIR)/%.o)

SRC = main.c
SRC := $(addprefix src/,$(SRC))
SRC_OBJS := $(SRC:src/%.c=$(BUILDDIR)/%.o)

OBJS = $(SRC_OBJS) $(LIBFT_OBJS)

all: $(BUILDDIR) $(NAME)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(BUILDDIR)/%.o: libft/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -I$(INCLUDE) -o $(NAME)

clean:
	rm -rf $(BUILDDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
