/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:06:01 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:06:14 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	write_to_file(int tmpfd, const char *input, const char *tmpfile)
{
	if (write(tmpfd, input, ft_strlen(input)) == -1
		|| write(tmpfd, "\n", 1) == -1)
	{
		perror("Error writing to temporary file");
		close(tmpfd);
		unlink(tmpfile);
		return (-1);
	}
	return (0);
}

int	write_user_input_to_file(
	int tmpfd, const char *delimiter, const char *tmpfile, t_list *data)
{
	char	*input;

	while (1)
	{
		input = read_and_process_input(delimiter, data);
		if (!input)
			break ;
		if (write_to_file(tmpfd, input, tmpfile) == -1)
		{
			free(input);
			return (-1);
		}
		free(input);
	}
	return (0);
}

char	*copy_infile_name(t_list *data, int index, int i)
{
	char	*s;
	int		j;

	j = 0;
	s = malloc(sizeof(char) * i + 1);
	while (j < i)
	{
		s[j] = data->execcmds[index + 1][j];
		j++;
	}
	s[j] = '\0';
	return (s);
}

void	move_commands_forward(t_list *data, int index)
{
	int	j;

	j = index;
	index++;
	while (data->execcmds[index + 1])
	{
		data->execcmds[index] = data->execcmds[index + 1];
		index++;
	}
	data->execcmds[index] = NULL;
	index = j;
	while (data->execcmds[j + 1])
	{
		data->execcmds[j] = data->execcmds[j + 1];
		j++;
	}
	data->execcmds[j] = NULL;
}

void	move_forward_and_check_for_null(t_list *data, int index)
{
	int	i;

	i = 2;
	while (data->execcmds[index + i])
	{
		data->execcmds[index] = data->execcmds[index + i];
		index++;
	}
	data->execcmds[index] = NULL;
	if (data->commandsarr[0] == NULL
		|| ft_strcmp(data->commandsarr[0], "<<") == 0)
	{
		if (data->commandsarr[0] != NULL
			&& ft_strcmp(data->commandsarr[0], "<<") == 0)
			free(data->commandsarr[0]);
		data->commandsarr[0] = ft_strdup(data->execcmds[0]);
	}
}
