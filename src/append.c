/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:36:17 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:36:31 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_append_redirection(char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror("Error opening file for append");
		exit(EXIT_FAILURE);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("Error redirecting stdout");
		close(fd);
		exit(EXIT_FAILURE);
	}
	close(fd);
}

void	move_forward_and_check_for_append(t_list *data, int index)
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
		|| ft_strcmp(data->commandsarr[0], ">>") == 0)
	{
		if (data->commandsarr[0] != NULL
			&& ft_strcmp(data->commandsarr[0], ">>") == 0)
			free(data->commandsarr[0]);
		data->commandsarr[0] = ft_strdup(data->execcmds[0]);
	}
}

void	reassign_and_handle_append(t_list *data, int index)
{
	char	*s;
	char	*temp;

	if (data->execcmds[index + 1] == NULL)
		return ;
	handle_append_redirection(data->execcmds[index + 1]);
	free(data->execcmds[index]);
	free(data->execcmds[index + 1]);
	move_forward_and_check_for_append(data, index);
	temp = ft_getpath(data);
	s = data->execcmds[0];
	if (temp)
	{
		if (!(access(temp, X_OK)))
		{
			data->execcmds[0] = ft_getpath(data);
			free(temp);
			free(s);
		}
		else
			free(temp);
	}
}
