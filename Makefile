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
	ft_calloc.c \
	ft_freesplit.c \
	ft_putstr_fd.c \
	ft_realloc.c \
	ft_strnew.c \
	ft_strchr.c \
	ft_strdup.c \
	ft_strncat.c \
	ft_strndup.c \
	ft_memcpy.c \
	ft_memmove.c \
	ft_memset.c \
	ft_split.c \
	ft_split_quote.c \
	ft_strcmp.c \
	ft_strjoin.c \
	ft_strjoin_space.c \
	ft_strncmp.c \
	ft_strnlen.c \
	ft_min.c \
	ft_strcat.c \
	ft_strcpy.c \
	ft_strlcpy.c \
	ft_strlen.c \
	ft_strncpy.c \
	ft_strstr.c \
	ft_strtok.c \
	ft_substr.c \

LIBFT_SRC := $(addprefix libft/,$(LIBFT_SRC))
LIBFT_OBJS := $(LIBFT_SRC:libft/%.c=$(BUILDDIR)/%.o)

SRC = append.c \
	env_variables.c \
	env_variables_utils_3.c \
	exit.c \
	general_utils_1.c \
	general_utils_4.c \
	inputredirection.c \
	parsing.c \
	parsing_utils_3.c \
	quotes_utils_1.c \
	redirection_utils_2.c \
	cd.c \
	env_variables_utils_1.c \
	execute_command.c \
	export.c \
	general_utils_2.c \
	heredoc.c \
	main.c \
	outputredirection.c \
	parsing_utils_1.c \
	pathing.c \
	quotes_utils_2.c \
	signals.c \
	echo.c \
	env_variables_utils_2.c \
	execute_command_utils.c \
	free_variables.c \
	general_utils_3.c \
	history.c \
	parsing_utils_2.c \
	quotes.c \
	redirection_utils_1.c \
	unset.c
	
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
