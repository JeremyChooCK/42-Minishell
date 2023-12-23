/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_variables_utils_1.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:24:32 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:24:34 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_env(void)
{
	t_env_list	**env_list;
	t_env_list	*current;

	env_list = ft_get_adress_env();
	if (!env_list || !*env_list)
		return ;
	current = *env_list;
	while (current != NULL)
	{
		printf("%s=%s\n", current->env_var.key, current->env_var.value);
		current = current->next;
	}
}

void	add_env_node(char *env_var, t_env_list **env_list)
{
	t_env_list	*new_node;
	t_env_list	*current;

	new_node = create_env_node(env_var);
	if (new_node == NULL)
		return ;
	if (*env_list == NULL)
		*env_list = new_node;
	else
	{
		current = *env_list;
		while (current->next != NULL)
			current = current->next;
		current->next = new_node;
	}
}

void	ft_free_env_var(t_env_var *env_var)
{
	if (env_var)
	{
		free(env_var->key);
		free(env_var->value);
	}
}

void	ft_init_t_env(char **envp)
{
	int			i;
	t_env_list	**env_list;

	env_list = ft_get_adress_env();
	if (env_list != NULL)
		*env_list = NULL;
	if (envp == NULL || env_list == NULL)
		return ;
	i = 0;
	while (envp[i] != NULL)
	{
		add_env_node(envp[i], env_list);
		i++;
	}
}

void	add_to_env_list(char *key, char *value, t_env_list **env_list)
{
	if (!update_env_var(key, value, env_list))
		add_new_env_var(key, value, env_list);
}
