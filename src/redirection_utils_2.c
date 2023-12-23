/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils_2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:47:41 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:47:55 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_redirections(t_list *data, int i)
{
	int	flags;
	int	fd;

	data->original_stdout = dup(STDOUT_FILENO);
	flags = (O_WRONLY | O_CREAT | O_APPEND);
	if (ft_strcmp(data->commandsarr[i], ">") == 0)
		flags = (O_WRONLY | O_CREAT | O_TRUNC);
	fd = open(data->commandsarr[i + 1], flags, 0644);
	if (fd == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	data->redirection_active = 1;
}

void	setup_redirections(t_list *data)
{
	int	i;

	i = 0;
	data->redirection_active = 0;
	while (data->commandsarr[i] != NULL)
	{
		if (ft_strcmp(data->commandsarr[i], ">") == 0
			|| ft_strcmp(data->commandsarr[i], ">>") == 0)
		{
			handle_redirections(data, i);
			update_commands_array(data, i);
			break ;
		}
		i++;
	}
}

void	open_infile_and_redirect_input(t_list *data, int index)
{
	int		i;
	char	*s;

	i = 0;
	while (data->execcmds[index + 1][i] && data->execcmds[index + 1][i] != '>')
		i++;
	s = copy_infile_name(data, index, i);
	data->inputfd = open(s, O_RDONLY);
	if (data->inputfd == -1)
	{
		perror(s);
		g_exit_code = 1;
		exit(EXIT_FAILURE);
	}
	free(s);
	dup2(data->inputfd, 0);
	close(data->inputfd);
	free(data->execcmds[index]);
	free(data->execcmds[index + 1]);
}

void	restore_stdout(t_list *data)
{
	if (data->redirection_active)
	{
		dup2(data->original_stdout, STDOUT_FILENO);
		close(data->original_stdout);
		data->redirection_active = 0;
	}
}
