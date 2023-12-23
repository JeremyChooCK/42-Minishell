/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:03:40 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:03:41 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_export(char *arg)
{
	t_env_list	**env_list;
	char		*key;
	char		*value;

	env_list = ft_get_adress_env();
	if (!parse_export_argument(arg, &key, &value))
	{
		g_exit_code = 1;
		return ;
	}
	add_to_env_list(key, value, env_list);
	g_exit_code = 0;
}
