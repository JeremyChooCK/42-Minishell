/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcspn.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 10:39:28 by jechoo            #+#    #+#             */
/*   Updated: 2023/11/29 21:53:22 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

size_t	ft_strcspn(const char *s, const char *reject)
{
	size_t	count;

	count = 0;
	while (s[count] && !ft_strchr(reject, s[count]))
		++count;
	return (count);
}
