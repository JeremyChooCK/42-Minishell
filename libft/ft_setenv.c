/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_setenv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 12:25:32 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/08 13:31:23 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

int	update_env_var(
	const char *name, const char *value, size_t name_len, size_t value_len)
{
	int		i;
	size_t	new_size;
	char	*new_env;

	i = 0;
	while (environ[i] != NULL)
	{
		if (ft_strncmp(environ[i], name, name_len) == 0
			&& environ[i][name_len] == '=')
		{
			new_size = name_len + value_len + 2;
			new_env = malloc(new_size);
			if (new_env == NULL)
				return (-1);
			ft_strcpy(new_env, name);
			ft_strcat(new_env, "=");
			ft_strcat(new_env, value);
			free(environ[i]);
			environ[i] = new_env;
			return (0);
		}
		i++;
	}
	return (1);
}

int	add_env_var(
	const char *name, const char *value, size_t name_len, size_t value_len)
{
	int		count;
	size_t	new_var_size;
	char	*new_var;
	char	**new_environ;

	count = 0;
	while (environ[count] != NULL)
		count++;
	new_environ = ft_realloc(environ, sizeof(char *) * (count + 2));
	if (new_environ == NULL)
		return (-1);
	new_var_size = name_len + value_len + 2;
	new_var = malloc(new_var_size);
	if (new_var == NULL)
		return (-1);
	ft_strcpy(new_var, name);
	ft_strcat(new_var, "=");
	ft_strcat(new_var, value);
	new_environ[count] = new_var;
	new_environ[count + 1] = NULL;
	environ = new_environ;
	return (0);
}

int	ft_setenv(const char *name, const char *value, int overwrite)
{
	size_t	name_len;
	size_t	value_len;

	if (name == NULL || value == NULL || strchr(name, '=') != NULL)
		return (-1);
	name_len = ft_strlen(name);
	value_len = ft_strlen(value);
	if (!overwrite || update_env_var(name, value, name_len, value_len) != 0)
		return (add_env_var(name, value, name_len, value_len));
	return (0);
}
