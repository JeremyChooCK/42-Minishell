/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_variables.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 19:09:31 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 19:09:33 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_free_history(t_history *history)
{
	t_history	*tmp;

	while (history)
	{
		tmp = history;
		history = history->next;
		free(tmp->command);
		free(tmp);
	}
}

void	check_if_list_is_null(t_list *list)
{
	if (list)
	{
		if (list->prompt)
			free(list->prompt);
		if (list->path)
			free(list->path);
		ft_freesplit(list->commandsarr);
		ft_freesplit(list->execcmds);
		ft_free_history(list->history);
		free(list);
	}
}

void	ft_free_list(t_list *list)
{
	t_env_list	**env_list;
	t_env_list	*env_current;
	t_env_list	*env_temp;

	env_list = ft_get_adress_env();
	env_current = *env_list;
	while (env_current != NULL)
	{
		env_temp = env_current;
		env_current = env_current->next;
		free(env_temp->env_var.key);
		free(env_temp->env_var.value);
		free(env_temp);
	}
	*env_list = NULL;
	check_if_list_is_null(list);
}

char	*handle_special_cases_and_cleanup(char **splitpath, t_list *data)
{
	ft_freesplit(splitpath);
	if (ft_strcmp(data->commandsarr[0], "<") == 0
		|| ft_strcmp(data->commandsarr[0], "<<") == 0
		|| ft_strcmp(data->commandsarr[0], ">") == 0
		|| ft_strcmp(data->commandsarr[0], ">>") == 0)
		return (ft_strdup(data->commandsarr[0]));
	return (NULL);
}

void	ft_free_current(t_env_list *current)
{
	free(current->env_var.key);
	free(current->env_var.value);
	free(current);
}
