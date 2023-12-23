/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_variables_utils_2.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:28:37 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:29:06 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*parse_env_var(const char *input)
{
	char	*env_value;

	if (input[0] == '$')
	{
		env_value = ft_getenv(input + 1);
		if (env_value != NULL)
			return (strdup(env_value));
		else
			return (NULL);
	}
	else
		return (ft_strdup(input));
}

t_env_list	*create_env_node(char *env_str)
{
	t_env_list	*node;
	char		*separator;
	int			key_len;

	node = malloc(sizeof(t_env_list));
	if (!node)
		return (NULL);
	separator = ft_strchr(env_str, '=');
	if (!separator)
		return (NULL);
	key_len = separator - env_str;
	node->env_var.key = ft_strndup(env_str, key_len);
	node->env_var.value = ft_strdup(separator + 1);
	node->next = NULL;
	return (node);
}

int	update_env_var(char *key, char *value, t_env_list **env_list)
{
	t_env_list	*current;

	current = *env_list;
	while (current != NULL)
	{
		if (ft_strcmp(current->env_var.key, key) == 0)
		{
			free(current->env_var.value);
			current->env_var.value = value;
			free(key);
			return (1);
		}
		current = current->next;
	}
	return (0);
}

void	add_new_env_var(char *key, char *value, t_env_list **env_list)
{
	t_env_list	*new_node;
	t_env_list	*current;
	char		*env_str;

	env_str = create_env_str(key, value);
	new_node = create_env_node(env_str);
	free(env_str);
	if (!new_node)
	{
		free(key);
		free(value);
		return ;
	}
	current = *env_list;
	if (!current)
		*env_list = new_node;
	else
	{
		while (current->next != NULL)
			current = current->next;
		current->next = new_node;
	}
	free(key);
	free(value);
}

char	*create_env_str(char *key, char *value)
{
	char	*env_str;
	char	*temp;

	env_str = ft_strjoin(key, "=");
	temp = env_str;
	env_str = ft_strjoin(env_str, value);
	free(temp);
	return (env_str);
}
