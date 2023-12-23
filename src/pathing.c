/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:00:19 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:00:22 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*join_paths_and_check_access(char **splitpath, t_list *data)
{
	char	*temp;
	char	*joinedpath;
	int		i;

	i = 0;
	while (splitpath[i] != NULL)
	{
		temp = ft_strjoin(splitpath[i], "/");
		joinedpath = ft_strjoin(temp, data->commandsarr[0]);
		free(temp);
		if (access(joinedpath, X_OK) == 0)
			return (joinedpath);
		free(joinedpath);
		i++;
	}
	return (NULL);
}

char	*ft_getpath(t_list *data)
{
	char	**splitpath;
	char	*joinedpath;

	splitpath = ft_split(ft_getenv("PATH"), ':');
	if (!splitpath)
		return (NULL);
	joinedpath = join_paths_and_check_access(splitpath, data);
	if (joinedpath)
	{
		ft_freesplit(splitpath);
		return (joinedpath);
	}
	return (handle_special_cases_and_cleanup(splitpath, data));
}

char	*find_command_in_path(char *cmd)
{
	char	*path;
	char	*path_copy;
	char	*token;
	char	*full_path;

	path = ft_getenv("PATH");
	path_copy = ft_strdup(path);
	token = ft_strtok(path_copy, ':');
	while (token != NULL)
	{
		full_path = concatenate(token, cmd);
		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return (full_path);
		}
		free(full_path);
		token = ft_strtok(NULL, ':');
	}
	free(path_copy);
	return (NULL);
}

char	*resolve_path(char *path)
{
	if (path == NULL || ft_strcmp(path, "~") == 0)
	{
		path = ft_getenv("HOME");
		if (path == NULL)
		{
			ft_putstr_fd("cd: HOME not set\n", 2);
			return (NULL);
		}
	}
	else if (ft_strcmp(path, "-") == 0)
	{
		path = ft_getenv("OLDPWD");
		if (path == NULL)
		{
			ft_putstr_fd("cd: OLDPWD not set\n", 2);
			return (NULL);
		}
		printf("%s\n", path);
	}
	return (parse_env_var(path));
}

char	*validate_and_resolve_path(char **args)
{
	char	*path;

	if (args && args[0] && args[1])
	{
		ft_putstr_fd("cd: too many arguments\n", 2);
		path = NULL;
	}
	else
		path = args[0];
	return (resolve_path(path));
}
