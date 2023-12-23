/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general_utils_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:12:21 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:12:22 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_identifier(char *str)
{
	if (!str || !*str || ft_isdigit(*str))
		return (0);
	while (*str)
	{
		if (!ft_isalnum(*str) && *str != '_')
			return (0);
		str++;
	}
	return (1);
}

int	process_flags(char **args, int *n_flag)
{
	int	i;
	int	j;

	*n_flag = 0;
	i = 0;
	while (args[i] && args[i][0] == '-')
	{
		j = 1;
		while (args[i][j] == 'n')
			j++;
		if (args[i][j] == '\0')
			*n_flag = 1;
		else
			break ;
		i++;
	}
	return (i);
}

void	handle_parent_process(t_list *data, int type, int id)
{
	int	status;

	signal(SIGINT, ft_signal_cmd_pipe);
	signal(SIGQUIT, SIG_IGN);
	close(data->pipefd[1]);
	if (type == 1)
	{
		dup2(data->pipefd[0], 0);
		close(data->pipefd[0]);
	}
	else
	{
		waitpid(id, &status, 0);
		if (WIFEXITED(status))
			g_exit_code = WEXITSTATUS(status);
	}
	wait(NULL);
	signal(SIGINT, ft_signal_cmd);
	signal(SIGQUIT, SIG_IGN);
}

void	handle_child_process(
	t_list *data, char **envp, int type, struct stat buff)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	check_for_redirection_and_close_pipe(data, type);
	if (data->commandsarr[0])
		execute_buildin(data);
	if (data->execcmds[0] != NULL)
	{
		remove_quotes_from_args(data->execcmds);
		execve(data->execcmds[0], data->execcmds, envp);
		if (stat(data->execcmds[0], &buff) == 0)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(data->execcmds[0], 2);
			ft_putstr_fd(": Permission denied\n", 2);
			exit(126);
		}
	}
	g_exit_code = EXIT_FAILURE;
	exit(EXIT_FAILURE);
}

int	checkempty(char *s)
{
	size_t	i;

	i = 0;
	while ((s[i] == ' ' || s[i] == '\t') && s != NULL)
		i++;
	return (i == ft_strlen(s));
}
