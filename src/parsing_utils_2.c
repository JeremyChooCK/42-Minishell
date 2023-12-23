/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:07:06 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:07:07 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*concatenate(const char *str1, const char *str2)
{
	char	*result;

	result = malloc(ft_strlen(str1) + ft_strlen(str2) + 1);
	ft_strcpy(result, str1);
	ft_strcat(result, "/");
	ft_strcat(result, str2);
	return (result);
}

char	*str_replace(char *orig, char *rep, char *with)
{
	int	count;

	if (!orig || !rep || ft_strlen(rep) == 0)
		return (NULL);
	if (!with)
		with = "";
	count = count_substring(orig, rep);
	return (perform_replacement(orig, rep, with, count));
}

void	process_string(char *src, char *dest)
{
	char	*start;
	size_t	var_len;

	while (*src)
	{
		var_len = 0;
		start = find_next_variable(src, &var_len);
		if (start)
			handle_variable(&src, &dest, start, var_len);
		else
			*dest++ = *src++;
	}
	*dest = '\0';
}

int	process_and_split_command(t_list *data, char ***cmd_parts)
{
	if (process_quotes(data->prompt) < 0)
	{
		ft_putstr_fd("Error: Unmatched quotes in command.\n", 2);
		return (-1);
	}
	*cmd_parts = ft_split_quote(data->prompt, ' ');
	if (!*cmd_parts)
	{
		ft_putstr_fd("Error splitting command input.\n", 2);
		return (-1);
	}
	return (0);
}

int	expand_variables_and_count_pipes(
	char **cmd_parts, t_list *data, int *numofpipes)
{
	char	*expanded_cmd;
	int		i;

	i = 0;
	while (cmd_parts[i] != NULL)
	{
		expanded_cmd = expand_env_variables(cmd_parts[i], 1);
		if (expanded_cmd)
			cmd_parts[i] = expanded_cmd;
		free(expanded_cmd);
		i++;
	}
	*numofpipes = checkforpipe(data->prompt);
	return (0);
}
