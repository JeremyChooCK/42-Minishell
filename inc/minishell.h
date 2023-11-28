/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:34:32 by jegoh             #+#    #+#             */
/*   Updated: 2023/11/26 21:25:08 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <signal.h>
# include <fcntl.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <termios.h>
# include <sys/stat.h>
# include <limits.h>
# include <readline/readline.h>
# include <readline/history.h>

# define HOSTNAME_MAX 256

typedef struct s_env_var
{
	char	*key;
	char	*value;
}	t_env_var;

typedef struct s_env_list
{
	t_env_var			env_var;
	struct s_env_list	*next;
}	t_env_list;

typedef struct s_history
{
	char				*command;
	struct s_history	*next;
	struct s_history	*prev;
}	t_history;

typedef struct s_list
{
	char		*prompt;
	char		*path;
	char		**commandsarr;
	char		**execcmds;
	int			pipefd[2];
	int			stdin;
	int			stdout;
	int			i;
	int			inputfd;
	t_history	*history;
	t_env_list	*env_vars;
}	t_list;

int		checkdir(char *path);
int		checkempty(char *s);
void    executecommands(t_list *data, char **envp, int type);
void	ft_add_to_history(t_list *data, char *command);
void	ft_display_history(t_list *data);
void    ft_display_prompt(t_list *data, char **envp);
int		getcmd(t_list *data, char **envp);
char    *getpath(t_list *data);
char    *remove_dotdot_slash_and_goback_one_dir(char *s, char *cwd);
char    *removedotslash(char *s);

#endif
