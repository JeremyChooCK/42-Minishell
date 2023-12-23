/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:15:41 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:15:42 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	change_directory(char *path)
{
	char	cwd[4096];

	if (path == NULL)
		return (1);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("cd: getcwd failed");
		free(path);
		return (1);
	}
	if (chdir(path) != 0)
	{
		perror("cd");
		free(path);
		return (1);
	}
	free(path);
	return (0);
}

int	ft_cd(char **args)
{
	char	*resolved_path;

	resolved_path = validate_and_resolve_path(args);
	return (change_directory(resolved_path));
}
