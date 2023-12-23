/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:33:23 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:33:34 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_heredoc(char *delimiter, t_list *data)
{
	char	*tmpfile;
	int		tmpfd;

	tmpfile = "/tmp/heredocXXXXXX";
	tmpfd = create_and_write_heredoc(delimiter, tmpfile, data);
	if (tmpfd == -1)
		exit(EXIT_FAILURE);
	setup_heredoc_fd(tmpfd, tmpfile);
}

void	setup_heredoc_fd(int tmpfd, char *tmpfile)
{
	int	heredocfd;

	if (close(tmpfd) == -1)
	{
		perror("Error closing temporary file");
		unlink(tmpfile);
		exit(EXIT_FAILURE);
	}
	heredocfd = open(tmpfile, O_RDONLY);
	if (heredocfd == -1)
	{
		perror("Error opening temporary file for heredoc");
		unlink(tmpfile);
		exit(EXIT_FAILURE);
	}
	if (dup2(heredocfd, STDIN_FILENO) == -1)
	{
		perror("Error redirecting stdin");
		close(heredocfd);
		unlink(tmpfile);
		exit(EXIT_FAILURE);
	}
	close(heredocfd);
	unlink(tmpfile);
}

int	create_and_write_heredoc(char *delimiter, const char *tmpfile, t_list *data)
{
	int	tmpfd;

	tmpfd = open_temp_file(tmpfile);
	if (tmpfd == -1)
		return (-1);
	if (write_user_input_to_file(tmpfd, delimiter, tmpfile, data) == -1)
	{
		close(tmpfd);
		return (-1);
	}
	return (tmpfd);
}

void	reassign_and_handle_heredoc(t_list *data, int index)
{
	char	*temp;
	char	*s;

	if (data->execcmds[index + 1] == NULL)
		return ;
	handle_heredoc(data->execcmds[index + 1], data);
	free(data->execcmds[index]);
	free(data->execcmds[index + 1]);
	move_forward_and_check_for_null(data, index);
	temp = ft_getpath(data);
	s = data->execcmds[0];
	if (!(access(temp, X_OK)))
	{
		data->execcmds[0] = ft_getpath(data);
		free(temp);
		free(s);
	}
	else
		free(temp);
}
