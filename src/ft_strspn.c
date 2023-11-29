/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strspn.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 10:39:28 by jechoo            #+#    #+#             */
/*   Updated: 2023/11/29 21:51:28 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

size_t	ft_strspn(const char *s, const char *accept)
{
	size_t	count;

	count = 0;
	while (s[count] && ft_strchr(accept, s[count]) != NULL)
		++count;
	return (count);
}
