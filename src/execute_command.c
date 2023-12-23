/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:26:10 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:26:43 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_piped_commands(t_list *data, char **envp, int numofpipes)
{
	char	*temp;
	char	**strarr;

	temp = reassign_prompt(data->prompt);
	strarr = ft_split_quote(temp, '|');
	free(temp);
	if (check_if_pipe_is_valid(strarr, numofpipes))
		process_commands(data, envp, strarr, numofpipes);
	ft_freesplit(strarr);
	data->i = 0;
	dup2(data->stdin, STDIN_FILENO);
	dup2(data->stdout, STDOUT_FILENO);
	return (1);
}

int	execute_multiple_commands(t_list *data, char **envp, int numofpipes)
{
	char	*temp;
	int		result;

	result = 0;
	if (numofpipes)
		result = execute_piped_commands(data, envp, numofpipes);
	else
	{
		temp = reassign_prompt(data->prompt);
		ft_freesplit(data->commandsarr);
		data->commandsarr = ft_split_quote(temp, ' ');
		free(temp);
		if (data->commandsarr == NULL)
			return (0);
	}
	return (result);
}

void	executecommands(t_list *data, char **envp, int type)
{
	prepare_command_arrays(data);
	if (!check_for_redirection(data->execcmds))
		remove_quotes_from_args(data->execcmds);
	if (ft_strcmp(data->commandsarr[0], "cd") == 0)
	{
		g_exit_code = ft_cd(data->commandsarr + 1);
		return ;
	}
	manage_process_and_signals(data, type, envp);
}

void	execute_specific_command(t_list *data, char **envp)
{
	if (ft_strcmp(data->commandsarr[0], "echo") == 0)
		g_exit_code = ft_echo(data->commandsarr + 1);
	else if (ft_strcmp(data->commandsarr[0], "cd") == 0)
		g_exit_code = ft_cd(data->commandsarr + 1);
	else if (ft_strcmp(data->commandsarr[0], "pwd") == 0)
		g_exit_code = ft_pwd();
	else if (ft_strcmp(data->commandsarr[0], "export") == 0)
		ft_export(data->commandsarr[1]);
	else if (ft_strcmp(data->commandsarr[0], "unset") == 0)
		ft_unset(data->commandsarr + 1);
	else if (ft_strcmp(data->commandsarr[0], "env") == 0)
		ft_env();
	else if (ft_strcmp(data->commandsarr[0], "history") == 0)
		ft_display_history(data);
	else
		handle_external_commands(data, envp);
}

// This function may break, revert it quickly if so
void	execute_command(t_list *data, char **envp)
{
	setup_redirections(data);
	if (strcmp(data->commandsarr[0], "echo") == 0)
		ft_echo(data->commandsarr + 1);
	else if (ft_strcmp(data->commandsarr[0], "exit") == 0)
		ft_exit(data);
	else
	{
		expand_command_arguments(data->commandsarr);
		execute_specific_command(data, envp);
	}
	restore_stdout(data);
}
