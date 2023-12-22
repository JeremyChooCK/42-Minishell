/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_quote.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 12:25:32 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/22 20:10:11 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

int	is_delimiter(char c, char quote, char delimiter)
{
	return ((c == delimiter) && quote == 0);
}

static int	count_words(const char *str, char delimiter)
{
	int		count;
	char	quote;

	count = 0;
	quote = 0;
	while (*str)
	{
		while (*str && is_delimiter(*str, quote, delimiter))
			str++;
		if (*str)
			count++;
		while (*str && (!is_delimiter(*str, quote, delimiter) || quote))
		{
			if ((*str == '\'' || *str == '\"') && quote == 0)
				quote = *str;
			else if (*str == quote)
				quote = 0;
			str++;
		}
	}
	return (count);
}

char	*extract_word(char **str, char delimiter)
{
	char	*start;
	char	quote;
	int		length;

	quote = 0;
	length = 0;
	while (**str && is_delimiter(**str, quote, delimiter))
		(*str)++;
	start = *str;
	while (**str && (!is_delimiter(**str, quote, delimiter) || quote))
	{
		if ((**str == '\'' || **str == '\"') && quote == 0)
			quote = **str;
		else if (**str == quote)
			quote = 0;
		(*str)++;
		length++;
	}
	return (ft_strndup(start, length));
}

char	**ft_split_quote(char *s, char delimiter)
{
	int		words;
	int		i;
	char	*str;
	char	**result;

	if (!s)
		return (NULL);
	words = count_words(s, delimiter);
	result = (char **)malloc(sizeof(char *) * (words + 1));
	if (!result)
		return (NULL);
	str = s;
	i = 0;
	while (i < words)
	{
		result[i] = extract_word(&str, delimiter);
		i++;
	}
	result[words] = NULL;
	return (result);
}
