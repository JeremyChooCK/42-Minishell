/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 19:01:08 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 19:01:18 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_echo(char **args)
{
	int	i;
	int	n_flag;

	if (!args[0])
	{
		printf("\n");
		return (0);
	}
	i = process_flags(args, &n_flag);
	while (args[i])
	{
		echo_out(args, i);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (!n_flag)
		printf("\n");
	return (0);
}

void	parse_and_print_echo(char *input)
{
	int		i;
	int		in_single_quote;
	int		in_double_quote;
	char	output[MAX_INPUT_LENGTH];
	int		output_index;

	ft_memset(output, 0, sizeof(output));
	output_index = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	i = 0;
	while (i < (int)ft_strlen(input))
	{
		if (update_quote(&i, input[i], &in_single_quote, &in_double_quote))
			continue ;
		if (input[i] == ' ' && !in_single_quote && !in_double_quote)
		{
			if (output_index > 0 && output[output_index - 1] != ' ')
				check_quotes(&i, &output_index, output);
			continue ;
		}
		output[output_index++] = input[i++];
	}
	printf("%s", output);
}

void	echo_out(char **str, int pos)
{
	char	*temp;
	char	**split_temp;
	char	*joined_cmd;
	int		expand;
	int		length;

	expand = 1;
	temp = ft_strdup(str[pos]);
	if (temp == NULL)
		exit(1);
	length = ft_strlen(temp);
	if (length >= 2 && temp[0] == '\'' && temp[length - 1] == '\'')
		expand = 0;
	split_temp = ft_split(temp, ' ');
	if (split_temp == NULL)
		exit(1);
	process_split_temp(split_temp, expand);
	joined_cmd = ft_strjoin_space(split_temp);
	if (joined_cmd)
	{
		parse_and_print_echo(joined_cmd);
		free(joined_cmd);
	}
	ft_freesplit(split_temp);
	free(temp);
}
