/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general_utils_3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:12:25 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:12:26 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_if_pipe_is_valid(char **strarr, int numofpipes)
{
	int	i;

	i = 0;
	while (strarr[i])
		i++;
	if (i != numofpipes + 1)
	{
		ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

int	getcmd(t_list *data, char **envp)
{
	char	**cmd_parts;
	int		numofpipes;
	int		result;

	cmd_parts = NULL;
	result = process_command_parts(data, cmd_parts, &numofpipes);
	if (result < 0)
		return (-1);
	return (execute_multiple_commands(data, envp, numofpipes));
}

int	open_temp_file(const char *tmpfile)
{
	int	tmpfd;

	tmpfd = open(tmpfile, O_RDWR | O_CREAT, 0644);
	if (tmpfd == -1)
	{
		perror("Error creating temporary file");
		return (-1);
	}
	return (tmpfd);
}

void	open_output_file(t_list *data, int index)
{
	data->inputfd = open(data->execcmds[index + 1],
			O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (data->inputfd == -1)
	{
		perror(data->execcmds[index + 1]);
		exit(EXIT_FAILURE);
	}
	dup2(data->inputfd, 1);
	close(data->inputfd);
	if (data->inputfd == -1)
	{
		perror(data->execcmds[index + 1]);
		exit(EXIT_FAILURE);
	}
}

char	**remove_arrow_and_outfile_strings(t_list *data)
{
	int		i;
	int		j;
	char	**result;

	i = 0;
	j = 0;
	while (data->execcmds[i])
		i++;
	result = malloc(sizeof(char *) * (i + 1 - 2));
	i = 0;
	while (data->execcmds[i])
	{
		if (ft_strcmp(data->execcmds[i], ">") == 0)
			i += 2;
		if (data->execcmds[i] == NULL)
			break ;
		result[j] = ft_strdup(data->execcmds[i]);
		i++;
		j++;
	}
	result[j] = NULL;
	return (result);
}
