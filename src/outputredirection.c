/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   outputredirection.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:40:27 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:40:41 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	outputredirection(t_list *data)
{
	int	i;

	i = 0;
	while (data->execcmds[i])
	{
		if (ft_strcmp(data->execcmds[i], ">") == 0)
		{
			check_if_redirection_is_last_arg(data->execcmds[i + 1]);
			reassign(data, 2, i);
			i = -1;
		}
		else if (ft_strcmp(data->execcmds[i], ">>") == 0)
		{
			check_if_redirection_is_last_arg(data->execcmds[i + 1]);
			reassign(data, 3, i);
			i = -1;
		}
		i++;
	}
}
