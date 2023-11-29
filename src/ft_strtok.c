/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtok.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 10:39:28 by jechoo            #+#    #+#             */
/*   Updated: 2023/11/29 21:53:57 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

char	*ft_strtok(char *str, const char *delim)
{
	static char	*lasts;
	char		*token_end;

	if (str == NULL)
		str = lasts;
	if (str == NULL)
		return (NULL);
	str += ft_strspn(str, delim);
	if (*str == '\0')
		return (NULL);
	token_end = str + ft_strcspn(str, delim);
	if (*token_end == '\0')
		lasts = NULL;
	else
	{
		*token_end = '\0';
		lasts = token_end + 1;
	}
	return (str);
}
