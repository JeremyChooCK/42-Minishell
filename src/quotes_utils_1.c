/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:52:22 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:52:41 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parse_quotes(
	const char *prompt, int *in_single_quote, int *in_double_quote, int i)
{
	if (prompt[i] == '\'' && (i == 0 || prompt[i - 1] != '\\'))
		*in_single_quote = !*in_single_quote;
	if (prompt[i] == '\"' && (i == 0 || prompt[i - 1] != '\\'))
		*in_double_quote = !*in_double_quote;
}

void	check_for_quotes_and_add_spaces(const char *prompt, char *s)
{
	int	in_single_quote;
	int	in_double_quote;

	in_single_quote = 0;
	in_double_quote = 0;
	add_spaces(prompt, s, &in_single_quote, &in_double_quote);
}

void	update_quote_flags(
	const char *prompt, int *in_single_quote, int *in_double_quote, int index)
{
	if (prompt[index] == '\'' && (index == 0 || prompt[index - 1] != '\\'))
		*in_single_quote = !*in_single_quote;
	if (prompt[index] == '\"' && (index == 0 || prompt[index - 1] != '\\'))
		*in_double_quote = !*in_double_quote;
}

int	calculate_new_length(const char *prompt)
{
	int	i;
	int	len;
	int	in_single_quote;
	int	in_double_quote;

	len = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	i = 0;
	while (prompt[i])
	{
		parse_quotes(prompt, &in_single_quote, &in_double_quote, i);
		if (!in_single_quote && !in_double_quote)
		{
			if (prompt[i] == '<' || prompt[i] == '>')
				len = len + 2 + (2 * (prompt[i + 1] == prompt[i]));
		}
		len++;
		i++;
	}
	return (len);
}

void	add_spaces(
	const char *prompt, char *s, int *in_single_quote, int *in_double_quote)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (prompt[i])
	{
		update_quote_flags(prompt, in_single_quote, in_double_quote, i);
		if (!*in_single_quote && !*in_double_quote
			&& (prompt[i] == '<' || prompt[i] == '>'))
		{
			s[j++] = ' ';
			s[j++] = prompt[i];
			if (prompt[i + 1] == prompt[i])
				s[j++] = prompt[i++];
			s[j++] = ' ';
		}
		else
			s[j++] = prompt[i];
		i++;
	}
	s[j] = '\0';
}
