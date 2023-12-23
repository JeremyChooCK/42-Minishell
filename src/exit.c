/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 18:57:10 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 18:57:13 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_exit(t_list *data)
{
	int	exit_status;

	exit_status = process_exit_args(data);
	if (exit_status == 1111)
		return ;
	else if (exit_status == 2)
	{
		ft_free_list(data);
		exit(2);
	}
	else
	{
		ft_free_list(data);
		exit(exit_status);
	}
}

void	replace_exit_status(char **command, char *exit_status)
{
	char	*new_command;
	char	*temp;
	int		i;

	i = 0;
	while (command[i] != NULL)
	{
		new_command = str_replace(command[i], "$?", exit_status);
		if (new_command != NULL)
		{
			temp = command[i];
			command[i] = new_command;
			free(temp);
		}
		i++;
	}
}

char	*validate_exit_argument(t_list *data)
{
	char	*arg;

	if (data->commandsarr[1])
	{
		if (data->commandsarr[2])
		{
			ft_putstr_fd("exit: too many arguments\n", 2);
			return (ft_strdup("-1"));
		}
		arg = ft_strdup(data->commandsarr[1]);
		if (!arg)
			return (NULL);
		strip_quotes(arg);
		if (!is_valid_number(arg))
		{
			ft_putstr_fd("exit: numeric argument required\n", 2);
			free(arg);
			return (ft_strdup("-2"));
		}
	}
	else
		arg = NULL;
	return (arg);
}

int	process_exit_args(t_list *data)
{
	int		i;
	char	*validated_arg;

	validated_arg = validate_exit_argument(data);
	if (validated_arg == NULL && data->commandsarr[1])
		return (1);
	if (validated_arg && strcmp(validated_arg, "-1") == 0)
	{
		free(validated_arg);
		g_exit_code = 1;
		return (1111);
	}
	if (validated_arg && strcmp(validated_arg, "-2") == 0)
	{
		free(validated_arg);
		return (2);
	}
	if (validated_arg)
	{
		i = ft_atoi(validated_arg);
		free(validated_arg);
		return (i);
	}
	return (0);
}
