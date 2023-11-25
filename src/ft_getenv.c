/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_getenv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 12:25:32 by jechoo            #+#    #+#             */
/*   Updated: 2023/11/25 20:40:48 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

char	**environ;

char	*ft_getenv(const char *name)
{
	size_t	name_len;
	int		i;

	if (name == NULL || environ == NULL)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (environ[i] != NULL)
	{
		if (ft_strncmp(environ[i], name, name_len) == 0
			&& environ[i][name_len] == '=')
			return (&environ[i][name_len + 1]);
		i++;
	}
	return (NULL);
}
