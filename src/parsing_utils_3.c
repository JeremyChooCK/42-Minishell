/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils_3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:08:18 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:08:20 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	process_command_parts(t_list *data, char **cmd_parts, int *numofpipes)
{
	if (process_and_split_command(data, &cmd_parts) < 0)
		return (-1);
	if (expand_variables_and_count_pipes(cmd_parts, data, numofpipes) < 0)
		return (-1);
	free(cmd_parts);
	return (0);
}

void	process_commands(
	t_list *data, char **envp, char **strarr, int numofpipes)
{
	int	type;

	type = 1;
	while (data->i < numofpipes + 1)
	{
		data->commandsarr = ft_split_quote(strarr[data->i], ' ');
		data->path = ft_getpath(data);
		if (data->i == numofpipes)
			type = 2;
		executecommands(data, envp, type);
		ft_freesplit(data->execcmds);
		if (data->i < numofpipes)
			ft_freesplit(data->commandsarr);
		data->execcmds = NULL;
		data->i++;
	}
}

char	*read_and_process_input(const char *delimiter, t_list *data)
{
	char	*input;

	dup2(data->stdin, 0);
	input = readline("> ");
	if (!input || ft_strcmp(input, delimiter) == 0)
	{
		if (input)
			free(input);
		return (NULL);
	}
	return (input);
}

void	reassign(t_list *data, int flag, int index)
{
	if (flag == 0)
		handle_input_redirection(data, index);
	else if (flag == 1)
		reassign_and_handle_heredoc(data, index);
	else if (flag == 2)
		handle_output_redirection(data, index);
	else if (flag == 3)
		reassign_and_handle_append(data, index);
}

void	process_split_temp(char **split_temp, int expand)
{
	int	i;

	i = -1;
	while (split_temp[++i] != NULL)
		split_temp[i] = expand_env_variables(split_temp[i], expand);
}
