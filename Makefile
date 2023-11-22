NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -lreadline
INCLUDE = inc
LIBFT = ft_split.c \
	ft_strcmp.c \
	ft_strjoin.c \
	ft_strlen.c \
	ft_strdup.c \

SRC = $(LIBFT) \
	main.c

SRC := $(addprefix src/,$(SRC))

OBJS = $(SRC:.c=.o)

all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -I$(INCLUDE) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
