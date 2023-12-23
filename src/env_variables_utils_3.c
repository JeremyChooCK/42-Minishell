/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_variables_utils_3.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:37:48 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:37:51 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	check_env_list_and_iterate(t_env_list	**env_list,
			t_env_list	*current, t_env_list	*prev)
{
	if (prev == NULL)
		*env_list = current->next;
	else
		prev->next = current->next;
	ft_free_current(current);
}
