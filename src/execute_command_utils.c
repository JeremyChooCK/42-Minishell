/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:26:30 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:26:40 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	execute_buildin2(t_list *data)
{
	if (ft_strcmp(data->commandsarr[0], "unset") == 0)
	{
		ft_unset(data->commandsarr + 1);
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "env") == 0)
	{
		ft_env();
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "history") == 0)
	{
		ft_display_history(data);
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "exit") == 0)
		ft_exit(data);
}

// data->execcmds[0] for echo fix the test cases, do not touch that
void	execute_buildin(t_list *data)
{
	if (ft_strcmp(data->execcmds[0], "echo") == 0)
	{
		g_exit_code = ft_echo(data->commandsarr + 1);
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "cd") == 0)
	{
		g_exit_code = ft_cd(data->commandsarr + 1);
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "pwd") == 0)
	{
		g_exit_code = ft_pwd();
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "export") == 0)
	{
		ft_export(data->commandsarr[1]);
		exit(1);
	}
	else
		execute_buildin2(data);
}
