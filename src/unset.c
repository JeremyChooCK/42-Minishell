/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:14:20 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:14:48 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_unset(char **args)
{
	t_env_list	**env_list;
	t_env_list	*current;
	t_env_list	*prev;
	int			i;

	env_list = ft_get_adress_env();
	if (!args || !*args || !env_list || !*env_list)
		return ;
	i = 0;
	while (args[i] != NULL)
	{
		prev = NULL;
		current = *env_list;
		while (current != NULL)
		{
			if (ft_strcmp(current->env_var.key, args[i]) == 0)
			{
				check_env_list_and_iterate(env_list, current, prev);
				break ;
			}
			prev = current;
			current = current->next;
		}
		i++;
	}
}
