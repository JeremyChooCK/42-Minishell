/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:10:46 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:10:48 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_signal_cmd(int sig)
{
	g_exit_code += sig;
	if (sig == 2)
	{
		g_exit_code = 130;
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	if (sig == SIGQUIT)
	{
		ft_putstr_fd("Quit (core dumped)\n", 2);
		exit(1);
	}
}

void	ft_signal_cmd_pipe(int sig)
{
	g_exit_code += sig;
	if (sig == 2)
	{
		g_exit_code = 130;
		printf("\n");
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	manage_process_and_signals(t_list *data, int type, char **envp)
{
	int			id;
	struct stat	buff;

	ft_memset(&buff, 0, sizeof(buff));
	if (pipe(data->pipefd) == -1)
		exit(EXIT_FAILURE);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	id = fork();
	if (id == -1)
		exit(1);
	if (id == 0)
		handle_child_process(data, envp, type, buff);
	else
		handle_parent_process(data, type, id);
}
