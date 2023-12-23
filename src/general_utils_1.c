/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general_utils_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:11:04 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:11:05 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i] != '\0')
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (i > 0);
}

void	handle_external_commands(t_list *data, char **envp)
{
	char	*cmd;

	data->path = ft_getpath(data);
	cmd = data->commandsarr[0];
	if (data->path || (cmd && (*cmd == '/' || !ft_strncmp(cmd, "./", 2))))
	{
		executecommands(data, envp, 0);
		wait(NULL);
	}
	else
	{
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": command not found\n", 2);
		g_exit_code = 127;
	}
	ft_freesplit(data->commandsarr);
	data->commandsarr = NULL;
	if (data->execcmds)
	{
		ft_freesplit(data->execcmds);
		data->execcmds = NULL;
	}
}

void	update_commands_array(t_list *data, int i)
{
	int	j;

	j = i;
	while (data->commandsarr[j + 2] != NULL)
	{
		data->commandsarr[j] = data->commandsarr[j + 2];
		j++;
	}
	data->commandsarr[j] = NULL;
}

void	handle_command(t_list *data, char **envp)
{
	parse_for_comments(&(data->prompt));
	if (checkempty(data->prompt) == 0)
	{
		ft_add_to_history(data, data->prompt);
		add_history(data->prompt);
		if (getcmd(data, envp) == 0)
			execute_command(data, envp);
	}
	else
		g_exit_code = 0;
	if (data->path)
		free(data->path);
	if (data->prompt != NULL)
		free(data->prompt);
}

void	ft_display_prompt(t_list *data, char **envp)
{
	dup2(data->stdin, STDIN_FILENO);
	dup2(data->stdout, STDOUT_FILENO);
	while (1)
	{
		data->prompt = readline("minishell$> ");
		signal(SIGINT, ft_signal_cmd);
		signal(SIGQUIT, SIG_IGN);
		if (!data->prompt)
			return ;
		handle_command(data, envp);
	}
}
