/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:34:32 by jegoh             #+#    #+#             */
/*   Updated: 2023/12/18 23:23:23 by jegoh            ###   ########.fr       */
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

# define NAME_SIZE 256
# define MAX_INPUT_LENGTH 1024

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

void	ft_sigint_handler(int sig);
void	ft_sigquit_handler(int sig);
void	ft_setup_signal_handlers(void);
char	**split_and_validate_path(char *path);
char	*join_paths_and_check_access(char **splitpath, t_list *data);
char	*handle_special_cases_and_cleanup(char **splitpath, t_list *data);
void	free_splitpath(char **splitpath);
char	*ft_getpath(t_list *data);
int		checkempty(char *s);
void	executecommands(t_list *data, char **envp, int type);
void	ft_add_to_history(t_list *data, char *command);
void	ft_display_history(t_list *data);
void	ft_display_prompt(t_list *data, char **envp);
int		getcmd(t_list *data, char **envp);
void	inputredirection(t_list *data);
char	*reassign_prompt(const char *prompt);
int		ft_echo(char **args);
int		checkdir(char **args);
int		ft_pwd(void);
void	ft_export(char *arg, t_env_list **env_list);
void	ft_unset(char **args, t_env_list **env_list);
void	ft_env(t_env_list *env_vars);
void    ft_exit(t_list *data);
void	ft_export(char *arg, t_env_list **env_list);

#endif
