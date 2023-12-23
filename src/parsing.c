/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:02:41 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:02:52 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	parse_export_argument(char *arg, char **key, char **value)
{
	char	*separator;

	if (!arg)
		return (0);
	separator = ft_strchr(arg, '=');
	if (separator)
	{
		*key = ft_strndup(arg, separator - arg);
		*value = ft_strdup(separator + 1);
	}
	else
	{
		*key = ft_strdup(arg);
		*value = ft_strdup("");
	}
	if (!is_valid_identifier(*key))
	{
		ft_putstr_fd(" not a valid identifier\n", 2);
		free(*key);
		free(*value);
		return (0);
	}
	return (1);
}

void	parse_for_comments(char **input)
{
	char	*hash_pos;

	hash_pos = ft_strchr(*input, '#');
	if (hash_pos != NULL)
		*hash_pos = '\0';
}

char	*reassign_prompt(const char *prompt)
{
	int		new_len;
	char	*s;

	if (prompt == NULL)
		return (NULL);
	new_len = calculate_new_length(prompt);
	s = malloc(new_len + 1);
	if (!s)
		return (NULL);
	check_for_quotes_and_add_spaces(prompt, s);
	return (s);
}

void	expand_command_arguments(char **args)
{
	int	i;

	i = 0;
	while (args[i] != NULL)
	{
		args[i] = expand_env_variables(args[i], 1);
		i++;
	}
}

void	prepare_command_arrays(t_list *data)
{
	int	i;

	i = 0;
	while (data->commandsarr[i])
		i++;
	data->execcmds = malloc(sizeof(char *) * (i + 1));
	if (!data->execcmds)
		exit(EXIT_FAILURE);
	if (data->commandsarr[0] && (data->commandsarr[0][0] == '/'
		|| !ft_strncmp(data->commandsarr[0], "./", 2)))
		data->execcmds[0] = ft_strdup(data->commandsarr[0]);
	else
		data->execcmds[0] = data->path;
	if (data->execcmds[0] == NULL)
	{
		ft_putstr_fd(data->commandsarr[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		return ;
	}
	data->path = NULL;
	i = 0;
	while (data->commandsarr[++i])
		data->execcmds[i] = ft_strdup(data->commandsarr[i]);
	data->execcmds[i] = NULL;
}
