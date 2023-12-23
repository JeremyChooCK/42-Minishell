/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:50:52 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:50:56 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	process_quotes(char *cmd_line)
{
	int	i;
	int	in_single_quote;
	int	in_double_quote;

	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (cmd_line[i] != '\0')
	{
		i++;
		if (cmd_line[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (cmd_line[i] == '\"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		else if (cmd_line[i] == '\\' && !in_single_quote)
		{
			if (cmd_line[i + 1] != '\0')
				i++;
			else
				return (-1);
		}
	}
	return ((in_single_quote || in_double_quote) * -1);
}

void	remove_quotes_from_arg(char *arg)
{
	int	len;

	len = ft_strlen(arg);
	if (len > 1 && ((arg[0] == '"' && arg[len - 1] == '"')
			|| (arg[0] == '\'' && arg[len - 1] == '\'')))
	{
		ft_memmove(arg, arg + 1, len - 2);
		arg[len - 2] = '\0';
	}
}

void	remove_quotes_from_args(char **args)
{
	int	i;

	i = 0;
	while (args[i] != NULL)
	{
		remove_quotes_from_arg(args[i]);
		i++;
	}
}

void	check_quotes(int *i, int *output_index, char *output)
{
	if (*output_index > 0 && output[(*output_index) - 1] != ' ')
	{
		output[(*output_index)++] = ' ';
		(*i)++;
	}
}

void	strip_quotes(char *str)
{
	int	i;
	int	j;
	int	len;

	i = 0;
	j = 0;
	len = ft_strlen(str);
	while (i < len)
	{
		if (str[i] != '\"' && str[i] != '\'')
			str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
}
