/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_variables.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:17:14 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:17:15 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env_list	**ft_get_adress_env(void)
{
	static t_env_list	*new = NULL;

	return (&new);
}

char	*ft_getenv(const char *name)
{
	t_env_list	**env_list;
	t_env_list	*current;

	env_list = ft_get_adress_env();
	if (!env_list || !*env_list || !name)
		return (NULL);
	current = *env_list;
	while (current)
	{
		if (ft_strcmp(current->env_var.key, name) == 0)
			return (current->env_var.value);
		current = current->next;
	}
	return (NULL);
}

char	*process_env_var(char **p, char *result, char *temp)
{
	char	var_name[NAME_SIZE];
	char	*end;
	char	*var_value;
	size_t	len;

	ft_bzero(var_name, NAME_SIZE);
	end = *p + 1;
	while (ft_isalnum((unsigned char)*end) || *end == '_')
		end++;
	ft_strncpy(var_name, *p + 1, ft_min(end - *p - 1, NAME_SIZE - 1));
	var_value = ft_getenv(var_name);
	if (var_value)
	{
		len = ft_strlen(var_value);
		temp = ft_realloc(result, (temp - result) + len + ft_strlen(end) + 1);
		if (!temp)
		{
			free(result);
			return (NULL);
		}
		ft_strcpy(temp, var_value);
		temp += len;
	}
	*p = end - 1;
	return (temp);
}

char	*expand_env_vars(char *str)
{
	char	*buffer;
	size_t	buf_size;

	if (str == NULL)
		return (NULL);
	buf_size = calculate_expansion_size(str);
	buffer = malloc(buf_size);
	if (!buffer)
		return (NULL);
	process_string(str, buffer);
	return (buffer);
}

char	*expand_env_variables(char *arg, int expand)
{
	char	*result;
	int		in_single_quote;

	in_single_quote = 0;
	if (arg == NULL)
		return (NULL);
	if (arg[0] == '\'' && arg[strlen(arg) - 1] == '\'')
		in_single_quote = 1;
	if (expand && !in_single_quote)
	{
		if (ft_strcmp(arg, "$?") == 0)
			result = ft_itoa(g_exit_code);
		else
			result = expand_env_vars(arg);
		free(arg);
		return (result);
	}
	return (arg);
}
