/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes_utils_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 19:04:49 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 19:04:51 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	update_quote(int *i, char c, int *in_single_quote, int *in_double_quote)
{
	int	check;

	check = 0;
	if (c == '\'' && !*in_double_quote)
	{
		*in_single_quote = !*in_single_quote;
		(*i)++;
		check = 1;
	}
	if (c == '\"' && !*in_single_quote)
	{
		*in_double_quote = !*in_double_quote;
		(*i)++;
		check = 1;
	}
	return (check);
}
