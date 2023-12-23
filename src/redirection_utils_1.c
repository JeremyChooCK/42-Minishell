/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:42:45 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:42:57 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	check_if_redirection_is_last_arg(char *s)
{
	if (s == NULL)
	{
		perror("syntax error near unexpected token `newline'");
		exit(1);
	}
}

int	check_for_redirection(char	**s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (!ft_strcmp(s[i], "'<'") || !ft_strcmp(s[i], "'<<'")
			|| !ft_strcmp(s[i], "'>'") || !ft_strcmp(s[i], "'>>'")
			|| !ft_strcmp(s[i], "\"<\"") || !ft_strcmp(s[i], "\"<<\"")
			|| !ft_strcmp(s[i], "\">\"") || !ft_strcmp(s[i], "\">>\""))
			return (1);
		i++;
	}
	return (0);
}

void	handle_input_redirection(t_list *data, int index)
{
	char	*temp;
	char	*temp2;

	open_infile_and_redirect_input(data, index);
	move_commands_forward(data, index);
	free(data->commandsarr[0]);
	data->commandsarr[0] = NULL;
	data->commandsarr[0] = ft_strdup(data->execcmds[0]);
	temp = data->execcmds[0];
	temp2 = ft_getpath(data);
	if (temp2)
	{
		if (!(access(temp2, X_OK)))
		{
			data->execcmds[0] = temp2;
			free(temp);
		}
	}
	free(data->commandsarr[0]);
	data->commandsarr[0] = NULL;
}

void	handle_output_redirection(t_list *data, int index)
{
	char	*temp;
	char	*s;
	char	**result;

	open_output_file(data, index);
	result = remove_arrow_and_outfile_strings(data);
	ft_freesplit(data->execcmds);
	data->execcmds = result;
	if (data->commandsarr[0] == NULL
		|| ft_strcmp(data->commandsarr[0], ">") == 0)
	{
		if (data->commandsarr[0] != NULL
			&& ft_strcmp(data->commandsarr[0], ">") == 0)
			free(data->commandsarr[0]);
		data->commandsarr[0] = ft_strdup(data->execcmds[0]);
	}
	temp = ft_getpath(data);
	s = data->execcmds[0];
	if (!(access(temp, X_OK)))
	{
		data->execcmds[0] = ft_getpath(data);
		free(s);
	}
	free(temp);
}

void	check_for_redirection_and_close_pipe(t_list *data, int type)
{
	inputredirection(data);
	if (type == 1)
		dup2(data->pipefd[1], 1);
	if (type == 2)
		dup2(data->stdout, 1);
	outputredirection(data);
	close(data->pipefd[0]);
	close(data->pipefd[1]);
}
