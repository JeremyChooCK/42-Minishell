/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin_space.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 10:39:28 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 13:51:58 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

char	*ft_strjoin_space(char **arr)
{
	int		i;
	char	*result;
	size_t	total_length;

	if (arr == NULL)
		return (NULL);
	total_length = 0;
	i = 0;
	while (arr[i] != NULL)
		total_length += ft_strlen(arr[i++]) + 1;
	result = malloc(total_length * sizeof(char));
	if (result == NULL)
		return (NULL);
	result[0] = '\0';
	i = 0;
	while (arr[i] != NULL)
	{
		ft_strcat(result, arr[i]);
		if (arr[i + 1] != NULL)
			ft_strcat(result, " ");
		i++;
	}
	return (result);
}
